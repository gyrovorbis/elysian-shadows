#ifndef GYRO_WAV_H
#define GYRO_WAV_H

#include "gyro_defines.h"
#include "common/gyro_wav_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYWav {
	char			filename[GYRO_PATH_MAX_SIZE];
	unsigned int	bytes;
	void*			_data;
} GYWav;

//===PRIVATE - INVOKED BY AUDIO SUBSYSTEM====
int _gyAudWavInit(void);
int _gyAudWavUninit(void);

//===PLATFORM SPECIFIC PRIVATE====
int _gyAudWavAlloc(GYWav *const wav);
int _gyAudWavFree(GYWav *const wav);

#ifdef GYRO_WAV_PLATFORM_INCLUDE
#	include GYRO_WAV_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_WAV_H
