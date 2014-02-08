#ifndef GYRO_OGG_H
#define GYRO_OGG_H

#include "gyro_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _GYAudioSource;

typedef struct _GYOgg {
	char			path[GYRO_PATH_MAX_SIZE];
	unsigned int	bytes;
	unsigned		looping;
	void*			_data;
} GYOgg;

GYOgg* gyAudOggCreate(const char*const path);
void gyAudOggDestroy(GYOgg*const ogg);

struct _GYAudioSource* gyAudOggStreamCreate(void);
void gyAudOggStreamDestroy(struct _GYAudioSource*const src);
void gyAudOggStreamPlay(struct _GYAudioSource*src, GYOgg*ogg);
void gyAudOggStreamPause(struct _GYAudioSource*src);
void gyAudOggStreamStop(struct _GYAudioSource*src);
void gyAudOggStreamSetLooping(struct _GYAudioSource*const src, const unsigned looping);
double gyAudOggStreamTime(struct _GYAudioSource*const src);
void gyAudOggStreamSync(struct _GYAudioSource*const src1, struct _GYAudioSource*const src2);

#ifdef GYRO_OGG_PLATFORM_INCLUDE
#	include GYRO_OGG_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_OGG_H
