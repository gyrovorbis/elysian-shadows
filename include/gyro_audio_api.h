#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#include "gyro_defines.h"
#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _GYAudioSource;
struct _GYWav;
struct _GYOgg;
struct _GYVector3;

int gyAudInit(void);
int gyAudUninit(void);

typedef enum _GY_SOURCE_ATTRIB {
	GY_SOURCE_POS,
	GY_SOURCE_VEL,
	GY_SOURCE_GAIN,
	GY_SOURCE_PITCH,
	GY_SOURCE_LOOPING,
	GY_SOURCE_ROLLOFF_FACTOR,
	GY_SOURCE_RELATIVE
} GY_SOURCE_ATTRIB;

//audio listener
void gyAudListenerSetPos(const struct _GYVector3* const pos);
void gyAudSetReferenceDistance(const int dist);
void gyAudSetZScale(const float scale);

//audio source
struct _GYAudioSource* gyAudSourceCreate(void);
void gyAudSourceDestroy(struct _GYAudioSource*const src);
void gyAudSourceSetfv(struct _GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const struct _GYVector3*const vec);
void gyAudSourceSetf(struct _GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const float val);
float gyAudSourceGetf(struct _GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib);
void gyAudSourceSeti(struct _GYAudioSource*const src, const GY_SOURCE_ATTRIB attrib, const int val);
void gyAudSourcePlayWav(struct _GYAudioSource*const src, const struct _GYWav*const wav);
void gyAudSourcePlayOgg(struct _GYAudioSource*const src, const struct _GYOgg*const ogg);
void gyAudSourceStop(struct _GYAudioSource*const src);
int  gyAudSourceIsPlaying(struct _GYAudioSource*const src);

#ifdef GYRO_AUDIO_PLATFORM_INCLUDE
#	include GYRO_AUDIO_PLATFORM_INCLUDE
#else
	typedef struct _GYAudioSource {
		void* ptr;
	} GYAudioSource;
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
