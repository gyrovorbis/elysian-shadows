#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_animation_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>

#include <utility/elysian_generic.hpp>
#include "elysian_collision_system.hpp"
#include <gyro_shader_api.h>


namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CollisionFlags)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CollidableStaticAttrib)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CollidableAttrib)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CollidableBase)

CollisionSystem* CollidableBase::_colSys = nullptr;

CollidableBase::CollidableBase(const CollidableBase &other) {
    *this = other;
    _sortSweepAABB = nullptr;
}

void CollidableAttrib::setStaticAttrib(const OOLUA::Lua_table_ref valTable) { _staticAttrib.fromTable(valTable); }

void CollidableAttrib::setFlags(const OOLUA::Lua_table_ref valTable) { flags.fromTable(valTable); }

void CollisionContact::_calculatePenetrationInternals(void) {
	gyContactCalculatePenetrationInternals(this,
										   (GYVector2*)&_bodyA->getCenterWorldPos(),
										   (_bodyB)? (GYVector2*)&_bodyB->getCenterWorldPos() : nullptr);
}

void CollisionContact::_calculateVelocityInternals(const float deltaTime) {
	gyContactCalculateVelocityInternals(this,
										(GYKinetic*)_bodyA->getEntity()->getRigidBody(),
										(_bodyB)? (GYKinetic*)_bodyB->getEntity()->getRigidBody() : nullptr,
										deltaTime);
}

void CollidableBase::update(void) {
	if(!_active) return;
    static const GYVector4 localVecs[2] = {
        { 0.0f, 0.0f, 0.0f, 1.0f }, //position
        { 1.0f, 1.0f, 1.0f, 1.0f }  //size
    };
    GYVector4 worldVecs[2];
    float oldY;
	/* This is necessary to ensure that if the attribute geometry was dynamically changed,
		the runtime data is updated to reflect it. Keep shit synced. */
	_collider.col.type = _attrib->_type; 

	//Calculate world vertices for collider
	gyMatPush();
    if(_worldSpace) {
        gyMatLoadIdentity();
    } else {
        //_entity->pushViewTransform();
        oldY = _entity->_transform._pos.y;
        _entity->_transform._pos.y -= _entity->_transform._pos.z;
    }
      _entity->_transform.apply();

    gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, _attrib->_relPos.z,
                        _attrib->_relSize.x, _attrib->_relSize.y, _attrib->_relSize.z);
    gyMatMultVec(localVecs, worldVecs, 2);
	gyCalcWorldVert(&_collider.col);

    if(!_worldSpace) _entity->_transform._pos.y = oldY;
     gyMatPop();

    _collider.col.worldPos = *(GYVector3*)&worldVecs[0];
    gyVec3Sub(&_worldSize, (GYVector3*)&_collider.box.worldVert[2], (GYVector3*)&_collider.box.worldVert[0]);
    _worldSize.z *= -1.0f;

    _updateSortSweepAABB();

}
    
void CollidableBase::render(void) {
    if(!_worldSpace) return;
    const static GYColor4 color = { 0, 0, 255, 128 };
	const static GYColor4 colColor =  { 255, 0, 0, 128 };
    
    gyVidTexBind(nullptr);
    gyVidNormalMapBind(nullptr);
    gyVidSpecularMapBind(nullptr);
	gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);

    gyMatPush();
        
    gyVidQuadListBegin((_contactCount == 0)? &color : &colColor);
    _entity->_transform.apply();
	gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, 0.0f, _attrib->_relSize.x, _attrib->_relSize.y, 1.0f);
    gyVidRect(0);
    gyMatPop();
    gyVidQuadListEnd();
    
	/* ==== DEBUG CODE ==== */
#if 0
	gyVidQuadListBegin(&colColor);

	gyMatPush(); //Render bottom of entity
    gyMatTranslateScale(_entity->_transform._pos.x, _entity->_transform._pos.y + 0.5f*_entity->_transform._size.y, /*ZCoordConstants::tileDecor*/ ZCoordConstants::fg1+2.0f, 2.0f, 2.0f, 1.0f);
	gyVidRect(0);
	gyMatPop();

	gyMatPush(); //Render center of entity
    gyMatTranslateScale(_entity->_transform._pos.x, _entity->_transform._pos.y, /*ZCoordConstants::tileDecor*/ ZCoordConstants::fg1+2.0f, 2.0f, 2.0f, 1.0f);
	gyVidRect(0);
	gyMatPop();

	gyVidQuadListEnd();
#endif

}

void CollidableBase::_updateSortSweepAABB(void) {
    float startX = (getEntity()->getArea()->width)*32.0f;
    float endX = 0.0f;
    float startY = (getEntity()->getArea()->height)*32.0f;
    float endY = 0.0f;
    auto checkBoundary = [&](GYVector4& vert) {
        if(vert.x < startX) startX = vert.x;
        if(vert.x > endX) endX = vert.x;
        if(vert.y < startY) startY = vert.y;
        if(vert.y > endY) endY = vert.y;
    };

	if(!_sortSweepAABB) return;

    for(unsigned i = 0; i < 4; ++i) checkBoundary(_collider.box.worldVert[i]);

    _sortSweepAABB->min[0] = startX;
    _sortSweepAABB->max[0] = endX;

#if ELYSIAN_SORT_SWEEP_DIMENSIONS >= 2
    _sortSweepAABB->min[1] = startY;
    _sortSweepAABB->max[1] = endY;
#   if ELYSIAN_SORT_SWEEP_DIMENSIONS >= 3
    _sortSweepAABB->min[2] = _collider.box.worldVert[3].z;
    _sortSweepAABB->max[2] = _collider.box.worldVert[0].z;
#   endif
#endif
}


/*
for(i = 0; i < _curArea->collidables.size(); ++i) {
    if(!_curArea->collidables[i]->_active || !_curArea->collidables[i]->_worldSpace) continue;
    _detectEntityTileZCollisionCoarse(*_curArea->collidables[i]);
    detectEntityCollision(i);
    if(_curArea->collidables[i]->getAttrib()->getFlags()->checkAgainst & CollisionFlags::TILE) {
        _detectEntityTileCollisionCoarse(*_curArea->collidables[i]);
    }
}*/

bool CollidableBase::checkFullCollision(Collidable* col) {
    return _colSys->_detectEntityEntityCollision(getEntity(), col->getEntity());
}



bool CollidableBase::checkAABBCollision(Collidable* col) {
    SortSweepCollidable* b = col->_sortSweepAABB;
    auto* a = _sortSweepAABB;



    return (((a->min[0] > b->min[0] && a->min[0] < b->max[0]) || (b->min[0] > a->min[0] && b->min[0] < a->max[0])) &&
            ((a->min[1] > b->min[1] && a->min[1] < b->max[1]) || (b->min[1] > a->min[1] && b->min[1] < a->max[1])));// &&
           // ((a->min[2] > b->min[2] && a->min[2] < b->max[2]) || (b->min[2] > a->min[2] && b->min[2] < a->max[2])));
}
\
void CollidableBase::setSortSweepAABB(SortSweepCollidable* sweep) {
    _sortSweepAABB = sweep;
}

SortSweepCollidable* CollidableBase::getSortSweepAABB(void) const { return _sortSweepAABB; }
/*
template<>
unsigned SortSweepCollidable::curIndex = 0;
*/
}
