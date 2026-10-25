/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_SYSTEM_API_H
#define GYRO_SYSTEM_API_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_LIB_VERSION_MAJOR  1
#define GYRO_LIB_VERSION_MINOR  6
#define GYRO_LIB_VERSION_PATCH  0

#define GYRO_LIB_VERSION GYRO_STRINGIFY_VERSION(GYRO_LIB_VERSION_MAJOR, GYRO_LIB_VERSION_MINOR, GYRO_LIB_VERSION_PATCH)

#ifndef GYRO_BUILD_NODE
#   define GYRO_LOCAL_BUILD
#   define GYRO_BUILD_NODE     LOCAL
#endif
#ifndef GYRO_BUILD_JOB
#   define GYRO_BUILD_JOB      UNKNOWN
#endif
#ifndef GYRO_BUILD_NUMBER
#   define GYRO_BUILD_NUMBER   UNKNOWN
#endif
#ifndef GYRO_BUILD_BRANCH
#   define GYRO_BUILD_BRANCH   UNKNOWN
#endif
#ifndef GYRO_BUILD_COMMIT
#   define GYRO_BUILD_COMMIT   UNKNOWN
#endif


#define GYRO_SYSTEM_PLATFORM_NAME_SIZE  100

typedef enum GY_SYSTEM_POWER_STATE {
    GY_SYSTEM_POWER_STATE_UNKNOWN,
    GY_SYSTEM_POWER_STATE_BATTERY,
    GY_SYSTEM_POWER_STATE_AC,
    GY_SYSTEM_POWER_STATE_CHARGING,
    GY_SYSTEM_POWER_STATE_CHARGED,
} GY_SYSTEM_POWER_STATE;


typedef struct GYSystemPowerInfo {
    GY_SYSTEM_POWER_STATE     state;
    int                       batterySecondsRemaining;
    int                       batteryPercentageRemaining;
} GYSystemPowerInfo;

typedef struct GYSystemInfo {
    char                        platformName[GYRO_SYSTEM_PLATFORM_NAME_SIZE];
    char                        osInfo[GYRO_SYSTEM_PLATFORM_NAME_SIZE];
    int                         cpuCount;
    int                         cacheLineSize;  //bytes, multi-threaded padding and SSE
    int                         systemRam;      //Megabytes
    int                         has3DNow;
    int                         hasAvx;
    int                         hasAvx2;
    int                         hasAltiVec;
    int                         hasMMX;
    int                         hasRdtsc;
    int                         hasSSE;
    int                         hasSSE2;
    int                         hasSSE3;
    int                         hasSSE41;
    int                         hasSSE42;
    int                         byteOrder; //0 - little, 1 - big
} GYSystemInfo;

typedef struct GYSysInitParams {
    void*   context;
    void    (*segFaultHandler)(void*);

} GYSysInitParams;

typedef enum _GY_DEBUG_SEVERITY { GY_DEBUG_VERBOSE, GY_DEBUG_WARNING, GY_DEBUG_ERROR, GY_DEBUG_OFF } GY_DEBUG_SEVERITY;

int gySysInit(const GYSysInitParams*const params);
int gySysUninit(void);

int gyDebugInit(void);
int gyDebugUninit(void);

void gyPrintf(const char *const str, ...);

typedef enum {
	GY_RET_FINISH = 0,
	GY_RET_CONTINUE
} GYAppCbRet;

typedef GYAppCbRet (*GYAppInitCb)(int argc, char **argv);
typedef GYAppCbRet (*GYAppUpdateCb)(void);
typedef GYAppCbRet (*GYAppInputCb)(void);
typedef GYAppCbRet (*GYAppDrawCb)(void);
typedef GYAppCbRet (*GYAppQuitCb)(void);

void gySysRegisterAppInit(GYAppInitCb init_cb);
void gySysRegisterAppUpdate(GYAppUpdateCb update_cb);
void gySysRegisterAppInput(GYAppInputCb input_cb);
void gySysRegisterAppDraw(GYAppDrawCb draw_cb);
void gySysRegisterAppQuit(GYAppQuitCb quit_cb);
int gySysAppExec(int argc, char* argv[]);

unsigned int gySysStatsVram(void);
unsigned int gySysStatsRam(void);

const char* gyVersion(void);

const GYSystemInfo* gySystemQueryInfo(void);
int gySystemPowerQueryInfo(GYSystemPowerInfo* info);
const char* gySystemPowerStateString(GY_SYSTEM_POWER_STATE state);


#include "common/gyro_system_common.h"

#ifdef GYRO_SYSTEM_PLATFORM_INCLUDE
#	include GYRO_SYSTEM_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
