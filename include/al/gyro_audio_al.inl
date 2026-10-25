/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_AUDIO_AL_H
#define GYRO_AUDIO_AL_H

#include <al/gyro_al.h>
#include <libGyro/gyro_system_api.h>

#define GY_AL_CLEAR_ERROR() \
    (_clearALError(__FILE__, __LINE__, __func__))

#define GY_AL_CHECK_ERROR() \
    (_checkALError(__FILE__, __LINE__, __func__))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYAudioSource {
    void* ptr;
} GYAudioSource;

void glalAudSourceAlloc(GYAudioSource* source);

static inline const char* _openALError(const int err) {
    switch (err) {
    case AL_NO_ERROR:
        return "AL_NO_ERROR";
    case AL_INVALID_NAME:
        return "AL_INVALID_NAME";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY";
    default:
        return "No such error code";
    }
}

static inline int _checkALError(const char* file, int line, const char* func) {
    int error = alGetError();

    if(error != AL_NO_ERROR) {
        _gyLog(GY_DEBUG_ERROR, "OpenAL Error: %d ('%s') [%s:%d (func %s)]", error, _openALError(error), file, line, func);
        return 1;
    }
    return 0;
}

static inline int _clearALError(const char* file, int line, const char* func) {
    int error = alGetError();

    if(error != AL_NO_ERROR) {
        _gyLog(GY_DEBUG_ERROR, "Uncaught OpenAL Error: %d ('%s'), Happened before: [%s:%d (func %s)]", error, _openALError(error), file, line, func);
        return 0;
    }
    return 1;

}

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
