#ifndef GYRO_SYSTEM_API_H
#define GYRO_SYSTEM_API_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_LIB_VERSION_MAJOR  1
#define GYRO_LIB_VERSION_MINOR  6
#define GYRO_LIB_VERSION_PATCH  0

#define _GYRO_STRINGIFY_MACRO(a) #a
#define GYRO_STRINGIFY_MACRO(a) _GYRO_STRINGIFY_MACRO(a)

//#define GYRO_LIB_VERSION GYRO_STRINGIFY_MACRO(GYRO_LIB_VERSION_MAJOR)##"."##GYRO_STRINGIFY_MACRO(GYRO_LIB_VERSION_MINOR)##"."##GYRO_STRINGIFY_MACRO(GYRO_LIB_VERSION_PATCH)
#define GYRO_LIB_VERSION    "1.6.0"


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


#include "common/gyro_system_common.h"

#ifdef GYRO_SYSTEM_PLATFORM_INCLUDE
#	include GYRO_SYSTEM_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
