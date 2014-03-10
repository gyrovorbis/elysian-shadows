#ifndef GYRO_SYSTEM_H
#define GYRO_SYSTEM_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_LIB_VERSION "1.5.6"

typedef enum _GY_DEBUG_LEVEL { GY_DEBUG_VERBOSE, GY_DEBUG_CRITICAL, GY_DEBUG_FATAL, GY_DEBUG_OFF } GY_DEBUG_LEVEL;

int gySysInit(void);
int gySysUninit(void);

void gyPrintf(const char *const str, ...);

unsigned int gySysStatsVram(void);
unsigned int gySysStatsRam(void);

#include "common/gyro_system_common.h"

#ifdef GYRO_SYSTEM_PLATFORM_INCLUDE
#	include GYRO_SYSTEM_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
