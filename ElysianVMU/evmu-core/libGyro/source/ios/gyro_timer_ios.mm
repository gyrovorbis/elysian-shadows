/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_timer_api.h>
#include <sys/time.h>



int gyTimerInit(void) {
    return 1;
}

int gyTimerUninit(void) {
    return 1;
}

int64_t gyTimerTicks(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    uint64_t nanos = (time.tv_sec * 1000000000) + (time.tv_usec * 1000);
    return nanos;
}


void gyTimerSleep(const unsigned long duration) {
}
