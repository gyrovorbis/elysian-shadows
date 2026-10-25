/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_timer_api.h>
#include <kos.h>

uint32_t _startTick = 0;

int64_t gyTimerTicks(void) {
    return (timer_ms_gettime64()-_startTick);
}

int gyTimerInit(void) {
    _startTick = timer_ms_gettime64();
    return 1;
}

int gyTimerUninit(void) {
    return 1;
}
