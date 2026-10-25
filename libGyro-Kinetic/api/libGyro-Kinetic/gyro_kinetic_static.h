/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_KINETIC_STATIC_H
#define GYRO_KINETIC_STATIC_H

#include <libGyro/gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYStatic {
    GYVector3   damping;
    float       inverseMass;
    float       coeffOfRest;
    float       invMomOfInertia;
    float       angDamping;
    float       zCoeffOfRest;
} GYStatic;

#ifdef __cplusplus
}
#endif

#endif
