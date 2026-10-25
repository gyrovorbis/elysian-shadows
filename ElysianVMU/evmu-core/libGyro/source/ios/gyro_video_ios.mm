/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>


int gyVidInit(const GYVidInitParams * const params) {
    glalVidOglInit(1080, 720, 1080, 720);
    return 1;
}
