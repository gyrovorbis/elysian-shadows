#include "elysian_force_generator.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_collidable.hpp"
#include "elysian_entity.hpp"
#include <gyro_kinetic.h>


namespace elysian {

GravityForceGenerator::GravityForceGenerator(void) {
    gyGravityForceSet(&_accel);
}

void GravityForceGenerator::setAccel(const GYVector3 &accel) {
    _accel = accel;
    gyGravityForceSet(&_accel);
}


void GravityForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
    GYVector3 force;
    const GYStatic* attrib = target->getStaticAttrib();
    const float invMass = attrib->inverseMass;
    if(invMass == 0.0f || target->getRestingOnBody()) return;

    gyVec3Scale(&force, &_accel, 1.0f/invMass);

    target->applyForce(force);

}

void ImplosionForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
    GYVector3 dir;
    float distance;

    //if(_focalEntity == body->getEntity()) return;

    gyVec3Sub(&dir, (GYVector3*)&_focalEntity->_transform._pos, (GYVector3*)&target->getPosition());

    distance = gyVec3Magnitude(&dir);

    //check range of influence
    if(distance > _radius) return;

    gyVec3Normalize(&dir);

    gyVec3ScaleAccum(&dir, _magnitude);

    target->applyForce(dir);
}

void ExplosionForceGenerator::applyForce(ForceTarget * const target, const float /*deltaTime*/) {
    GYVector3 dir;
    float distance;
    GYVector3 pos3 = target->getPosition();

    //if(_focalEntity == body->getEntity()) return;

    gyVec3Sub(&dir, (GYVector3*)&pos3, (GYVector3*)&_focalEntity->_transform._pos);

    distance = gyVec3Magnitude(&dir);

    //check range of influence
    if(distance > _radius) return;

    gyVec3Normalize(&dir);

    gyVec3ScaleAccum(&dir, _magnitude);

    target->applyForce(dir);

}


}
