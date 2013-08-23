#ifndef GYRO_INPUT_H
#define GYRO_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_platform.h"

#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#	include "dc/gyro_input_dc.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_x86
#	include "x86/gyro_input_x86.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#	include "ios/gyro_input_ios.h"
#else
	typedef void GYInput;
#endif

int gyInputInit(void);
int gyInputUninit(void);

void gyInputPoll(GYInput *const input);

#ifdef __cplusplus
}
#endif

#endif
