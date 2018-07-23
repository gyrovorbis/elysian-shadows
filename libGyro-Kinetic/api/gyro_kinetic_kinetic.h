#ifndef GYRO_KINETIC_KINETIC_H
#define GYRO_KINETIC_KINETIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_matrix_api.h"

typedef struct _GYKinetic {
	GYVector3 vel;
	GYVector3 accel;
	GYVector3 prevAccel;
	GYVector3 forceAccum;
	float rot;
	float angAccel;
	float torqueAccum;
    int resting;
} GYKinetic;

#ifdef __cplusplus
}
#endif

#endif
