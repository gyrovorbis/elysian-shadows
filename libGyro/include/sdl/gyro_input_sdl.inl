/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_INPUT_X86_H
#define GYRO_INPUT_X86_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYInputInitParams {
    void*       viewWidget;
    void*       glWidget;
    unsigned    controllerCount;
} GYInputInitParams;


#ifdef __cplusplus
}
#endif

#endif // GYRO_INPUT_X86_H
