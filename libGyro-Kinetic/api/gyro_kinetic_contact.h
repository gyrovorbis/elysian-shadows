#ifndef GYRO_KINETIC_CONTACT_H
#define GYRO_KINETIC_CONTACT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_math_api.h"

struct _GYCollider;
struct _GYStatic;

typedef struct _GYContact {
	GYVector2 objPos[2];
	GYVector2 worldPos;
	GYVector2 normal;
	const struct _GYCollider *collider[2];
	const struct _GYStatic *staticAttrib[2];
	//GYVector2 pointOfContact; Is that not the exact same thing as worldPos? WTF?
	float sepVel;
	float closingVel;
	float deltaVel;
	float depth;
	float coeffOfRest;
	//GYint32 _colIndex[2]; //support for just one coming later.
} GYContact;

//gyroContactCalculateInternals(GYuint32 index, const float deltaTime)


#ifdef __cplusplus
}
#endif

#endif
