#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_defines.h"

//100% implementation-dependent
typedef void* GYWavSource;

struct _GYWav;
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


typedef struct _GYOgg {
	char path[GYRO_PATH_MAX_SIZE];
	unsigned int bytes;
	unsigned looping;
	void* _data;
} GYOgg;


GYOgg* gyAudOggCreate(const char*const path);
void gyAudOggDestroy(GYOgg*const ogg);

//audio listener
void gyAudListenerSetPos(const struct _GYVector3* const pos);
void gyAudSetReferenceDistance(const int dist);
void gyAudSetZScale(const float scale);

//audio source
GYWavSource* gyAudSourceCreate(void);
void gyAudSourceDestroy(GYWavSource*const src);
void gyAudSourceSetfv(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const struct _GYVector3*const vec);
void gyAudSourceSetf(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const float val);
float gyAudSourceGetf(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib);
void gyAudSourceSeti(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const int val);
void gyAudSourcePlayWav(GYWavSource*const src, const struct _GYWav*const wav);
void gyAudSourcePlayOgg(GYWavSource*const src, const struct _GYOgg*const ogg);
void gyAudSourceStop(GYWavSource*const src);
int  gyAudSourceIsPlaying(GYWavSource*const src);

GYWavSource* gyAudOggStreamCreate(void);
void gyAudOggStreamDestroy(GYWavSource*const src);
void gyAudOggStreamPlay(GYWavSource*src, GYOgg*ogg);
void gyAudOggStreamPause(GYWavSource*src);
void gyAudOggStreamStop(GYWavSource*src);
void gyAudOggStreamSetLooping(GYWavSource*const src, const unsigned looping);
double gyAudOggStreamTime(GYWavSource*const src);
void gyAudOggStreamSync(GYWavSource*const src1, GYWavSource*const src2);


#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
