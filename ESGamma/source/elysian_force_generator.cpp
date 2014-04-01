#include "elysian_force_generator.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"

namespace elysian {

void GravityForceGenerator::applyForce(RigidBody * const body, const float deltaTime) {
	GYVector2 force;
	const float invMass = body->getEntity()->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
	if(invMass == 0.0f) return;

	gyVec2Scale(&force, &_accel, 1.0f/invMass);

	body->applyForce(force);

}


}
