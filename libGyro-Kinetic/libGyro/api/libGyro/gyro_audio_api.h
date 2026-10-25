/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#include "gyro_defines.h"
#include "gyro_platform.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GYAudioSource;
struct GYWav;
struct GYOgg;
struct GYVector3;

/*** TODO
 * GYWav and GYOgg use GYAudBuffer
 * internally for polymorphic compatibility
 */

typedef struct GYAudBuffer {
    unsigned int	bytes;
    void*			_data;
} GYAudBuffer;

typedef enum GY_SOURCE_ATTRIB {
    GY_SOURCE_POS,
    GY_SOURCE_VEL,
    GY_SOURCE_GAIN,
    GY_SOURCE_PITCH,
    GY_SOURCE_LOOPING,
    GY_SOURCE_ROLLOFF_FACTOR,
    GY_SOURCE_RELATIVE
} GY_SOURCE_ATTRIB;

int gyAudInit(void);
int gyAudUninit(void);

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos);
void gyAudListenerSetGain(const float gain);
void gyAudSetReferenceDistance(const int dist);
void gyAudSetZScale(const float scale);

//audio source
struct GYAudioSource* gyAudSourceCreate(void);
void gyAudSourceDestroy(struct GYAudioSource*const src);
void gyAudSourceSetfv(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const struct GYVector3*const vec);
void gyAudSourceSetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val);
float gyAudSourceGetf(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib);
void gyAudSourceSeti(struct GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val);
void gyAudSourcePlayWav(struct GYAudioSource*const src, const struct GYWav*const wav);
//void gyAudSourcePlayOgg(struct GYAudioSource*const src, const struct GYOgg*const ogg);
void gyAudSourceStop(struct GYAudioSource*const src);
int  gyAudSourceIsPlaying(struct GYAudioSource*const src);

GYAudBuffer* gyAudBufferCreate(void);
void gyAudBufferData(GYAudBuffer* buff, unsigned char* data, int format, size_t size, int freq);
void gyAudBufferDestroy(GYAudBuffer* buff);
//void gyAudSourceBindBuffer(struct GYAudioSource* src, const struct GYAudBuffer* buff);
//void gyAudSourceUnbindBuffer(struct GYAudSource* src);
void gyAudSourcePlayBuffer(struct GYAudioSource* src, const struct GYAudBuffer* buff);


#ifdef GYRO_AUDIO_PLATFORM_INCLUDE
#	include GYRO_AUDIO_PLATFORM_INCLUDE
#else
    typedef struct GYAudioSource {
        void* ptr;
    } GYAudioSource;
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
