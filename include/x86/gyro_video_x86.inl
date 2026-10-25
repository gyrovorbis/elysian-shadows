/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VIDEO_X86_H
#define GYRO_VIDEO_X86_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GY_VID_RESOLUTION { GY_VID_320x240, GY_VID_640x480, GY_VID_1280x720 } GY_VID_RESOLUTION;

typedef enum GY_WIN_PROPERTIES {
    GY_WIN_RESIZABLE = 1,
    GY_WIN_FULLSCREEN = 2,
    GY_WIN_STRETCH = 4
} GY_WIN_PROPERTIES;

typedef struct GYVidInitParams {
    GY_VID_RESOLUTION res;
    int prop;
    int minWidth, minHeight;
    int maxWidth, maxHeight;
    const char *title;
    const char *icon;
} GYVidInitParams;

#ifdef __cplusplus
}
#endif

#endif
