#include "elysian_rigid_body.hpp"
#include <kinetic/gyro_kinetic.h>
#include "elysian_collidable.hpp"
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

//ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ForceTarget)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(RigidBodyAttrib)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(RigidBodyBase)

ForceTarget::ForceTarget(void):
	GYKinetic {
		{ 0.0f, 0.0f, 0.0f },		//vel
		{ 0.0f, 0.0f, 0.0f },		//accel
        { 0.0f, 0.0f, 0.0f },		//prevAccel
		{ 0.0f, 0.0f, 0.0f },		//forceAccum
		0.0f,				//rot
		0.0f,				//angAccel
        0.0f,				//torqueAccum
        0                   //resting
	}
{
}

void ForceTarget::applyForce(const GYVector3 &force) {
	gyVec3AddAccum(&forceAccum, &force);
}

void ForceTarget::applyForceToPoint(const GYVector2 &loc, const GYVector2 &force) {
	GYVector2 relPoint;

	gyVec2AddAccum((GYVector2*)&forceAccum, &force);

	gyVec2Sub(&relPoint, (GYVector2*)&getPosition(), &loc);
	torqueAccum += gyVec2ScalarCrossProd(&relPoint, &force);
}


void RigidBodyBase::integrate(const float deltaTime) {
	if(!_active) return;

    //float baseDamping;
    //float angDamping;
    GYStatic*const stat = getEntity()->getCollidable()->getAttrib()->getStaticAttrib();

    if(resting) {
        stat->angDamping    *= _tileDamping;
        gyVec2ScaleAccum((GYVector2*)&stat->damping, _tileDamping);
    } else {
      /*  angDamping          = stat->angDamping;
        baseDamping         = stat->damping;
        stat->angDamping    = stat->zDamping;
        stat->damping       = stat->zDamping;*/
    }

	gyIntegrate(this,
				stat,
                deltaTime,
                &_entity->_transform._pos,
                &_entity->_transform._orientZ);

    if(resting) {
        stat->angDamping    /= _tileDamping;
        gyVec2ScaleAccum((GYVector2*)&stat->damping, 1.0f/_tileDamping);
    } else {
       /* stat->damping       = baseDamping;
        stat->angDamping    = angDamping;*/

    }
}

auto RigidBodyBase::getTargetType(void) const -> TARGET_TYPE { return TARGET_RIGID_BODY; }
float RigidBodyBase::getOrientation(void) const { return getEntity()->_transform._orientZ; }
const GYVector3& RigidBodyBase::getPosition(void) const { return getEntity()->_transform._pos; }
const GYStatic* RigidBodyBase::getStaticAttrib(void) const { return getEntity()->getCollidable()->getAttrib()->getStaticAttrib(); }
bool RigidBodyBase::getRestingOnBody(void) const { return getEntity()->getCollidable()->getRestingOnBody(); }


}
