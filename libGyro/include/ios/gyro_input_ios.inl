/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_INPUT_IOS_INL
#define GYRO_INPUT_IOS_INL
#ifdef __cplusplus
extern "C" {
#endif

#include <gyro_matrix_api.h>

typedef struct GYInputInitParams {
    char compat;
} GYInputInitParams;


typedef struct GYInput {
    int up;
    int down;
    int left;
    int right;
    int shake;
    float pinch;
    float cursorMag;
    GYVector3 accelerometer;
    GYVector2 cursorPos;
    GYVector2 cursorDir;
} GYInput;

#ifdef __cplusplus
}
#endif

#endif // GYRO_INPUT_IOS_INL

