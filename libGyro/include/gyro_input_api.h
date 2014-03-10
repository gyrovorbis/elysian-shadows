#ifndef GYRO_INPUT_H
#define GYRO_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_platform.h"

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
