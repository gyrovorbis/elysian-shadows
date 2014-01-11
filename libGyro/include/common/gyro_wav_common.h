#ifndef GYRO_WAV_SHARED_H
#define GYRO_WAV_SHARED_H

#include "../gyro_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _GYWav;

struct _GYWav *gyAudWavCreate(const char *const filename);
int gyAudWavDestroy(struct _GYWav *const wav);
unsigned int gyAudWavAmount(void);
int gyAudWavClear(void);
void gyAudWavWalkthrough(void);

#ifdef __cplusplus
}
#endif


#endif // GYRO_WAV_SHARED_H
