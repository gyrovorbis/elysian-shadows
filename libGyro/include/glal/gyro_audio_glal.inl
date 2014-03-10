#ifndef GYRO_AUDIO_GLAL_H
#define GYRO_AUDIO_GLAL_H

#include "gyro_glal.h"
#include "../gyro_system_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYAudioSource {
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

static inline int _checkALError(void) {
	int error = alGetError();

	if(error != AL_NO_ERROR) {
		_gyLog(GY_DEBUG_CRITICAL, "OpenAL Error raised: %s", _openALError(error));
		return 1;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
