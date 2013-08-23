#ifndef GYRO_WAV_H
#define GYRO_WAV_H

#include "gyro_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __GYWavPayload;
typedef struct __GYWavPayload _GYWavPayload;

typedef struct _GYWav {
    char filename[GYRO_FILENAME_MAX_SIZE];
    unsigned int bytes;
	_GYWavPayload *_data;
} GYWav;

GYWav *gyAudWavCreate(const char *const filename);
int gyAudWavDestroy(GYWav *const wav);
unsigned int gyAudWavAmount(void);
int gyAudWavClear(void);
void gyAudWavWalkthrough(void);

//===PRIVATE - INVOKED BY AUDIO SUBSYSTEM====
int _gyAudWavInit(void);
int _gyAudWavUninit(void);

//===PLATFORM SPECIFIC PRIVATE====
int _gyAudWavAlloc(GYWav *const wav);
int _gyAudWavFree(GYWav *const wav);

#ifdef __cplusplus
}
#endif

#endif // GYRO_WAV_H
