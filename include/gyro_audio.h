#ifndef GYRO_AUDIO_H
#define GYRO_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

//100% implementation-dependent
typedef void* GYWavSource;

struct _GYWav;
struct _GYVector3;

int gyAudInit(void);
int gyAudUninit(void);

int gyAudOggStart(const char *const filename);
int gyAudOggStop(void);

void gyAudWavPlay(const struct _GYWav *const wav);
//GYWav* gyAudWavCreate(const char* path);
//void gyAudWavDestroy(struct _GYWav* const wav);


typedef enum _GY_SOURCE_ATTRIB {
	GY_SOURCE_POS,
	GY_SOURCE_VEL,
	GY_SOURCE_GAIN,
	GY_SOURCE_PITCH,
	GY_SOURCE_LOOPING
} GY_SOURCE_ATTRIB;


//audio listener
void gyAudListenerSetPos(const struct _GYVector3* const pos);
void gyAudSetReferenceDistance(const int dist);
void gyAudSetZScale(const float scale);

//audio source
GYWavSource* gyAudSourceCreate(void);
void gyAudSourceDestroy(GYWavSource*const src);
void gyAudSourceSetfv(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const struct _GYVector3*const vec);
void gyAudSourceSetf(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const float val);
void gyAudSourceSeti(GYWavSource*const src, const GY_SOURCE_ATTRIB attrib, const int val);
void gyAudSourcePlayWav(GYWavSource*const src, const struct _GYWav* wav);


#ifdef __cplusplus
}
#endif

#endif // GYRO_AUDIO_H
