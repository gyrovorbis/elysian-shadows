#include "elysian_force_generator.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"

namespace elysian {

void GravityForceGenerator::applyForce(RigidBody * const body, const float /*deltaTime*/) {
	GYVector2 force;
	const float invMass = body->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
	if(invMass == 0.0f) return;

	gyVec2Scale(&force, &_accel, 1.0f/invMass);

	body->applyForce(force);

}

void ImplosionForceGenerator::applyForce(RigidBody * const body, const float /*deltaTime*/) {
	GYVector2 dir;
	float distance;

	if(_focalEntity == body->getEntity()) return;

	gyVec2Sub(&dir, (GYVector2*)&_focalEntity->transform._pos, (GYVector2*)&body->getEntity()->transform._pos);

	distance = gyVec2Magnitude(&dir);

	//check range of influence
	if(distance > _radius) return;

	gyVec2Normalize(&dir);

	gyVec2ScaleAccum(&dir, _magnitude);

	body->applyForce(dir);
}

void ExplosionForceGenerator::applyForce(RigidBody * const body, const float /*deltaTime*/) {
	GYVector2 dir;
	float distance;

	if(_focalEntity == body->getEntity()) return;

	gyVec2Sub(&dir, (GYVector2*)&body->getEntity()->transform._pos, (GYVector2*)&_focalEntity->transform._pos);

	distance = gyVec2Magnitude(&dir);

	//check range of influence
	if(distance > _radius) return;

	gyVec2Normalize(&dir);

	gyVec2ScaleAccum(&dir, _magnitude);

	body->applyForce(dir);

}


}
