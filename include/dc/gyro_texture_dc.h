/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <stdint.h>


typedef struct GYTexCoord2 {
//#if GYRO_PLATFORM != GYRO_PLATFORM_DC
    float u, v;
//#else
    //uint32_t uv; //packed together on Dreamcast
//#endif
} GYTexCoord2;

typedef struct GYTexCoordFrame {
	GYTexCoord2 topLeft;
	GYTexCoord2 botRight;
//#if GYRO_PLATFORM == GYRO_PLATFORM_DC
//	GYTexCoord2 topRight;
//	GYTexCoord2 botLeft;
//#endif
	uint16_t spriteWidth, spriteHeight;
} GYTexCoordFrame;

