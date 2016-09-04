#include <stdio.h>
#include <stdarg.h>
#include <gyro_defines.h>
#include <string.h>
#include <gyro_system_api.h>
#include "gyro_system_callbacks.h"

static GYDebugCallbacks _dbgCallbacks = {
    NULL,
    NULL,
    NULL,
};

static GY_DEBUG_SEVERITY _dbgLevel = GY_DEBUG_WARNING;

GYAppInitCb     gyAppInitCb        = NULL;
GYAppUpdateCb   gyAppUpdateCb      = NULL;
GYAppInputCb    gyAppInputCb       = NULL;
GYAppDrawCb     gyAppDrawCb        = NULL;
GYAppQuitCb     gyAppQuitCb        = NULL;

const char* gyVersion(void) { return GYRO_LIB_VERSION; }

void _gySysPrintBuildInfo(const char*const platString) {
    char strBuff[40];

    _gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_x86_64 [%s] v%s", platString, GYRO_LIB_VERSION);
    _gyPush();
#ifdef GYRO_LOCAL_BUILD
    _gyLog(GY_DEBUG_WARNING, "THIS IS AN UNOFFICIAL BUILD!");
#else
    _gyLog(GY_DEBUG_VERBOSE, "THIS IS AN OFFICIAL BUILD!");
#endif
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Git branch name",  GYRO_STRINGIFY_MACRO(GYRO_BUILD_BRANCH));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Git commit ID",    GYRO_STRINGIFY_MACRO(GYRO_BUILD_COMMIT));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build number",     GYRO_STRINGIFY_MACRO(GYRO_BUILD_NUMBER));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build node",       GYRO_STRINGIFY_MACRO(GYRO_BUILD_NODE));
    sprintf(strBuff, "%s %s", __DATE__, __TIME__);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build timestamp", strBuff);
    _gyPop(1);


}

void _gyLog(GY_DEBUG_SEVERITY level, const char*const str, ...) {
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;

    if(_dbgLevel == GY_DEBUG_OFF || level < _dbgLevel) return;

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    if(_dbgCallbacks.printf) _dbgCallbacks.printf(level, buffer);
    else gyPrintf(buffer);
}

void _gyPush(void) {
    if(_dbgCallbacks.push) _dbgCallbacks.push();
}

void _gyPop(const unsigned count) {
    if(_dbgCallbacks.pop) _dbgCallbacks.pop(count);
}

void gySysDebugEnable(GY_DEBUG_SEVERITY level, const GYDebugCallbacks* callbacks) {
    _dbgLevel = level;
    memcpy(&_dbgCallbacks, callbacks, sizeof(GYDebugCallbacks));
}

void gySysRegisterAppInit(GYAppInitCb init_cb)
{
    gyAppInitCb = init_cb;
}

void gySysRegisterAppUpdate(GYAppUpdateCb update_cb)
{
    gyAppUpdateCb = update_cb;
}

void gySysRegisterAppInput(GYAppInputCb input_cb)
{
    gyAppInputCb = input_cb;
}

void gySysRegisterAppDraw(GYAppDrawCb draw_cb)
{
    gyAppDrawCb = draw_cb;
}

void gySysRegisterAppQuit(GYAppQuitCb quit_cb)
{
    gyAppQuitCb = quit_cb;
}


