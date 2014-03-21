#include "elysian_rigid_body.hpp"
#include "elysian_entity.hpp"
#include <kinetic/gyro_kinetic.h>
#include "elysian_collidable.hpp"
#include "elysian_generic.hpp"

namespace elysian {


RigidBody::RigidBody(Entity& entity):
	Component(entity),
	GYKinetic {
		{ 0.0f, 0.0f },		//vel
		{ 0.0f, 0.0f },		//accel
		{ 0.0f, 0.0f },		//prevAccel
		{ 0.0f, 0.0f },		//forceAccum
		0.0f,				//rot
		0.0f,				//angAccel
		0.0f				//torqueAccum
	}
{

}

void RigidBody::integrate(const float deltaTime) {
	Collidable* collidable = getEntity()->getCollidable();

	gyIntegrate(this,
				collidable->getAttrib()->getStaticAttrib(),
				deltaTime,
				(GYVector2*)&_entity->transform._pos,
				&_entity->transform._orientZ);
}

void RigidBody::applyForce(const GYVector2 &force) {
//	gyApplyForce(this, &force);
}

void RigidBody::applyPointForce(const GYVector2 &loc, const GYVector2 &force) {
//	gyApplyPointForce(this, &force, &loc);
}

bool RigidBody::canAddTo(Entity& entity) {
	return (entity.getCollidable());
}

bool RigidBody::load(std::ifstream& stream) {
	return true;
}

bool RigidBody::save(std::ofstream& stream) {
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
