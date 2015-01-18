#include "elysian_collidable.hpp"
#include "elysian_instance_transform.hpp"
#include "elysian_generic.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Collidable)

CollisionSystem* Collidable::_colSys = nullptr;

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

void Collidable::update(void) {
	if(!_active) return;
    static const GYVector4 localVecs[2] = {
        { 0.0f, 0.0f, 0.0f, 1.0f }, //position
        { 1.0f, 1.0f, 1.0f, 1.0f }  //size
    };
    GYVector4 worldVecs[2];
	/* This is necessary to ensure that if the attribute geometry was dynamically changed,
		the runtime data is updated to reflect it. Keep shit synced. */
	_collider.col.type = _attrib->_type; 

	//Calculate world vertices for collider
	gyMatPush();
	gyMatLoadIdentity();
    _entity->_transform.apply();
    gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, _attrib->_relPos.z,
                        _attrib->_relSize.x, _attrib->_relSize.y, _attrib->_relSize.z);
    gyMatMultVec(localVecs, worldVecs, 2);
	gyCalcWorldVert(&_collider.col);
    gyMatPop();

    _collider.col.worldPos = *(GYVector3*)&worldVecs[0];
    gyVec3Sub(&_worldSize, (GYVector3*)&_collider.box.worldVert[2], (GYVector3*)&_collider.box.worldVert[0]);
    _worldSize.z *= -1.0f;

    _updateSortSweepAABB();

}
    
void Collidable::render(void) {
    const static GYColor4 color = { 0, 0, 255, 128 };
	const static GYColor4 colColor =  { 255, 0, 0, 128 };
    
	gyVidTexBind(NULL);
	gyVidAlphaLevel(GY_ALPHA_FULL);
    
    gyMatPush();
        
    gyVidQuadListBegin((_contactCount == 0)? &color : &colColor);
    _entity->_transform.apply();
	gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, 0.0f, _attrib->_relSize.x, _attrib->_relSize.y, 1.0f);
    gyVidRect(0);
    gyMatPop();
    gyVidQuadListEnd();
    
	/* ==== DEBUG CODE ==== */

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

}
     
bool Collidable::load(FILE* /*file*/) {
//	unsigned type;
//	bool empty = false;

	dbgprintf(LOG_VERBOSE, "Collidable::Load(ifstream&)");
    dbgPush();
	/*
	stream >> flags.type;
	stream >> flags.checkAgainst;

	stream >> type;
	dbgprintf(LOG_VERBOSE, "flags.type - %d, flags.checkAgainst - %d", flags.type, flags.checkAgainst);
	dbgprintf(LOG_VERBOSE, "collider.type - %d", type);
	
	//Instantiate collider and set type
	switch((GY_COLLIDER_TYPE)type) {
	case GY_COLLIDER_NONE:
		dbgprintf(LOG_CRITICAL, "No collider type! Defaulting to AABB!");
		type = GY_COLLIDER_AABB;
		empty = true;
	case GY_COLLIDER_AABB:
	case GY_COLLIDER_OBB:
		_collider.box = new GYBoxCollider;
		_collider.base->type = (GY_COLLIDER_TYPE)type;
	case GY_COLLIDER_RTRI:
		_collider.tri = new GYTriCollider;
		_collider.base->type = (GY_COLLIDER_TYPE)type;
		break;
	default:
		dbgprintf(LOG_CRITICAL, "Unsupported collider.type!");
	}

	//Load transform
	if(!empty) InstanceTransform::Load(_collider.base->transform, stream);
	*/
    dbgPop();
	return true;

}

bool Collidable::save(FILE* /*file*/) {
	dbgprintf(LOG_VERBOSE, "Collidable::Save(ofstream&)");
    dbgPush();
/*
	stream << getColliderType();
	if(getColliderType()) InstanceTransform::Save(_collider.base->transform, stream);
	*/

    dbgPop();
	return true;
}


//O(1)
bool Collidable::addToLevel(void) {
    _sortSweepAABB = _colSys->addSortSweepCollidable();
    _sortSweepAABB->collidable = this;
    _updateSortSweepAABB();
	_curArea->collidables.push_back(this);
        return true;
}

//O(n)
bool Collidable::removeFromLevel(void) {
    _colSys->removeSortSweepCollidable(_sortSweepAABB);
    return containerEraseOnMatch(this, _curArea->collidables);
}

void Collidable::_updateSortSweepAABB(void) {
    float startX = (_curArea->width)*32.0f;
    float endX = 0;
    float startY = (_curArea->height)*32.0f;
    float endY = 0;
    auto checkBoundary = [&](GYVector4& vert) {
        if(vert.x < startX) startX = vert.x;
        if(vert.x > endX) endX = vert.x;
        if(vert.y < startY) startY = vert.y;
        if(vert.y > endY) endY = vert.y;
    };

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

bool Collidable::checkFullCollision(Collidable* col) {
    return _colSys->detectCollidableCollidableCollision(this, col);
}

template<>
unsigned SortSweepCollidable::curIndex = 0;

}
