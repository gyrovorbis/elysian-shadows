#include <fstream>
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"
#include "elysian_area.hpp"
#include "elysian_instance_transform.hpp"
#include "elysian_generic.hpp"

namespace elysian {

void Collidable::update(void) {
#ifdef ELYSIAN_ENGINE
	static const GYVector4 origin = { 0.0f, 0.0f, 0.0f, 1.0f };
	/* This is necessary to ensure that if the attribute geometry was dynamically changed,
		the runtime data is updated to reflect it. Keep shit synced. */
	_collider.col.type = _attrib->_type; 

	//Calculate world vertices for collider
	gyMatPush();
	gyMatLoadIdentity();
	_entity->transform.apply();
	gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, 0.0f, _attrib->_relSize.x, _attrib->_relSize.y, 1.0f);
    gyMatMultVec(&origin, &_centerWorldPos, 1);
	gyCalcWorldVert(&_collider.col);
	gyMatPop();
#endif
}
    
void Collidable::render(void) {
#ifdef ELYSIAN_ENGINE
    const static GYColor4 color = { 0, 0, 255, 128 };
	const static GYColor4 colColor =  { 255, 0, 0, 128 };
    
	gyVidTexBind(NULL);
	gyVidAlphaLevel(GY_ALPHA_FULL);
    
    gyMatPush();
        
    gyVidQuadListBegin((_contactCount == 0)? &color : &colColor);
    _entity->transform.apply();
    gyMatTranslateScale(_attrib->_relPos.x, _attrib->_relPos.y, 30.0f, _attrib->_relSize.x, _attrib->_relSize.y, 1.0f);
    gyVidRect(0);
    gyMatPop();
    gyVidQuadListEnd();
    
	/* ==== DEBUG CODE ==== */

	gyVidQuadListBegin(&colColor);

	gyMatPush(); //Render bottom of entity
	gyMatTranslateScale(_entity->transform._pos.x, _entity->transform._pos.y + 0.5f*_entity->transform._size.y, 30.0f, 2.0f, 2.0f, 1.0f);
	gyVidRect(0);
	gyMatPop();

	gyMatPush(); //Render center of entity
	gyMatTranslateScale(_entity->transform._pos.x, _entity->transform._pos.y, 30.0f, 2.0f, 2.0f, 1.0f);
	gyVidRect(0);
	gyMatPop();

	gyVidQuadListEnd();

#endif    
}
     
bool Collidable::load(std::ifstream& /*stream*/) {
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

bool Collidable::save(std::ofstream& /*stream*/) {
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
	_curArea->collidables.push_back(this);
        return true;
}

//O(n)
bool Collidable::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->collidables);
}

}
