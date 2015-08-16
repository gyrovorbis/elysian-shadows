#include "elysian_collision_system.hpp"
#include <crtp/elysian_collidable.hpp>
#include "crtp/elysian_camera_system.hpp"
#include <crtp/elysian_level.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_entity.hpp>
#include "elysian_debug.hpp"
#include "elysian_timer_system_base.hpp"
#include <level/elysian_zcoords.hpp>
#include <crtp/elysian_depth_buff_entry.hpp>
#include <crtp/elysian_map_system.hpp>
#include "elysian_party_system.hpp"

/*
    Welcome, motherfuckers, to hell. Where abstract math meets low-level optimization.
    Where structural C algorithms meet encapsulated C++ datatypes.
    Where real men struggle to stand tall and bitches weep...
    --FG
*/

namespace elysian {

CollisionSystem::CollisionSystem(void):
    _detectionMonitor("detectEntities"),
    _tileDetectMonitor("tiles"),
    _entityDetectMonitor("entities")
{
    Collidable::_colSys = this;
    _sortSweepPool.initialize();
}

//This is a pretty DAMN expensive operation
void CollisionSystem::render(void) {
    renderTileColliders();
    if(_renderLayer != RENDER_DISABLED) {
        for(unsigned i = 0; i < _curArea->collidables.size(); ++i) {
            _curArea->collidables[i]->render();
        }
    }
}

void CollisionSystem::update(void) {
	if(!_enabled) return;

    for(unsigned i = 0; i < _curArea->collidables.size(); ++i) {
        if(!_curArea->collidables[i]->getSortSweepAABB()) {
            SortSweepCollidable* sweep = &_sortSweepPool.claimFreeEntry();
            sweep->collidable = _curArea->collidables[i];
            _curArea->collidables[i]->setSortSweepAABB(sweep);
        }
    }

	//prepareGeometry();
	detectCollisions();
    resolvePenetration();
    _checkTriggers();
}

void CollisionSystem::_checkTriggers(void) {

    for(unsigned i = 0; i < _contactAmount; ++i) {
        CollisionContact &curContact = _contactPool[i];
        uint32_t curMask = 0;

        if(curContact._eventMonitorMask) {
            bool prevCollision = false;
            if(previouslyColliding(*curContact._bodyA, *curContact._bodyB)) {
                prevCollision = true;
                //break;
            }

            for(unsigned i = 0; i < 32; ++i) {
                if( !(curMask = (curContact._eventMonitorMask & (1<<i))) ) continue;
                auto* monitor = _contactMonitors[curMask];
                if(!monitor) continue;

                if(curMask & curContact._bodyA->_eventMonitorMask) {
                    if(!prevCollision) monitor->onCollisionEnter(curContact._bodyA, curContact._bodyB, &curContact);
                    else monitor->onCollide(curContact._bodyA, curContact._bodyB, &curContact);
                }
                if(curMask & curContact._bodyB->_eventMonitorMask) {
                    if(!prevCollision) monitor->onCollisionEnter(curContact._bodyB, curContact._bodyA, &curContact);
                    else monitor->onCollide(curContact._bodyB, curContact._bodyA, &curContact);
                }
            }
        }
    }


    for(unsigned i = 0; i < _prevContactAmount; ++i) {
        Collidable& colA = *_prevContactPool[i].collidable[0];
        Collidable& colB = *_prevContactPool[i].collidable[1];
        uint32_t curMask = 0;

        if(!checkCollision(colA, colB)) {
            for(unsigned i = 0; i < 32; ++i) {
                curMask = (1<<i);
                auto* monitor = _contactMonitors[curMask];
                if(!monitor) continue;
                if(curMask & colA._eventMonitorMask) monitor->onCollisionExit(&colA, &colB);
                if(curMask & colB._eventMonitorMask) monitor->onCollisionExit(&colB, &colA);
            }
        }
    }

    _prevContactAmount = 0;
    for(unsigned i = 0; i < _contactAmount; ++i) {
        if(_contactPool[i]._eventMonitorMask) {
            _prevContactPool[_prevContactAmount].collidable[0] = _contactPool[i]._bodyA;
            _prevContactPool[_prevContactAmount].collidable[1] = _contactPool[i]._bodyB;
            ++_prevContactAmount;
        }
    }
}

void CollisionSystem::prepareGeometry(void) {
    gyMatPush();
    gyMatLoad(&getCameraSys()->getCurrentState()->viewMat);


    unsigned size = _curArea->collidables.size();
    for(unsigned i = 0; i < size; ++i) {
        assert(_curArea->collidables[i]->getAttrib());
        _curArea->collidables[i]->update();
    }

    gyMatPop();
}

void CollisionSystem::_applyTileColliderTransform(const Tile::COLLIDER_TYPE colType) const {
    switch(colType) {
    case Tile::COLLIDER_HALF_S:
        gyMatTranslateScale(0.0f, 0.25f, 0.0f, 1.0f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_HALF_W:
        gyMatTranslateScale(-0.25f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
        break;
    case Tile::COLLIDER_HALF_E:
        gyMatTranslateScale(0.25f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
        break;
    case Tile::COLLIDER_HALF_N:
        gyMatTranslateScale(0.0f, -0.25f, 0.0f, 1.0f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_QUAD_NE:
        gyMatTranslateScale(0.25f, -0.25f, 0.0f, 0.5f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_QUAD_NW:
        gyMatTranslateScale(-0.25f, -0.25f, 0.0f, 0.5f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_QUAD_SW:
        gyMatTranslateScale(-0.25f, 0.25f, 0.0f, 0.5f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_QUAD_SE:
        gyMatTranslateScale(0.25f, 0.25f, 0.0f, 0.5f, 0.5f, 1.0f);
        break;
    case Tile::COLLIDER_45DEG_NE:
        gyMatRotateZ((1.0f/2.0f)*M_PI); //90 deg
        break;
    case Tile::COLLIDER_45DEG_SW:
        gyMatRotateZ((3.0f/2.0f)*M_PI); //270 deg
        break;
    case Tile::COLLIDER_45DEG_NW:
        gyMatRotateZ(M_PI); // 180 deg
        break;
    default:
        break;
    }
}

//HARD-CODED TO ONE LAYER
void CollisionSystem::renderTileColliders(void) const {

    static const GYVector4 boxLocalVerts[4] = {
        { -0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f, -0.5f, 0.0f, 1.0f },
        { -0.5f, 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 0.0f, 1.0f }
    };
    static const GYVector4 triLocalVerts[3] = {
        { 0.5f, -0.5f, 0.0f, 1.0f },
        { -0.5f, 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 0.0f, 1.0f }
    };
    static const GYColor4 boxColors[4] = {
        { 0, 0, 255, 128 },
        { 0, 0, 255, 128 },
        { 0, 0, 255, 128 },
        { 0, 0, 255, 128 }
    };
    static const GYColor4 triColors[3] = {
        { 0, 0, 255, 128 },
        { 0, 0, 255, 128 },
        { 0, 0, 255, 128 }
    };
    static const GYColor4 zTranColor = {
        255, 255, 0, 128
    };

    //Check if we're even rendering solidity. If not, return.
    if(_renderLayer == RENDER_DISABLED) return;
    //Check if there's even an active player for solidity to be rendered relative to
    if(!getPartySys()->getPartyLeader()) return;

    int startX, startY, endX, endY;
    unsigned playerElev = getPartySys()->getPartyLeader()->getElev();
    //Collidable &col = *getPartySys()->getPartyLeader()->getCollidable();

    gyVidMaterialBind(nullptr);
    gyVidSpecularMapBind(nullptr);
    gyVidNormalMapBind(nullptr);
    gyVidTexBind(nullptr);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidQuadListBegin(boxColors);
    getMapSys()->getOnscreenTileRegion(startX, startY, endX, endY);
    for(int y = startY; y <= endY; ++y) {
        for(int x = startX; x <= endX; ++x) {
            unsigned startIndex, stopIndex;
            Tile::COLLIDER_TYPE colType = Tile::COLLIDER_NONE;

            //Retrieve current colliderType at map location
            switch(_renderLayer) {
            case RENDER_LAYER_1:
                startIndex = stopIndex = 0;
                break;
            case RENDER_LAYER_2:
                startIndex = stopIndex = 1;
                break;
            case RENDER_LAYER_3:
                startIndex = stopIndex = 2;
                break;
            case RENDER_COMBINED:
                startIndex = 0; stopIndex = 2;
                break;
            default: assert(0);
            }

            //bool isOnZPlane = false;
            bool isZTran = false;

            //Iterate over interested layer(s)
            for(unsigned i = startIndex; i <= stopIndex; ++i) {
                TileEntry tileEntry = _curArea->getTileEntry(i, x, y);

                //Only look at colliders on the entity's z-plane
                if(tileEntry.getElev() > playerElev + 1 ||
                    tileEntry.getElev() < playerElev) continue;

                Tile::COLLIDER_TYPE colType2 = _curLevel->tileset[tileEntry.getIndex()].colliderType;
                colType2 = Tile::reorientCollider(colType2, tileEntry.getOrient());
                //Combine and accumulate
                colType = Tile::combineColliders(colType, colType2);
                //isOnZPlane = true;

                if(tileEntry.getZTran() && tileEntry.getElev() == playerElev) isZTran = true;
            }


//			if(!isOnZPlane && !col._onZTran && col.getZTranState() != Collidable::ZTRAN_ACTIVE) colType = Tile::COLLIDER_FULL;
            if(colType == Tile::COLLIDER_NONE && !isZTran) continue;

            gyMatPush();
            gyMatTranslateScale(x*32.0f, y*32.0f + 50.0f/*ZCoordConstants::tileDecor*/, 50.0f/*ZCoordConstants::tileDecor*/, 32.0f, 32.0f, 32.0f);

            if(colType == Tile::COLLIDER_NONE) {
                gyMatPop();
                continue;
            }

            switch(colType) {
            case Tile::COLLIDER_L_NE:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_HALF_N);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
                //gyVidColorArray(boxColors);
                gyVidRect(0);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                break;
            case Tile::COLLIDER_L_NW:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_HALF_N);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
                gyVidRect(0);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                break;
            case Tile::COLLIDER_L_SW:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_HALF_S);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                break;
            case Tile::COLLIDER_L_SE:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_HALF_S);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                break;
            case Tile::COLLIDER_QUADS_SE_NW:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                break;
            case Tile::COLLIDER_QUADS_SW_NE:
                gyMatPush();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                gyMatPop();
                _applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                gyVidRect(0);
                break;
            case Tile::COLLIDER_FULL:
            case Tile::COLLIDER_HALF_S:
            case Tile::COLLIDER_HALF_W:
            case Tile::COLLIDER_HALF_E:
            case Tile::COLLIDER_HALF_N:
            case Tile::COLLIDER_QUAD_NE:
            case Tile::COLLIDER_QUAD_NW:
            case Tile::COLLIDER_QUAD_SE:
            case Tile::COLLIDER_QUAD_SW:
                _applyTileColliderTransform(colType);
                gyVidRect(0);
                //gyVidColorArray(boxColors);
                //gyVidTriangleStrip(boxLocalVerts, 0, 4);
                break;
            case Tile::COLLIDER_45DEG_NE:
            case Tile::COLLIDER_45DEG_NW:
            case Tile::COLLIDER_45DEG_SW:
            case Tile::COLLIDER_45DEG_SE:
                //_applyTileColliderTransform(colType);
                //gyVidColorArray(triColors);
              //  gyVidTriangleStrip(triLocalVerts, 0, 3);
                break;
            default: assert(0);
            }
            gyMatPop();
        }
    }

    gyVidQuadListEnd();
}


//ONLY HANDLES RECTS... why?
void CollisionSystem::renderEntities(void) {
    for(std::vector<Collidable *>::const_iterator it = _curArea->collidables.begin();
        it != _curArea->collidables.end();
        ++it)
    {
        (*it)->render();
    }
}

void CollisionSystem::detectCollisions(void) {
    unsigned i = 0;

	getTimerSys()->pushTaskMonitor(&_detectionMonitor);

	//reset contact counters
	_contactAmount = 0;
	_entityEntityChecks = 0;
	for(; i < _curArea->collidables.size(); ++i) _curArea->collidables[i]->_contactCount = 0;

    if(getEngineState() != System::ENGINE_ESTK_STOP) {

		getTimerSys()->pushTaskMonitor(&_tileDetectMonitor);

		//Entity v Tile Collision Detection
        for(i = 0; i < _curArea->collidables.size(); ++i) {
            if(!_curArea->collidables[i]->_active || !_curArea->collidables[i]->_worldSpace) continue;
            _detectEntityTileZCollisionCoarse(*_curArea->collidables[i]);
            if(_curArea->collidables[i]->getAttrib()->getFlags()->getTypeAgainst().getValue() & CollisionFlags::TILE) {
                _detectEntityTileCollisionCoarse(*_curArea->collidables[i]);
            }
        }

		getTimerSys()->popTaskMonitor(&_tileDetectMonitor);

		//Entity v Entity Collision Detection
		getTimerSys()->pushTaskMonitor(&_entityDetectMonitor);

		if(!_broadPhase) {
			for(i = 0; i < _curArea->collidables.size(); ++i) {
                detectEntityCollision(i);
			}
		} else {
			_sortSweepPool.sortAndSweep();
            for(unsigned i = 0; i < _sortSweepPool.getContactCount(); ++i) {
                _detectEntityEntityCollision(_sortSweepPool.getContactBodyA(i)->getEntity(), _sortSweepPool.getContactBodyB(i)->getEntity());
            }

		}

		getTimerSys()->popTaskMonitor(&_entityDetectMonitor);

    }

    //Check GUI-layer collidables
    for(i = 0; i < _curArea->collidables.size(); ++i) {
        if(!_curArea->collidables[i]->_active || _curArea->collidables[i]->_worldSpace) continue;
        for(unsigned j = i+1; j < _curArea->collidables.size(); ++j) {
            detectCollidableCollidableCollision(_curArea->collidables[i], _curArea->collidables[j]);
        }
    }
    //_sortSweepPool.sortAndSweep();


	getTimerSys()->popTaskMonitor(&_detectionMonitor);
}

void CollisionSystem::_detectEntityTileZCollisionCoarse(Collidable& /*collidable*/) {
#if 0
    GYVector3 worldPos = collidable.getCenterWorldPos();
    float zTop = 0.0f;
    bool found = false;

    const unsigned elev = /*(worldPos.z == 0.0f)? 0 :*/ (worldPos.z)/32;
    float zOffset = worldPos.z - elev*32.0f;
    worldPos.y += 32.0f + zOffset;
    /*it->getEntity()->getCollidable()->getAttrib()->getRelSize().y*
            it->getEntity()->transform._size.y * it->getEntity()->transform._scale.y;*/


        for(int i = 0; i < 3; ++i) {
            for(unsigned j = 0; j < 1; ++j) {
                //if(found) break;

            TileEntry entry = getCurrentArea()->getTileEntry(i, (worldPos.x+16.0f)/32, (worldPos.y)/32/*+elev*/-1/*-1*/);


            //if(!(j == 0 && entry.getNormal()) && !(j == 0 && !entry.getNormal())) {


                if(getCurLevel()->getTile(entry.getIndex())->standOn) {

                    if(entry.getNormal() && ((int)worldPos.y+16)/32 > ((int)worldPos.y/32)) continue;


                    float newTop = (entry.getElev()*32 + (entry.getNormal()? 32.0f : 0.0f) );


                    if(newTop > zTop && (fabs(newTop-worldPos.z) <= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX)) {
                        zTop = newTop;
                        //found = true;
                    }
                }
            //}
            }

        }

        if((collidable.getEntity()->transform._pos.z <= zTop)) {
            _contactPool[_contactAmount]._bodyA = &collidable;
            _contactPool[_contactAmount].staticAttrib[0] = collidable.getAttrib()->getStaticAttrib();
            _contactPool[_contactAmount].staticAttrib[1] = nullptr;
            _contactPool[_contactAmount]._bodyB = nullptr;
            _contactPool[_contactAmount]._eventMonitorMask = 0;
            _contactPool[_contactAmount].depth = 0.0f;
            _contactPool[_contactAmount].setZDepth(worldPos.z-zTop-1.0f);
            _contactPool[_contactAmount++]._resolvePen = collidable._resolvePen;
            ++collidable._contactCount;
            if(_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) assert(0);
           // collidable.update();
        }


    //}

#endif
}

bool CollisionSystem::detectCollidableCollidableCollision(Collidable* col1, Collidable* col2) {
    Collidable *bodyA = col1;
    Collidable *bodyB = col2;
    bool contact = false;
    int swapFlag;
    float aBot = bodyA->getCollider()->worldPos.z;
    float aTop = aBot + bodyA->getWorldSize().z;

    float bBot = bodyB->getCollider()->worldPos.z;
    float bTop = bBot + bodyB->getWorldSize().z;

    float zBot = -1.0f;
    float zTop = -1.0f;
    bool zTopFound = false;
    bool zBotFound = false;

    bool worldSpace = bodyA->_worldSpace;

    gyColliderCurrent(&bodyA->_collider.col, bodyA->getAttrib()->getStaticAttrib());

    //Return if collision flags don't match OR
    //if both are on different z-planes.
    if(!(bodyA->getAttrib()->flags.getTypeAgainst().getValue() & bodyB->getAttrib()->flags.getType().getValue())
            /* || bodyA->getEntity()->getElev() != bodyB->getEntity()->getElev() */) return false;

    //worldSpace checks in 3D, screen checks in 2D
    if(worldSpace) {

    //solve for overlapping z/elevation region

    if(aTop >= bBot && aTop <= bTop) {
        zTopFound = true;
        zTop = aTop;
    }
    else if(bTop >= aBot && bTop <= aTop) {
        zTop = bTop;
        zTopFound = true;
    }

    if(aBot <= bTop && aBot >= bBot) {
        zBotFound = true;
        zBot = aBot;
    }
    else if(bBot <= aTop && bBot >= aBot) {
        zBotFound = true;
        zBot = bBot;
    }

    //No overlap!
    if(!zTopFound || !zBotFound) return false;

    if(zBot == aBot) {
        //adjustments go to bodyB
        float zOffset = aBot - bBot;
        if(zOffset > bodyB->getWorldSize().z) zOffset = bodyB->getWorldSize().z;
        if(bodyB->getCollider()->type == GY_COLLIDER_AABB || bodyB->getCollider()->type == GY_COLLIDER_OBB) {
            for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyB->getCollider())->worldVert[i].y -= zOffset;
        }
    } else if(zBot == bBot) {
        //adjustments go to bodyA
        float zOffset = bBot - aBot;
        if(zOffset > bodyA->getWorldSize().z) zOffset = bodyA->getWorldSize().z;
        if(bodyA->getCollider()->type == GY_COLLIDER_AABB || bodyA->getCollider()->type == GY_COLLIDER_OBB) {
            for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyA->getCollider())->worldVert[i].y -= zOffset;
        }
    }

    }


    if(gyCollision(&bodyB->_collider.col, bodyB->getAttrib()->getStaticAttrib(), &_contactPool[_contactAmount], &swapFlag)) {
        contact = true;
        ++bodyA->_contactCount;
        ++bodyB->_contactCount;

       float zDiff;// =  (zTop == aTop)? (aTop-bBot) : -((bTop-aBot));

        if(!swapFlag) {
            _contactPool[_contactAmount]._bodyA = bodyA;
            _contactPool[_contactAmount]._bodyB = bodyB;
if(worldSpace) {
            if(zTop == aTop && zBot == bBot) {
                zDiff = aTop-bBot;
            } else if(zTop == bTop && zBot == aBot) {
                zDiff = aBot-bTop;
            } else zDiff = 100.0f;
}

        }
        else {
            _contactPool[_contactAmount]._bodyB = bodyA;
            _contactPool[_contactAmount]._bodyA = bodyB;
if(worldSpace) {
            if(zTop == aTop && zBot == bBot) {
                zDiff = aBot-bTop;
            } else if(zTop == bTop && zBot == aBot) {
                zDiff = aTop-bBot;
            }
            else zDiff = 100.0f;
        }
        }

        _contactPool[_contactAmount]._resolvePen		= bodyA->_resolvePen && bodyB->_resolvePen;
        _contactPool[_contactAmount]._eventMonitorMask	= (bodyA->_eventMonitorMask | bodyB->_eventMonitorMask);

    //    float zDiff = zTop-zBot;

        _contactPool[_contactAmount].setZDepth(worldSpace? zDiff : 0.0f);

        //Emit onCollide trigger for any entitjjies with this component
        //getBehavSys()->onCollide(bodyA->getEntity(), bodyB);
        //getBehavSys()->onCollide(bodyB->getEntity(), bodyA)


        if(++_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) {
            assert(0);
            return contact; //MAXXED OUT -- NOT GOOD ENOUGH
        }
    }

    if(worldSpace) {
    if(zBot == aBot) {
        //adjustments go to bodyB
        float zOffset = aBot - bBot;
        if(zOffset > bodyB->getWorldSize().z) zOffset = bodyB->getWorldSize().z;
        if(bodyB->getCollider()->type == GY_COLLIDER_AABB || bodyB->getCollider()->type == GY_COLLIDER_OBB) {
            for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyB->getCollider())->worldVert[i].y += zOffset;
        }
    } else if(zBot == bBot) {
        //adjustments go to bodyA
        float zOffset = bBot - aBot;
        if(zOffset > bodyA->getWorldSize().z) zOffset = bodyA->getWorldSize().z;
        if(bodyA->getCollider()->type == GY_COLLIDER_AABB || bodyA->getCollider()->type == GY_COLLIDER_OBB) {
            for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyA->getCollider())->worldVert[i].y += zOffset;
        }
    }
    }

    return contact;
}

bool CollisionSystem::_detectEntityEntityCollision(Entity * const entity1, Entity * const entity2) {
    Collidable* bodyA = entity1->getCollidable();
    Collidable* bodyB = entity2->getCollidable();
    int swapFlag;
    float aBot, aTop;
    float bBot, bTop;
    float zBot, zTop;
    bool zTopFound, zBotFound;
    bool retVal = false;

	_entityEntityChecks++;

	if(!bodyB || !bodyA) return false;

    gyColliderCurrent(&bodyA->_collider.col, bodyA->getAttrib()->getStaticAttrib());

    bool worldSpace = bodyA->_worldSpace;

    //Return if collision flags don't match OR
    //if both are on different z-planes.
    if(bodyB->_worldSpace != worldSpace) return false;
    if(!(bodyA->getAttrib()->flags.getTypeAgainst().getValue() & bodyB->getAttrib()->flags.getType().getValue())
            /* || bodyA->getEntity()->getElev() != bodyB->getEntity()->getElev() */) return false;

    if(worldSpace) {
        //solve for overlapping z/elevation region
        aBot = bodyA->getCollider()->worldPos.z;
        aTop = aBot + bodyA->getWorldSize().z;

        bBot = bodyB->getCollider()->worldPos.z;
        bTop = bBot + bodyB->getWorldSize().z;

        zBot = -1.0f;
        zTop = -1.0f;
        zTopFound = false;
        zBotFound = false;

        if(aTop >= bBot && aTop <= bTop) {
            zTopFound = true;
            zTop = aTop;
        }
        else if(bTop >= aBot && bTop <= aTop) {
            zTop = bTop;
            zTopFound = true;
        }

        if(aBot <= bTop && aBot >= bBot) {
            zBotFound = true;
            zBot = aBot;
        }
        else if(bBot <= aTop && bBot >= aBot) {
            zBotFound = true;
            zBot = bBot;
        }

        //No overlap!
        if(!zTopFound || !zBotFound) return false;

        if(zBot == aBot) {
            //adjustments go to bodyB
            float zOffset = aBot - bBot;
            if(zOffset > bodyB->getWorldSize().z) zOffset = bodyB->getWorldSize().z;
            if(bodyB->getCollider()->type == GY_COLLIDER_AABB || bodyB->getCollider()->type == GY_COLLIDER_OBB) {
              //  for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyB->getCollider())->worldVert[i].y -= zOffset;
            }
        } else if(zBot == bBot) {
            //adjustments go to bodyA
            float zOffset = bBot - aBot;
            if(zOffset > bodyA->getWorldSize().z) zOffset = bodyA->getWorldSize().z;
            if(bodyA->getCollider()->type == GY_COLLIDER_AABB || bodyA->getCollider()->type == GY_COLLIDER_OBB) {
              //  for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyA->getCollider())->worldVert[i].y -= zOffset;
            }
        }

    }

    if(gyCollision(&bodyB->_collider.col, bodyB->getAttrib()->getStaticAttrib(), &_contactPool[_contactAmount], &swapFlag)) {
        ++bodyA->_contactCount;
        ++bodyB->_contactCount;

       float zDiff;// =  (zTop == aTop)? (aTop-bBot) : -((bTop-aBot));

        if(!swapFlag) {
            _contactPool[_contactAmount]._bodyA = bodyA;
            _contactPool[_contactAmount]._bodyB = bodyB;

            if(worldSpace) {
            if(zTop == aTop && zBot == bBot) {
                zDiff = aTop-bBot;
            } else if(zTop == bTop && zBot == aBot) {
                zDiff = aBot-bTop;
            } else zDiff = 100.0f;
            }

        }
        else {
            _contactPool[_contactAmount]._bodyB = bodyA;
            _contactPool[_contactAmount]._bodyA = bodyB;

            if(worldSpace) {
            if(zTop == aTop && zBot == bBot) {
                zDiff = aBot-bTop;
            } else if(zTop == bTop && zBot == aBot) {
                zDiff = aTop-bBot;
            } else zDiff = 100.0f;
            }
        }

        _contactPool[_contactAmount]._resolvePen		= (bodyA->_resolvePen && bodyB->_resolvePen);
        _contactPool[_contactAmount]._eventMonitorMask	= (bodyA->_eventMonitorMask | bodyB->_eventMonitorMask);

    //    float zDiff = zTop-zBot;

        _contactPool[_contactAmount].setZDepth(worldSpace? zDiff : 0.0f);
        retVal = true;

        //Emit onCollide trigger for any entitjjies with this component
        //getBehavSys()->onCollide(bodyA->getEntity(), bodyB);
        //getBehavSys()->onCollide(bodyB->getEntity(), bodyA)


        if(++_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) {
            _contactAmount -= 2;
            return false;
        }
    }

    if(worldSpace) {
        if(zBot == aBot) {
            //adjustments go to bodyB
            float zOffset = aBot - bBot;
            if(zOffset > bodyB->getWorldSize().z) zOffset = bodyB->getWorldSize().z;
            if(bodyB->getCollider()->type == GY_COLLIDER_AABB || bodyB->getCollider()->type == GY_COLLIDER_OBB) {
              //  for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyB->getCollider())->worldVert[i].y += zOffset;
            }
        } else if(zBot == bBot) {
            //adjustments go to bodyA
            float zOffset = bBot - aBot;
            if(zOffset > bodyA->getWorldSize().z) zOffset = bodyA->getWorldSize().z;
            if(bodyA->getCollider()->type == GY_COLLIDER_AABB || bodyA->getCollider()->type == GY_COLLIDER_OBB) {
               // for(unsigned i = 0; i < 4; ++i) ((GYBoxCollider*)bodyA->getCollider())->worldVert[i].y += zOffset;
            }
        }
    }
    return retVal;
}

//Looks correct algorithmically.
void CollisionSystem::detectEntityCollision(const unsigned i) {
    
	for(unsigned j = i+1; j < _curArea->collidables.size(); ++j) {
        _detectEntityEntityCollision(_curArea->collidables[i]->getEntity(), _curArea->collidables[j]->getEntity());
	}
}


//FOR FUCKING TILES
inline void CollisionSystem::_detectCollidableColliderCollision(Collidable &collidable, GYCollider &collider) {
    int swapFlag;

    gyColliderCurrent(&collidable._collider.col, collidable.getAttrib()->getStaticAttrib());
    if(gyCollision(&collider, nullptr, &_contactPool[_contactAmount], &swapFlag)) {
        _contactPool[_contactAmount]._bodyA = &collidable;
        _contactPool[_contactAmount]._bodyB = nullptr;
        _contactPool[_contactAmount]._eventMonitorMask = 0;
        _contactPool[_contactAmount].setZDepth(ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX+1.0f);
        _contactPool[_contactAmount++]._resolvePen = collidable._resolvePen;
        ++collidable._contactCount;
        if(_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) assert(0);
    }
}


void CollisionSystem::_detectEntityTileCollisionFine(Collidable &collidable, const unsigned x, const unsigned y, const unsigned char elev) {
    GYBoxCollider boxCollider;
    TileEntry tileEntry[3];
    Tile::COLLIDER_TYPE colType = Tile::COLLIDER_NONE;

    tileEntry[0] = _curArea->getTileEntry(0, x, y);
    tileEntry[1] = _curArea->getTileEntry(1, x, y);
    tileEntry[2] = _curArea->getTileEntry(2, x, y);

        //Iterate over tile layers and combine their colliders
        for(unsigned i = 0; i < 3; ++i) {

    #if 0
            if(tileEntry[i].getElev() > EntityElev + 1 ||
                tileEntry[i].getElev() < EntityElev) continue;
    #else
            if(tileEntry[i].getElev() != elev) continue;
    #endif

            //if((collidable._onZTran || collidable.getZTranState() == Collidable::ZTRAN_ACTIVE) && tileEntry[i].getElev() != EntityElev) continue;
            Tile::COLLIDER_TYPE colType2 = _curLevel->tileset[tileEntry[i].getIndex()].colliderType;
            colType2 = Tile::reorientCollider(colType2, tileEntry[i].getOrient());
            //Combine and accumulate
            colType = Tile::combineColliders(colType, colType2);
        }

        //Do not allow collider to move to location without a tile at its elevation
        //UNLESS he's z-transitioning!
        //if(!isTileAtElev && !collidable._onZTran && collidable.getZTranState() != Collidable::ZTRAN_ACTIVE) colType = Tile::COLLIDER_FULL;

        //Will not always be the case!
        boxCollider.collider.type = GY_COLLIDER_AABB;

        //SKETCHY!!!
        //boxCollider.collider.worldPos.x = x*32.0f;
        //boxCollider.collider.worldPos.y = y*32.0f;

        switch(colType) {
        case Tile::COLLIDER_NONE:
            return;
        case Tile::COLLIDER_L_NE:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_HALF_N);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_L_NW:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_HALF_N);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_L_SW:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_HALF_S);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_L_SE:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_HALF_S);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_QUADS_SE_NW:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_QUADS_SW_NE:
            gyMatPush();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            gyMatPop();
            _applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        case Tile::COLLIDER_FULL:
        case Tile::COLLIDER_HALF_S:
        case Tile::COLLIDER_HALF_W:
        case Tile::COLLIDER_HALF_E:
        case Tile::COLLIDER_HALF_N:
        case Tile::COLLIDER_QUAD_NE:
        case Tile::COLLIDER_QUAD_NW:
        case Tile::COLLIDER_QUAD_SE:
        case Tile::COLLIDER_QUAD_SW:
            _applyTileColliderTransform(colType);
            gyCalcBoxWorldVert(&boxCollider);
            _detectCollidableColliderCollision(collidable, boxCollider.collider);
            break;
        default:
            break;
            assert(0);
        }
}

void CollisionSystem::_detectEntityTileCollisionCoarse(Collidable &collidable, const ENTITY_TILE_CHECK checkFlag) {

    // ==== ZTRAN LOGIC ====
#if 0
    bool zTranThisFrame = false;
    int closestElev = -(ELYSIAN_TILE_ENTRY_ELEV_MAX+1);
    int smallestElevDiff = -(ELYSIAN_TILE_ENTRY_ELEV_MAX+1);
    unsigned zTranX, zTranY;

    collidable._onZTran = false;

    for(unsigned i = 0; i < 3; ++i) {
        unsigned tileX = ((int)collidable.getCenterWorldPos().x+16)/32;
        unsigned tileY = ((int)collidable.getCenterWorldPos().y+16)/32;

        TileEntry entry = _curArea->getTileEntry(i, tileX, tileY);
        if(entry.getZTran()) {
            if(entry.getElev() == collidable.getEntity()->getElev()) { // check elevation
                zTranThisFrame = true;
                zTranX = tileX;
                zTranY = tileY;
            }
            collidable._onZTran = true;
        }


        int elevDiff = entry.getElev() - collidable.getEntity()->getElev();
        if(elevDiff > 0) {
            if(smallestElevDiff <= 0 || elevDiff < smallestElevDiff) {
                smallestElevDiff = elevDiff;
                closestElev = entry.getElev();
            }
        } else if(elevDiff == 0 && smallestElevDiff < 0) {
            smallestElevDiff = elevDiff;
            closestElev = entry.getElev();

        } else if(elevDiff < 0 && elevDiff > smallestElevDiff) {
            smallestElevDiff = elevDiff;
            closestElev = entry.getElev();
        }

    /*
        if(elevDiff != 0) {
            //Force favoritism for higher elevations when two options are available
            if(elevDiff == smallestElevDiff) {
                if(entry.getElev() > closestElev) closestElev = entry.getElev();
            }
            else if(elevDiff < smallestElevDiff) {
                closestElev = entry.getElev();
                smallestElevDiff = elevDiff;
            }
        }
        */
    }

    unsigned prevZTranX, prevZTranY;
    unsigned curTileX, curTileY;
    switch(collidable.getZTranState()) {
    case Collidable::ZTRAN_INACTIVE:
        if(zTranThisFrame) {
            collidable.setZTranState(Collidable::ZTRAN_ACTIVE);
            collidable.setZTranTile(zTranX, zTranY);
        }
        break;
    case Collidable::ZTRAN_ACTIVE:
        collidable.getZTranTile(prevZTranX, prevZTranY);
            curTileX = ((int)collidable.getCenterWorldPos().x+16)/32;
            curTileY = ((int)collidable.getCenterWorldPos().y+16)/32;
        if(!(prevZTranX == curTileX && prevZTranY == curTileY)) {
            if(closestElev != ELYSIAN_TILE_ENTRY_ELEV_MAX) {
                //Debug::printf(Debug::DBG_VERBOSE, "Elevation - %d", closestElev);
                collidable.getEntity()->setElev(closestElev);
            }
            collidable.setZTranState(Collidable::ZTRAN_INACTIVE);
        }
        break;
    }


    // ==== /ZTRAN LOGIC ====
#endif

    int startX = (getCurrentArea()->width)*32.0f;
	int endX = 0;
    int startY = (getCurrentArea()->height)*32.0f;
	int endY = 0;
	auto checkBoundary = [&](GYVector4& vert) {
		if(vert.x < startX) startX = vert.x;
		if(vert.x > endX) endX = vert.x;
		if(vert.y < startY) startY = vert.y;
		if(vert.y > endY) endY = vert.y;
	};

	GYVector4 worldVertsOrig[4];
    //float sizeZ = 0.0f;
    char startElev;
    char endElev;
    memcpy(worldVertsOrig, collidable._collider.box.worldVert, sizeof(worldVertsOrig));

    switch(checkFlag) {
    case ENTITY_TILE_CHECK_VOLUME:
        startElev	=	(worldVertsOrig[3].z)/32;
        endElev		=	(worldVertsOrig[0].z)/32;
        //sizeZ = worldVertsOrig[0].z-worldVertsOrig[3].z;
        break;
    case ENTITY_TILE_CHECK_BOTTOM:
        endElev		=	(worldVertsOrig[0].z-16.0f-1.0f)/32+1;
        startElev	=	(endElev > 0)? endElev-1 : 0;
        break;
    case ENTITY_TILE_CHECK_TOP:
        startElev	=	(worldVertsOrig[3].z)/32.0f;
        endElev		=	startElev;
        break;
    }

    if(startElev < 0) startElev = 0;
    else if(startElev > ELYSIAN_TILE_ENTRY_ELEV_MAX) startElev = ELYSIAN_TILE_ENTRY_ELEV_MAX;

    if(endElev < 0) startElev = 0;
    else if(endElev > ELYSIAN_TILE_ENTRY_ELEV_MAX) endElev = ELYSIAN_TILE_ENTRY_ELEV_MAX;

    //const char diffElev = endElev - startElev;
    //const int sizeY = endY - startY;

    switch(collidable.getAttrib()->getColliderType()) {
    case GY_COLLIDER_AABB:


/*
        for(int y = (int)collidable._collider.box.worldVert[0].y/32; y <= (int)collidable._collider.box.worldVert[3].y/32+1; ++y)
            for(int x = (int)collidable._collider.box.worldVert[0].x/32; x <= (int)collidable._collider.box.worldVert[1].x/32+1; ++x) {
                gyMatPush();
                gyMatLoadIdentity();
                gyMatTranslateScale(x*32.0f, y*32.0f, 5.0f, 32.0f, 32.0f, 1.0f);
                _detectEntityTileCollisionFine(collidable, x, y, 0);
                gyMatPop();
            }
        break;*/
    case GY_COLLIDER_OBB: //not very efficient for long shit
        for(unsigned i = 0; i < 4; ++i) checkBoundary(collidable._collider.box.worldVert[i]);
#ifdef ELYSIAN_COLLISION_SYSTEM_PROTECTED
        if(startX < 0 || startY < 0 || endX > getCurrentArea()->width*32.0f || endY > getCurrentArea()->height*32.0f) return;
#endif
        for(unsigned char elev = startElev; elev <= endElev; ++elev) {

            const float zOffset = worldVertsOrig[3].z - ((int)startElev*32);

            for(unsigned i = 0; i < 4; ++i)
                collidable._collider.box.worldVert[i].y = worldVertsOrig[i].y;// - ((float)(elev-startElev)*32.0f);//-(int)worldVertsOrig[i].z%32;

#if 1
            for(int y = startY/32-(elev-startElev)-1; y <= endY/32-(elev-startElev)+1; ++y) {
                if(y < 0) continue;
                for(int x = startX/32; x <= endX/32+1; ++x) {

                    gyMatPush();
                    gyMatLoadIdentity();
                    //gyMatTranslateScale(x*32.0f, y*32.0f, 5.0f, 32.0f, 32.0f, 32.0f);
                    gyMatTranslateScale(x*32.0f, (y/*+(elev-startElev)*/)*32.0f-zOffset, 5.0f, 32.0f, 32.0f, 32.0f);
                    _detectEntityTileCollisionFine(collidable, x, y-elev, elev);
                    gyMatPop();
                }
            }
#else
            for(int y = startY/32-(elev-startElev); y <= endY/32-(elev-startElev)+1; ++y) {
                if(y < 0) continue;
                for(int x = startX/32; x <= endX/32+1; ++x) {

                    gyMatPush();
                    gyMatLoadIdentity();
                    //gyMatTranslateScale(x*32.0f, y*32.0f, 5.0f, 32.0f, 32.0f, 32.0f);
                    gyMatTranslateScale(x*32.0f,  y*32.0f/*-zOffset*/, 5.0f, 32.0f, 32.0f, 32.0f);
                    _detectEntityTileCollisionFine(collidable, x, y, elev);
                    gyMatPop();
                }
            }

#endif
        }
        break;

    default:
        assert(0);
    }
}

bool CollisionSystem::resolvePenetration(void) {


    //float deltaTime = getTimerSys()->getDeltaTime();
    unsigned i, j;


    if(_contactAmount == 0) {
        _conResIter = 0;
        return true;
    }

    for(i = 0; i < _contactAmount; ++i) _contactPool[i]._calculatePenetrationInternals();

    unsigned iter;
    for(iter = 0; iter < ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS; ++iter) {

        float   deepest         = -1.0f;
        float   deepestZ        = 0.0f;
        int     deepestIndex    = -1;
        int     deepestZIndex   = -1;

        for(j = 0; j < _contactAmount; ++j) {
            //reevaluate penetration depth
            if(!_contactPool[j]._resolvePen) continue;
            float fabsZDepth = fabs(_contactPool[j]._zDepth);
            if(fabsZDepth > deepestZ && fabsZDepth <= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX) {
                deepestZ = fabsZDepth;
                deepestZIndex = j;
            }
            if(_contactPool[j].depth > deepest && _contactPool[j]._resolvePen) {
                deepestIndex = j;
                deepest = _contactPool[j].depth;
            }
        }

        if(deepestZ > ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON) {
            _resolveZPenetration(&_contactPool[deepestZIndex]);
        } else if(deepest > ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON) {
            _resolveXYPenetration(&_contactPool[deepestIndex]);
        } else {
            break; //shit has all been resolved!
        }

#if 0
        //if(deepest <= ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON) break; //EPSILAAAAWN
        //if(deepestIndex == -1) break; //no more candidates for resolution
        //this can probably be lowered quite a bit once point of contact is fucking correct
        //Debug::printf(Debug::DBG_VERBOSE, "-------------POSITION ITERATION - %d-------", iter);

        //Resolve the Z direction first!
        if(fabs(_contactPool[deepestIndex].getZDepth()) <= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX /*&& fabs(_contactPool[deepestIndex].getZDepth()) >= 1.0f*/) {

            //if(fabs(_contactPool[deepestIndex].getZDepth()) > 1.0f) {
            /*   if(_contactPool[deepestIndex].getZDepth() > 0) {
                   if(_contactPool[deepestIndex]._bodyB) {
                       _contactPool[deepestIndex]._bodyB->getEntity()->transform._pos.z += _contactPool[deepestIndex].getZDepth();//;-0.5f;
                       _contactPool[deepestIndex]._bodyB->getEntity()->transform._pos.y -= _contactPool[deepestIndex].getZDepth();//-0.5f;
                   }
               } else {
                   _contactPool[deepestIndex]._bodyA->getEntity()->transform._pos.z += _contactPool[deepestIndex].getZDepth();//+0.5f;
                   _contactPool[deepestIndex]._bodyA->getEntity()->transform._pos.y -= _contactPool[deepestIndex].getZDepth();//+0.5f;
               }*/
        //   }
               _contactPool[deepestIndex].depth = 0.0f;
               _contactPool[deepestIndex].setResolveZ(true);
               //Update all other contacts, nigga!
               continue;
        } else _contactPool[deepestIndex].setResolveZ(false);


        //printf("ABOUT TO RESOLVE DEEPEST - %d, %f", deep

        _resolveXYPenetration(&_contactPool[deepestIndex]);
#endif
    }

    _conResIter = iter;

   // if(iter == ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS) Debug::printf(Debug::DBG_WARNING, "OUT OF PENETRATION ITERATIONS!", iter);

    return true;
}

void CollisionSystem::_resolveZPenetration(CollisionContact * const contact) {
    Collidable* col[2];
    //Collidable *newCol[2];
    float totalInertia = 0.0f;
    float deltaZ[2], linearInertia[2];
    float coeff[2];
    //float newCoeff[2];
    //unsigned bodyToCol[2];
   // GYCollider* collider[2];

    if(!contact->_resolvePen) return;

    if(!contact->_bodyB) {
        contact->_zDepth = 0.0f;
        return;
    }

    float zDepth = contact->getZDepth();

    if(contact->getZDepth() > 0.0f) {
        col[0] = contact->_bodyB;
        col[1] = contact->_bodyA;
       // bodyToCol[0] = 1;
       // bodyToCol[1] = 0;
        coeff[0] = -1.0f;
        coeff[1] = 1.0f;
    } else {
        col[0] = contact->_bodyA;
        col[1] = contact->_bodyB;
        //bodyToCol[0] = 0;
        //bodyToCol[1] = 1;
        coeff[0] = 1.0f;
        coeff[1] = -1.0f;
    }

    //zDepth = fabs(zDepth);

    //Do NOT push shit out of contact! Push it to the edge, and let it chill!
    if(zDepth > 0.0f) {
        zDepth -= ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON;
    } else {
        zDepth += ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON;
    }

    totalInertia += linearInertia[0] = col[0]->getAttrib()->getStaticAttrib()->inverseMass;
    totalInertia += linearInertia[1] = /*((col[1]->getStackedOnGround())? 0.0f :*/ col[1]->getAttrib()->getStaticAttrib()->inverseMass;
    //ADD ROTATIONAL INERTIA HERE!?!?
    totalInertia = 1.0f/totalInertia;

    for(unsigned i = 0; i < 2; ++i) {
        deltaZ[i] = zDepth * linearInertia[i] * totalInertia * coeff[i];
        col[i]->getEntity()->_transform._pos.z -= deltaZ[i];
       // col[i]->getEntity()->transform._pos.y += deltaZ[i];
    }

    GYVector3 vel[2];
    vel[0] = col[0]->getEntity()->getRigidBody()->getVelocity();
    vel[1] = col[1]->getEntity()->getRigidBody()->getVelocity();

    //bool resting = false;
    if(vel[0].z <= 0.0f /*&& vel[1].z <= 0.0f*/) {
        col[0]->setRestingOnBody(true);
        //GYVector3 newVel = vel[0];
        //newVel.z = 0;
        //col[0]->getEntity()->getRigidBody()->setVelocity(newVel);
        if(col[1]->getStackedOnGround()) col[0]->setStackedOnGround(true);
       // resting = true;
    }


    if(col[0]->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
        const float zImpulse = fabs(vel[1].z /* col[0]->getAttrib()->getStaticAttrib()->damping.z*/);
        auto* rb = col[0]->getEntity()->getRigidBody();
        if(zImpulse >= rb->getDurability()) {
            rb->setDurability(rb->getDurability() - zImpulse);
            rb->setBreakContact(contact);
        }
    }

    if(col[1]->getEntity()->getRigidBody()->getAttrib()->isBreakable()) {
        const float zImpulse = fabs(vel[0].z /*col[1]->getAttrib()->getStaticAttrib()->damping.z*/);
        auto* rb = col[1]->getEntity()->getRigidBody();
        if(zImpulse >= rb->getDurability()) {
            rb->setDurability(rb->getDurability() - zImpulse);
            rb->setBreakContact(contact);
        }
    }


 /*   GYVector3 vel = col[0]->getEntity()->getRigidBody()->getVelocity();
    vel.z = 0.0f;

    if(vel.z < 0.0f) {
    col[0]->getEntity()->getRigidBody()->setVelocity(vel);
    col[0]->getEntity()->getRigidBody()->accel.z = 0.0f;
}*/

    //Update all contacts!
    for(unsigned j = 0; j < _contactAmount; ++j) {
        CollisionContact* adjContact = &_contactPool[j];
        if(adjContact == contact) continue;
#if 0
        if(adjContact->_bodyA == contact->_bodyA) {


        } else if(adjContact->_bodyB == contact->_bodyA) {

        }


        if(adjContact->_bodyB == contact->_bodyA) {

        } else if(adjContact->_bodyB == contact->_bodyB) {

        }

#else
        if(adjContact->getZDepth() > 0.0f) {
          //  newCol[0] = adjContact->_bodyB;
           // newCol[1] = adjContact->_bodyA;
           // newCoeff[0] = -1.0f;
          //  newCoeff[1] = 1.0f;
        } else {
          //  newCol[0] = adjContact->_bodyA;
           // newCol[1] = adjContact->_bodyB;
          //  newCoeff[0] = 1.0f;
          //  newCoeff[1] = -1.0f;
        }

      /*  for(unsigned k = 0; k < 2; ++k) {
            for(unsigned l = 0; l < 2; ++l) {
                if(newCol[k] == col[l]) {
                    //if(k == l) contact->_zDepth += deltaZ[l];
                    //else contact->_zDepth -= deltaZ[l];
                    adjContact->_zDepth -= deltaZ[l] * coeff[k] * coeff[l];
                }
            }
        }*/
#if 0
        if(adjContact->_bodyA == contact->_bodyA) {
            adjContact->_zDepth -= deltaZ[bodyToCol[0]];
        } else if(adjContact->_bodyA == contact->_bodyB) {
            adjContact->_zDepth += deltaZ[bodyToCol[1]];
        }

        if(adjContact->_bodyB == contact->_bodyA) {
            adjContact->_zDepth += deltaZ[bodyToCol[0]];
        } else if(adjContact->_bodyB == contact->_bodyB) {
            adjContact->_zDepth -= deltaZ[bodyToCol[1]];
        }

        if(newCol[1] == col[0]) {
            /*if(!newCol[0]->getRestingOnBody())*/ newCol[0]->setRestingOnBody(col[0]->getRestingOnBody());
            /*if(!newCol[0]->getStackedOnGround())*/ newCol[0]->setStackedOnGround(col[0]->getStackedOnGround());
       }
#endif


#endif
#if 0
        if(newCol[0] == col[0]) {
            adjContact->_zDepth += fabs(deltaZ[0]) * newCoeff[0];
        } else if(newCol[0] == col[1]) {
            adjContact->_zDepth += fabs(deltaZ[1]) * newCoeff[0];

        }

        if(newCol[1] == col[0]) {
            adjContact->_zDepth += fabs(deltaZ[0]) * newCoeff[1];
            newCol[0]->setRestingOnBody(col[0]->getRestingOnBody());
            newCol[0]->setStackedOnGround(col[0]->getStackedOnGround());
        } else if(newCol[1] == col[1]) {
            adjContact->_zDepth += fabs(deltaZ[1]) * newCoeff[1];
        }

#endif
    }

   //contact->setResolveZ(true);
    contact->_zDepth -= zDepth;
    contact->depth = 0.0f;
}

void CollisionSystem::_resolveXYPenetration(CollisionContact * const con) {
    GYVector2 posDelta[2];
    //GYVector2 velDelta[2]
    GYVector2 rotDir[2]; //Apparently rotDir isn't really used? What in fuck's cunt?
    float orientDelta[2];
    //float rotDelta[2];
    GYVector2 cp;

    auto applyDamp = [](GYVector2 delta[2], CollisionContact* _contact) {
        const GYVector2* damping[2] = {
            (const GYVector2*)&_contact->_bodyA->getAttrib()->getStaticAttrib()->damping,
            (const GYVector2*)((_contact->_bodyB)? &_contact->_bodyB->getAttrib()->getStaticAttrib()->damping : nullptr)
        };
        unsigned fucked[2] = {0};
        unsigned i;

        for(i = 0; i < 2; ++i) {
            if(!damping[i]) break;
            unsigned n = !i;

            //Try to force the other body to resolve the X penetration
            if(damping[i]->x == 0.0f) {
               ++fucked[0];
                if(damping[n]) delta[n].x -= delta[i].x;
                delta[i].x = 0.0f;
            }

            //Try to force the other body to resolve the Y penetration
            if(damping[i]->y == 0.0f) {
               ++fucked[1];
                if(damping[n]) delta[n].y -= delta[i].y;
                delta[i].y = 0.0f;
            }
        }

        GYVector2 newNormal = _contact->normal;

        //Can't move in the X direction
        if(fucked[0] == i) {
            newNormal.x = 0.0f;
        }
        //Can't move in the Y direction
        if(fucked[1] == i) {
            newNormal.y = 0.0f;
        }

        _contact->depth = gyVec2DotProd(&_contact->normal, &newNormal) * _contact->depth;
        _contact->normal = newNormal;

    };

    gyResolvePenetration(con, posDelta, rotDir, orientDelta); //maybe must update its own shit

    applyDamp(posDelta, con);

    gyVec2AddAccum((GYVector2*)&con->_bodyA->getEntity()->_transform._pos, &posDelta[0]);
    //con->_bodyA->getEntity()->move(posDelta[0]);
    con->_bodyA->getEntity()->_transform._angle += orientDelta[0];

    if(con->staticAttrib[1]) {
        //applyDamp(dampedDelta, posDelta[1], con->_bodyB->getAttrib()->getStaticAttrib()->damping);
        gyVec2AddAccum((GYVector2*)&con->_bodyB->getEntity()->_transform._pos, &posDelta[1]);
        //con->_bodyB->getEntity()->move(posDelta[1]);
        con->_bodyB->getEntity()->_transform._angle += orientDelta[1];
    }

/**** /TAKEN FROM C PENETRATION RESOLUTION *****/

    //printf("pen - %f\n", _conPool[deepestIndex]._depth);
    //printf("posDelta[0] = <%f, %f> posDelta[1] = <%f, %f>\n", posDelta[0].x, posDelta[0].y, posDelta[1].x, posDelta[1].y);

    //update ALL contacts affected by this resolution
    for(unsigned j = 0; j < _contactAmount; ++j) {
        if(_contactPool[j].collider[0] == con->collider[0]) {
            gyVec2RotOrigin(&cp, &_contactPool[j].objPos[0], orientDelta[0]);
            gyVec2SubFrom(&cp, &_contactPool[j].objPos[0]);
            gyVec2AddAccum(&cp, &posDelta[0]);
            //printf("0 - 0, cp = <%f, %f>\n", cp.x, cp.y);
            _contactPool[j].depth -= gyVec2DotProd(&cp, &_contactPool[j].normal);
            //printf("depth - %f\n", _contactPool[j].depth);

        }
        else if(_contactPool[j].collider[0] == con->collider[1]) {
            gyVec2RotOrigin(&cp, &_contactPool[j].objPos[0], orientDelta[1]);
            gyVec2SubFrom(&cp, &_contactPool[j].objPos[0]);
            gyVec2AddAccum(&cp, &posDelta[1]);
            //printf("0 - 1, cp = <%f, %f>\n", cp.x, cp.y);
            _contactPool[j].depth -= gyVec2DotProd(&cp, &_contactPool[j].normal);
            //printf("depth - %f\n", _contactPool[j].depth);
        }

        //if(THERE IS EVEN A COLINDEX[1]) {
        if(_contactPool[j].collider[1] && _contactPool[j].collider[1] == con->collider[0]) {
            gyVec2RotOrigin(&cp, &_contactPool[j].objPos[1], orientDelta[0]);
            gyVec2SubFrom(&cp, &_contactPool[j].objPos[1]);
            gyVec2AddAccum(&cp, &posDelta[0]);
            //printf("1 - 0, cp = <%f, %f>\n", cp.x, cp.y);
            _contactPool[j].depth += gyVec2DotProd(&cp, &_contactPool[j].normal);
            //printf("depth - %f\n", _contactPool[j].depth);
        }
        else if(_contactPool[j].collider[1] && _contactPool[j].collider[1] == con->collider[1]) {
            gyVec2RotOrigin(&cp, &_contactPool[j].objPos[1], orientDelta[1]);
            gyVec2SubFrom(&cp, &_contactPool[j].objPos[1]);
            gyVec2AddAccum(&cp, &posDelta[1]);
            //printf("1 - 1, cp = <%f, %f>\n", cp.x, cp.y);
            _contactPool[j].depth += gyVec2DotProd(&cp, &_contactPool[j].normal);
            //printf("depth - %f\n", _contactPool[j].depth);

        }
    }

}

void CollisionSystem::onAreaExit(void) {
    getCollisionSys()->clearContacts();
    _sortSweepPool.clear();
}

void CollisionSystem::onAreaEnter(void) {
    unsigned size = _curArea->collidables.size();

    for(unsigned i = 0; i < size; ++i) {
        SortSweepCollidable* sweep = &_sortSweepPool.claimFreeEntry();
        sweep->collidable = _curArea->collidables[i];
        _curArea->collidables[i]->setSortSweepAABB(sweep);
    }
}



/*
void CollisionSystem::ResolveContacts(void) {
    ResolvePenetration();
    ResolveVelocities();
}
*/
bool CollisionSystem::checkCollision(const Entity &entity1, const Entity &entity2) const {
    if(!entity1.getCollidable() || ! entity2.getCollidable()) return false;
    return checkCollision(*entity1.getCollidable(), *entity2.getCollidable());
}

//Still a O(n) lookup to check for collision. Plenty of room for optimization
//EACH BODY HAS A COLLISION AMOUNT, SO IF IT ISNT COLLIDING, YOU DONT BOTHER WITH THIS
//LOOKUP!
//contactCount should be entity only? That would be faster?
bool CollisionSystem::checkCollision(const Collidable &col1, const Collidable &col2) const {
    if(col1._contactCount == 0 || col2._contactCount == 0) return false;
    for(unsigned i = 0; i < _contactAmount; ++i) {
        if((&col1 == _contactPool[i]._bodyA && &col2 == _contactPool[i]._bodyB)
            || (&col2 == _contactPool[i]._bodyA && &col1 == _contactPool[i]._bodyB)) {
                return true;
        }
    }
    return false;
}

    CollisionSystem::RENDER_LAYER CollisionSystem::cycleRenderLayer(void) {
        _renderLayer = (_renderLayer+1 == RENDER_MAX)? RENDER_LAYER_1 : (RENDER_LAYER)(_renderLayer+1);
        return _renderLayer;
    }

}
