/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_OGG_H
#define GYRO_OGG_H

#include "gyro_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GYAudioSource;

typedef struct GYOgg {
    char			path[GYRO_PATH_MAX_SIZE];
    unsigned int	bytes;
    unsigned		looping;
    void*			_data;
} GYOgg;

GYOgg* gyAudOggCreate(const char*const path);
void gyAudOggDestroy(GYOgg*const ogg);

struct GYAudioSource* gyAudOggStreamCreate(const char* threadName);
void gyAudOggStreamDestroy(struct GYAudioSource*const src);
void gyAudOggStreamPlay(struct GYAudioSource*src, GYOgg*ogg);
void gyAudOggStreamPause(struct GYAudioSource*src);
void gyAudOggStreamStop(struct GYAudioSource*src);
void gyAudOggStreamSetLooping(struct GYAudioSource*const src, const unsigned looping);
double gyAudOggStreamTime(struct GYAudioSource*const src);
void gyAudOggStreamSync(struct GYAudioSource*const src1, struct GYAudioSource*const src2);

#ifdef GYRO_OGG_PLATFORM_INCLUDE
#	include GYRO_OGG_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_OGG_H
