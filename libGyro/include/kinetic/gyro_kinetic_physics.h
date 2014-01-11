#ifndef GYRO_KINETIC_PHYSICS_H
#define GYRO_KINETIC_PHYSICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../gyro_matrix_api.h"
#include "gyro_kinetic_static.h"

struct _GYKinetic;
struct _GYContact;

void gyResolvePenetration(struct _GYContact *con, GYVector2 posDelta[], GYVector2 rotDir[], float orientDelta[], float deltaTime);
    
//Physics pipeline (excluding collision checking)
void gyIntegrate(void *collider, struct _GYKinetic *kin, const struct _GYStatic *stat);

/* CONTACT RESOLUTION
Algorithms for resolving ONE contact. Since this contact could have potentially affected a million other contacts,
these algorithms are going to have to be called iteratively to resolve them all.

STATIC ATTRIBUTE DATA MAY BE STORED WITHIN CONTACT AND MAY NOT HAVE TO BE PASSED?
*/

//NULL static attributes mean something? 

void gyResolveVelocity(struct _GYContact *con, struct _GYKinetic *kinA, struct _GYKinetic *kinB, const struct _GYStatic *statA, const struct _GYStatic *statB);

//Auxiliary functions
void gyApplyForce(struct _GYKinetic *kin, const GYVector2 *force);
void gyApplyPointForce(struct _GYKinetic *kin, const GYVector2 *force, const GYVector2 *point);
//apply global damp (that doesn't fuck with individual damp values)? Very good idea

#ifdef __cplusplus
}
#endif


#endif
