#ifndef GYRO_KINETIC_KINETIC_H
#define GYRO_KINETIC_KINETIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../gyro_matrix_api.h"

typedef struct _GYKinetic {
	GYVector2 vel;
	GYVector2 accel;
	GYVector2 prevAccel;
	GYVector2 forceAccum;
	float rot;
	float angAccel;
	float torqueAccum;
} GYKinetic;

#ifdef __cplusplus
}
#endif

#endif
