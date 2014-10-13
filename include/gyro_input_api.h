#ifndef GYRO_INPUT_H
#define GYRO_INPUT_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _GYInput;

int gyInputInit(void);
int gyInputUninit(void);

void gyInputPoll(struct _GYInput *const input);

#ifdef GYRO_INPUT_PLATFORM_INCLUDE
#	include GYRO_INPUT_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
