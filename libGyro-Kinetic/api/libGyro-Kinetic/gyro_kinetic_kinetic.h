/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_KINETIC_KINETIC_H
#define GYRO_KINETIC_KINETIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libGyro/gyro_matrix_api.h>

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
