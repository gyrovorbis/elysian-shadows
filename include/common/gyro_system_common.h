/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_SYSTEM_SHARED_H
#define GYRO_SYSTEM_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYDebugCallbacks {
    void(*printf)(const GY_DEBUG_SEVERITY level, const char *const);
    void(*push)(void);
    void(*pop)(const unsigned);
} GYDebugCallbacks;

void gySysDebugEnable(GY_DEBUG_SEVERITY level, const GYDebugCallbacks* callbacks);
void _gyLog(GY_DEBUG_SEVERITY level, const char* const str, ...);
void _gyPush(void);
void _gyPop(const unsigned count);

#ifdef __cplusplus
}
#endif

#endif // GYRO_SYSTEM_SHARED_H
