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
	char path[GYRO_FILENAME_MAX_SIZE];
	unsigned int bytes;
	unsigned looping;
	void* _data;
} GYOgg;


GYOgg* gyAudCreateOgg(const char*const path);
void gyAudDestroyOgg(GYOgg*const ogg);

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

void gyAudOggUpdate(GYWavSource* const src, struct _GYOgg* const ogg);
void gyAudStopOggStream(GYWavSource*const src, GYOgg*const ogg);
void gyAudPauseOggStream(GYWavSource*const src, GYOgg*const ogg);
void gyAudOggSync(GYOgg*const ogg1, GYOgg*const ogg2);
double gyAudOggTime(GYOgg*const ogg);
void gyAudOggSetLooping(GYOgg*const ogg, const unsigned looping);


//NEWFANGLED API!!!!!!
GYWavSource* gyAudSourceOggStreamCreate(void);
void gyAudSourceOggStreamDestroy(GYWavSource*const src);
void gyAudOggStreamPlay(GYWavSource*src, GYOgg*ogg);
void gyAudOggStreamPause(GYWavSource*src);
void gyAudOggStreamStop(GYWavSource*src);





#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
