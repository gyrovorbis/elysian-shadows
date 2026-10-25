/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_CHROMATIC_ABERRATION_GL_H
#define GYRO_CHROMATIC_ABERRATION_GL_H

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_GLAL_CHROMATIC_ABERRATION_VERTEX_SHADER     "/assets/shaders/chromaticAberration.vsh"
#define GYRO_GLAL_CHROMATIC_ABERRATION_FRAGMENT_SHADER   "/assets/shaders/chromaticAberration.fsh"

int glalVidInitializeChromaticAberration(void);
int glalVidUninitializeChromaticAberration(void);
void glalVidApplyChromaticAberration(void);

#ifdef __cplusplus
}
#endif



#endif // GYRO_CHROMATIC_ABERRATION_GLAL_H
