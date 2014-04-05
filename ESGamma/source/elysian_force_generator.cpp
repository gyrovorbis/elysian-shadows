#include "elysian_force_generator.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"

namespace elysian {

void GravityForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
	GYVector2 force;
	const GYStatic* attrib = target->getStaticAttrib();
	const float invMass = attrib->inverseMass;
	if(invMass == 0.0f) return;

	gyVec2Scale(&force, &_accel, 1.0f/invMass);

	target->applyForce(force);

}

void ImplosionForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
	GYVector2 dir;
	float distance;

	//if(_focalEntity == body->getEntity()) return;

	gyVec2Sub(&dir, (GYVector2*)&_focalEntity->transform._pos, (GYVector2*)&target->getPosition());

	distance = gyVec2Magnitude(&dir);

	//check range of influence
	if(distance > _radius) return;

	gyVec2Normalize(&dir);

	gyVec2ScaleAccum(&dir, _magnitude);

	target->applyForce(dir);
}

void ExplosionForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
	GYVector2 dir;
	float distance;
	GYVector3 pos3 = target->getPosition();

	//if(_focalEntity == body->getEntity()) return;

	gyVec2Sub(&dir, (GYVector2*)&pos3, (GYVector2*)&_focalEntity->transform._pos);

	distance = gyVec2Magnitude(&dir);

	//check range of influence
	if(distance > _radius) return;

	gyVec2Normalize(&dir);

	gyVec2ScaleAccum(&dir, _magnitude);

	target->applyForce(dir);

}


}
