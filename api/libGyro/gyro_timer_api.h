/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_TIMER_H
#define GYRO_TIMER_H

#include "gyro_platform.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tm;
    
int                 gyTimerInit(void);
int                 gyTimerUninit(void);
int64_t             gyTimerTicks(void);
void                gyTimerSleep(const unsigned long duration);
char*               gyTimerStrpTime(const char *s, const char *format, struct tm *tm);

#ifdef GYRO_TIMER_PLATFORM_INCLUDE
#	include GYRO_TIMER_PLATFORM_INCLUDE
#endif
    
#ifdef __cplusplus
}
#endif

#endif // GYRO_TIMER_H
