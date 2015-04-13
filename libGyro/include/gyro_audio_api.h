#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#include "gyro_defines.h"
#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GYAudioSource;
struct GYWav;
struct GYOgg;
struct GYVector3;

int gyAudInit(void);
int gyAudUninit(void);

typedef enum GY_SOURCE_ATTRIB {
    GY_SOURCE_POS,
    GY_SOURCE_VEL,
    GY_SOURCE_GAIN,
    GY_SOURCE_PITCH,
    GY_SOURCE_LOOPING,
    GY_SOURCE_ROLLOFF_FACTOR,
    GY_SOURCE_RELATIVE
} GY_SOURCE_ATTRIB;

//audio listener
void gyAudListenerSetPos(const struct GYVector3* const pos);
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
void gyAudSourcePlayOgg(struct GYAudioSource*const src, const struct GYOgg*const ogg);
void gyAudSourceStop(struct GYAudioSource*const src);
int  gyAudSourceIsPlaying(struct GYAudioSource*const src);

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
