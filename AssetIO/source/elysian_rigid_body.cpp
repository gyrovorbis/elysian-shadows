#include "elysian_rigid_body.hpp"
#include "elysian_entity.hpp"
#include <kinetic/gyro_kinetic.h>
#include "elysian_collidable.hpp"
#include "elysian_generic.hpp"

namespace elysian {

ForceTarget::ForceTarget(void):
	GYKinetic {
		{ 0.0f, 0.0f, 0.0f },		//vel
		{ 0.0f, 0.0f, 0.0f },		//accel
		{ 0.0f, 0.0f, 0.0f },		//prevAccel
		{ 0.0f, 0.0f, 0.0f },		//forceAccum
		0.0f,				//rot
		0.0f,				//angAccel
		0.0f				//torqueAccum
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


RigidBody::RigidBody(Entity& entity):
	Component(entity)

{}

void RigidBody::integrate(const float deltaTime) {
	if(!_active) return;

    float baseDamping;
    float angDamping;
    GYStatic*const stat = getEntity()->getCollidable()->getAttrib()->getStaticAttrib();

    if(_onGround) {
        stat->angDamping    *= _tileDamping;
        stat->damping       *= _tileDamping;
    } else {
      /*  angDamping          = stat->angDamping;
        baseDamping         = stat->damping;
        stat->angDamping    = stat->zDamping;
        stat->damping       = stat->zDamping;*/
    }

	gyIntegrate(this,
				stat,
                deltaTime,
				&_entity->transform._pos,
				&_entity->transform._orientZ);

    if(_onGround) {
        stat->angDamping    /= _tileDamping;
        stat->damping       /= _tileDamping;
    } else {
       /* stat->damping       = baseDamping;
        stat->angDamping    = angDamping;*/

    }

}

auto RigidBody::getTargetType(void) const -> TARGET_TYPE { return TARGET_RIGID_BODY; }
float RigidBody::getOrientation(void) const { return getEntity()->transform._orientZ; }
const GYVector3& RigidBody::getPosition(void) const { return getEntity()->transform._pos; }
const GYStatic* RigidBody::getStaticAttrib(void) const { return getEntity()->getCollidable()->getAttrib()->getStaticAttrib(); }

bool RigidBody::canAddTo(Entity& entity) {
	return (entity.getCollidable());
}

bool RigidBody::load(FILE* /*file*/) {
	return true;
}

bool RigidBody::save(FILE* /*file*/) {
	return true;
}

bool RigidBody::addToLevel(void) {
	_curArea->rigidBodies.push_back(this);
	return true;
}

bool RigidBody::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->rigidBodies);
	return true;
}

auto RigidBody::getType(void) const -> TYPE { return Component::RIGID_BODY; }


}
