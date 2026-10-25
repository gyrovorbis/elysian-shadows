/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_test_api.h>

typedef struct GYTestQueue {
    GYTest*     stack[GYRO_TEST_QUEUE_DEPTH_MAX];
    GYTest*     last;
    GYTest*     first;
    GYTest*     current;
    uint32_t    completeCount;
    uint32_t    pendingCount;
} GYTestQueue;

static int placeHolder = 0;

void updatePlaceHolder(void) {
    ++placeHolder;
}


