#ifndef GYRO_SYSTEM_API_H
#define GYRO_SYSTEM_API_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_LIB_VERSION "1.5.8"

#define _GYRO_STRINGIFY_MACRO(a) #a
#define GYRO_STRINGIFY_MACRO(a) _GYRO_STRINGIFY_MACRO(a)

#ifndef GYRO_BUILD_NODE
#   define GYRO_BUILD_NODE     LOCAL
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

typedef struct GYSysParams {
    void*   context;
    void    (*segFaultHandler)(void*);

} GYSysParams;

typedef enum _GY_DEBUG_LEVEL { GY_DEBUG_VERBOSE, GY_DEBUG_CRITICAL, GY_DEBUG_FATAL, GY_DEBUG_OFF } GY_DEBUG_LEVEL;

int gySysInit(const GYSysParams*const params);
int gySysUninit(void);

int gyDebugInit(void);
int gyDebugUninit(void);

void gyPrintf(const char *const str, ...);

typedef enum {
	GY_RET_FINISH = 0,
	GY_RET_CONTINUE
} GYAppCbRet;

typedef GYAppCbRet (*GYAppInitCb)(int argc, char **argv);
typedef GYAppCbRet (*GYAppTickCb)(void);
typedef GYAppCbRet (*GYAppDrawCb)(void);
typedef GYAppCbRet (*GYAppQuitCb)(void);

void gySysRegisterAppInit(GYAppInitCb init_cb);
void gySysRegisterAppTick(GYAppTickCb tick_cb);
void gySysRegisterAppDraw(GYAppDrawCb draw_cb);
void gySysRegisterAppQuit(GYAppQuitCb quit_cb);

unsigned int gySysStatsVram(void);
unsigned int gySysStatsRam(void);

void gyro_main(void);

#include "common/gyro_system_common.h"

#ifdef GYRO_SYSTEM_PLATFORM_INCLUDE
#	include GYRO_SYSTEM_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
