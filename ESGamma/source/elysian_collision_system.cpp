#include "elysian_collision_system.hpp"
#include "elysian_collidable.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_map_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_party_system.hpp"

/*
	Welcome, motherfuckers, to hell. Where abstract math meets low-level optimization. 
	Where structural C algorithms meet encapsulated C++ datatypes.
	Where real men struggle to stand tall and bitches weep...
	--FG
*/

namespace elysian {

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
				auto monitor = _contactMonitors[curMask];

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
				if(curMask & colA._eventMonitorMask) _contactMonitors[curMask]->onCollisionExit(&colA, &colB);
				if(curMask & colB._eventMonitorMask) _contactMonitors[curMask]->onCollisionExit(&colB, &colA);
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
	unsigned size = _curArea->collidables.size();
	for(unsigned i = 0; i < size; ++i) {
		assert(_curArea->collidables[i]->getAttrib());
		_curArea->collidables[i]->update();
	}
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

	unsigned startX, startY, endX, endY;
	unsigned playerElev = getPartySys()->getPartyLeader()->getElev();
	Collidable &col = *getPartySys()->getPartyLeader()->getCollidable();

	gyVidTexBind(NULL);
	gyVidAlphaLevel(GY_ALPHA_FULL);
	getMapSys()->getOnscreenTileRegion(startX, startY, endX, endY);
	for(unsigned y = startY; y <= endY; ++y) {
		for(unsigned x = startX; x <= endX; ++x) {
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

			bool isOnZPlane = false;
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
				isOnZPlane = true;
				
				if(tileEntry.getZTran() && tileEntry.getElev() == playerElev) isZTran = true;
			}
			

			if(!isOnZPlane && !col._onZTran && col.getZTranState() != Collidable::ZTRAN_ACTIVE) colType = Tile::COLLIDER_FULL;
			if(colType == Tile::COLLIDER_NONE && !isZTran) continue;

			gyMatPush();
			gyMatTranslateScale(x*32.0f, y*32.0f, ZCoordConstants::tileDecor, 32.0f, 32.0f, 1.0f);
			
			if(isZTran) {
				gyVidQuadListBegin(&zTranColor);
				gyVidRect(0);
				gyVidQuadListEnd();
			}
			
			if(colType == Tile::COLLIDER_NONE) {
				gyMatPop();
				continue;
			}

			switch(colType) {
			case Tile::COLLIDER_L_NE:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_HALF_N);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_L_NW:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_HALF_N);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_L_SW:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_HALF_S);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_L_SE:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_HALF_S);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_QUADS_SE_NW:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_SE);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_NW);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_QUADS_SW_NE:
				gyMatPush();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_SW);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				gyMatPop();
				_applyTileColliderTransform(Tile::COLLIDER_QUAD_NE);
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
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
				gyVidColorArray(boxColors);
				gyVidTriangleStrip(boxLocalVerts, 0, 4);
				break;
			case Tile::COLLIDER_45DEG_NE:
			case Tile::COLLIDER_45DEG_NW:
			case Tile::COLLIDER_45DEG_SW:
			case Tile::COLLIDER_45DEG_SE:
				_applyTileColliderTransform(colType);
				gyVidColorArray(triColors);
				gyVidTriangleStrip(triLocalVerts, 0, 3);
				break;
			default: assert(0);
			}
			gyMatPop();
		}
	}
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

	//reset contact counters
	_contactAmount = 0;
	for(; i < _curArea->collidables.size(); ++i) _curArea->collidables[i]->_contactCount = 0;

	for(i = 0; i < _curArea->collidables.size(); ++i) {
		if(!_curArea->collidables[i]->_active) continue;
		detectEntityCollision(i);
		if(_curArea->collidables[i]->getAttrib()->getFlags()->checkAgainst & CollisionFlags::TILE)
			_detectEntityTileCollisionCoarse(*_curArea->collidables[i]);
	} 
}

//Looks correct algorithmically.
void CollisionSystem::detectEntityCollision(const unsigned i) {
	Collidable *bodyA = _curArea->collidables[i];
	Collidable *bodyB;
	int swapFlag;
    
    gyColliderCurrent(&bodyA->_collider.col, bodyA->getAttrib()->getStaticAttrib());
    
	for(unsigned j = i+1; j < _curArea->collidables.size(); ++j) {
		bodyB = _curArea->collidables[j];
			
		//Return if collision flags don't match OR
		//if both are on different z-planes.
        if(!(bodyA->getAttrib()->flags.checkAgainst & bodyB->getAttrib()->flags.type)
                /* || bodyA->getEntity()->getElev() != bodyB->getEntity()->getElev() */) continue;

        //solve for overlapping z/elevation region
        float aBot = bodyA->getCollider()->worldPos.z;
        float aTop = aBot + bodyA->getWorldSize().z;

        float bBot = bodyB->getCollider()->worldPos.z;
        float bTop = bBot + bodyB->getWorldSize().z;

        float zBot = -1.0f;
        float zTop = -1.0f;
        bool zTopFound = false;
        bool zBotFound = false;

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
        if(!zTopFound || !zBotFound) continue;

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


		if(gyCollision(&bodyB->_collider.col, bodyB->getAttrib()->getStaticAttrib(), &_contactPool[_contactAmount], &swapFlag)) {
			++bodyA->_contactCount;
			++bodyB->_contactCount;

           float zDiff;// =  (zTop == aTop)? (aTop-bBot) : -((bTop-aBot));

			if(!swapFlag) { 
				_contactPool[_contactAmount]._bodyA = bodyA;
				_contactPool[_contactAmount]._bodyB = bodyB;

                if(zTop == aTop && zBot == bBot) {
                    zDiff = aTop-bBot;
                } else if(zTop == bTop && zBot == aBot) {
                    zDiff = aBot-bTop;
                } else zDiff = 100.0f;

			}
			else {
				_contactPool[_contactAmount]._bodyB = bodyA;
				_contactPool[_contactAmount]._bodyA = bodyB;

                if(zTop == aTop && zBot == bBot) {
                    zDiff = aBot-bTop;
                } else if(zTop == bTop && zBot == aBot) {
                    zDiff = aTop-bBot;
                } else zDiff = 100.0f;
			}

			_contactPool[_contactAmount]._resolvePen		= bodyA->_resolvePen && bodyB->_resolvePen;
			_contactPool[_contactAmount]._eventMonitorMask	= (bodyA->_eventMonitorMask | bodyB->_eventMonitorMask);

        //    float zDiff = zTop-zBot;

            _contactPool[_contactAmount].setZDepth(zDiff);
			
			//Emit onCollide trigger for any entitjjies with this component
            //getBehavSys()->onCollide(bodyA->getEntity(), bodyB);
			//getBehavSys()->onCollide(bodyB->getEntity(), bodyA)

			
            if(++_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) {
                assert(0);
                break; //MAXXED OUT -- NOT GOOD ENOUGH
            }
		}

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
}

inline void CollisionSystem::_detectCollidableColliderCollision(Collidable &collidable, GYCollider &collider) {
	int swapFlag;

	gyColliderCurrent(&collidable._collider.col, collidable.getAttrib()->getStaticAttrib());
	if(gyCollision(&collider, nullptr, &_contactPool[_contactAmount], &swapFlag)) {
		_contactPool[_contactAmount]._bodyA = &collidable;
		_contactPool[_contactAmount]._bodyB = nullptr;
		_contactPool[_contactAmount]._eventMonitorMask = 0;
        _contactPool[_contactAmount].setZDepth(32.0f);
		_contactPool[_contactAmount++]._resolvePen = collidable._resolvePen;
		++collidable._contactCount;
        if(_contactAmount == ELYSIAN_COLLISION_CONTACT_LIST_SIZE) assert(0);
	}
}


void CollisionSystem::_detectEntityTileCollisionFine(Collidable &collidable, const unsigned x, const unsigned y, const unsigned char elev) {
	GYBoxCollider boxCollider;
	TileEntry tileEntry[3];
	Tile::COLLIDER_TYPE colType = Tile::COLLIDER_NONE;
	bool isTileAtElev = false;

	tileEntry[0] = _curArea->getL1TileEntry(x, y);
	tileEntry[1] = _curArea->getL2TileEntry(x, y);
	tileEntry[2] = _curArea->getL3TileEntry(x, y);

		//Iterate over tile layers and combine their colliders
		for(unsigned i = 0; i < 3; ++i) {
			//Only look at colliders on the entity's z-plane
		//	unsigned EntityElev = collidable.getEntity()->getElev();

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
			if(tileEntry[i].getElev() == elev) isTileAtElev = true;
		}

		//Do not allow collider to move to location without a tile at its elevation
		//UNLESS he's z-transitioning!
		//if(!isTileAtElev && !collidable._onZTran && collidable.getZTranState() != Collidable::ZTRAN_ACTIVE) colType = Tile::COLLIDER_FULL;

		//Will not always be the case!
		boxCollider.collider.type = GY_COLLIDER_AABB;

		//SKETCHY!!!
		boxCollider.collider.worldPos.x = x*32.0f;
		boxCollider.collider.worldPos.y = y*32.0f;

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
	int closestElev = -(ELYSIAN_MAP_ENTRY_ELEV_MAX+1);
	int smallestElevDiff = -(ELYSIAN_MAP_ENTRY_ELEV_MAX+1);
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
			if(closestElev != ELYSIAN_MAP_ENTRY_ELEV_MAX) {
				//Debug::printf(Debug::VERBOSE, "Elevation - %d", closestElev);
				collidable.getEntity()->setElev(closestElev);
			}
			collidable.setZTranState(Collidable::ZTRAN_INACTIVE);
		}
		break;
	}


	// ==== /ZTRAN LOGIC ====
#endif

	int startX = (getCurArea()->width)*32.0f;
	int endX = 0;
	int startY = (getCurArea()->height)*32.0f;
	int endY = 0;
	auto checkBoundary = [&](GYVector4& vert) {
		if(vert.x < startX) startX = vert.x;
		if(vert.x > endX) endX = vert.x;
		if(vert.y < startY) startY = vert.y;
		if(vert.y > endY) endY = vert.y;
	};

	GYVector4 worldVertsOrig[4];
	float sizeZ = 0.0f;
	char startElev;
	char endElev;
	memcpy(worldVertsOrig, collidable._collider.box.worldVert, sizeof(worldVertsOrig));

	switch(checkFlag) {
	case ENTITY_TILE_CHECK_VOLUME:
		startElev	=	(worldVertsOrig[3].z)/32;
		endElev		=	(worldVertsOrig[0].z)/32;
		sizeZ = worldVertsOrig[0].z-worldVertsOrig[3].z;
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
	else if(startElev > ELYSIAN_MAP_ENTRY_ELEV_MAX) startElev = ELYSIAN_MAP_ENTRY_ELEV_MAX;

	if(endElev < 0) startElev = 0;
	else if(endElev > ELYSIAN_MAP_ENTRY_ELEV_MAX) endElev = ELYSIAN_MAP_ENTRY_ELEV_MAX;

	const char diffElev = endElev - startElev;
	const int sizeY = endY - startY;

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
		if(startX < 0 || startY < 0 || endX > getCurArea()->width*32.0f || endY > getCurArea()->height*32.0f) return;
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
                    gyMatTranslateScale(x*32.0f, (y+(elev-startElev))*32.0f-zOffset, 5.0f, 32.0f, 32.0f, 32.0f);
					_detectEntityTileCollisionFine(collidable, x, y, elev);
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

	GYVector2 posDelta[2];
	//GYVector2 velDelta[2]
	GYVector2 rotDir[2]; //Apparently rotDir isn't really used? What in fuck's cunt? 
	float orientDelta[2];
	//float rotDelta[2];
	GYVector2 cp;
	//float deltaTime = getTimerSys()->getDeltaTime();
	unsigned i, j;


	if(_contactAmount == 0) {
		_conResIter = 0;
		return true;
	}

	for(i = 0; i < _contactAmount; ++i) _contactPool[i]._calculatePenetrationInternals();

	unsigned iter;
	for(iter = 0; iter < ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS; ++iter) {

		float deepest = -1.0f;
		int deepestIndex = -1;

		for(j = 0; j < _contactAmount; ++j) {
			//reevaluate penetration depth
			if(_contactPool[j].depth > deepest && _contactPool[j]._resolvePen) {
				deepestIndex = j;
				deepest = _contactPool[j].depth;
			}
		}
		
		if(deepest <= ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON) break; //EPSILAAAAWN
		if(deepestIndex == -1) break; //no more candidates for resolution
		//this can probably be lowered quite a bit once point of contact is fucking correct
		//Debug::printf(Debug::VERBOSE, "-------------POSITION ITERATION - %d-------", iter);

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
		gyResolvePenetration(&_contactPool[deepestIndex], posDelta, rotDir, orientDelta); //maybe must update its own shit

        CollisionContact *con = &_contactPool[deepestIndex];


/**** TAKEN FROM C PENETRATION RESOLUTION *****/
        auto applyDamp = [](GYVector2 delta[2], CollisionContact* contact) {
            const GYVector2* damping[2] = {
                (const GYVector2*)&contact->_bodyA->getAttrib()->getStaticAttrib()->damping,
                (const GYVector2*)((contact->_bodyB)? &contact->_bodyB->getAttrib()->getStaticAttrib()->damping : nullptr)
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

            GYVector2 newNormal = contact->normal;

            //Can't move in the X direction
            if(fucked[0] == i) {
                newNormal.x = 0.0f;
            }
            //Can't move in the Y direction
            if(fucked[1] == i) {
                newNormal.y = 0.0f;
            }

            contact->depth = gyVec2DotProd(&contact->normal, &newNormal) * contact->depth;
            contact->normal = newNormal;

        };

        applyDamp(posDelta, con);

        gyVec2AddAccum((GYVector2*)&con->_bodyA->getEntity()->transform._pos, &posDelta[0]);
        //con->_bodyA->getEntity()->move(posDelta[0]);
		con->_bodyA->getEntity()->transform._orientZ += orientDelta[0];

		if(con->staticAttrib[1]) {
            //applyDamp(dampedDelta, posDelta[1], con->_bodyB->getAttrib()->getStaticAttrib()->damping);
            gyVec2AddAccum((GYVector2*)&con->_bodyB->getEntity()->transform._pos, &posDelta[1]);
            //con->_bodyB->getEntity()->move(posDelta[1]);
			con->_bodyB->getEntity()->transform._orientZ += orientDelta[1];
		}
		
/**** /TAKEN FROM C PENETRATION RESOLUTION *****/

		//printf("pen - %f\n", _conPool[deepestIndex]._depth);
		//printf("posDelta[0] = <%f, %f> posDelta[1] = <%f, %f>\n", posDelta[0].x, posDelta[0].y, posDelta[1].x, posDelta[1].y);
		
		//update ALL contacts affected by this resolution
		for(j = 0; j < _contactAmount; ++j) {
			if(_contactPool[j].collider[0] == _contactPool[deepestIndex].collider[0]) {
				gyVec2RotOrigin(&cp, &_contactPool[j].objPos[0], orientDelta[0]);
				gyVec2SubFrom(&cp, &_contactPool[j].objPos[0]);
				gyVec2AddAccum(&cp, &posDelta[0]);
				//printf("0 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j].depth -= gyVec2DotProd(&cp, &_contactPool[j].normal);
				//printf("depth - %f\n", _contactPool[j].depth);
				
			}
			else if(_contactPool[j].collider[0] == _contactPool[deepestIndex].collider[1]) {
				gyVec2RotOrigin(&cp, &_contactPool[j].objPos[0], orientDelta[1]);
				gyVec2SubFrom(&cp, &_contactPool[j].objPos[0]);
				gyVec2AddAccum(&cp, &posDelta[1]);
				//printf("0 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j].depth -= gyVec2DotProd(&cp, &_contactPool[j].normal);
				//printf("depth - %f\n", _contactPool[j].depth);
			}
			
			//if(THERE IS EVEN A COLINDEX[1]) {
			if(_contactPool[j].collider[1] && _contactPool[j].collider[1] == _contactPool[deepestIndex].collider[0]) {
				gyVec2RotOrigin(&cp, &_contactPool[j].objPos[1], orientDelta[0]);
				gyVec2SubFrom(&cp, &_contactPool[j].objPos[1]);
				gyVec2AddAccum(&cp, &posDelta[0]);
				//printf("1 - 0, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j].depth += gyVec2DotProd(&cp, &_contactPool[j].normal);
				//printf("depth - %f\n", _contactPool[j].depth);
			}
			else if(_contactPool[j].collider[1] && _contactPool[j].collider[1] == _contactPool[deepestIndex].collider[1]) {
				gyVec2RotOrigin(&cp, &_contactPool[j].objPos[1], orientDelta[1]);
				gyVec2SubFrom(&cp, &_contactPool[j].objPos[1]);
				gyVec2AddAccum(&cp, &posDelta[1]);
				//printf("1 - 1, cp = <%f, %f>\n", cp.x, cp.y);
				_contactPool[j].depth += gyVec2DotProd(&cp, &_contactPool[j].normal);
				//printf("depth - %f\n", _contactPool[j].depth);
				
			}
		}
	}

	_conResIter = iter;

//	if(iter == ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS) Debug::printf(Debug::CRITICAL, "OUT OF PENETRATION ITERATIONS!", iter);

	return true; 
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
		_renderLayer = (_renderLayer+1 == RENDER_MAX)? RENDER_DISABLED : (RENDER_LAYER)(_renderLayer+1);
		return _renderLayer;
	}

}
