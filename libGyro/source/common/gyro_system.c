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

static GY_DEBUG_SEVERITY _dbgLevel = GY_DEBUG_DBG_WARNING;

GYAppInitCb gy_appInitCb = NULL;
GYAppTickCb gy_appTickCb = NULL;
GYAppDrawCb gy_appDrawCb = NULL;
GYAppQuitCb gy_appQuitCb = NULL;

void _gyLog(GY_DEBUG_SEVERITY level, const char*const str, ...) {
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;

    if(_dbgLevel == GY_DEBUG_DBG_OFF || level < _dbgLevel) return;

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
	gy_appInitCb = init_cb;
}

void gySysRegisterAppTick(GYAppTickCb tick_cb)
{
	gy_appTickCb = tick_cb;
}

void gySysRegisterAppDraw(GYAppDrawCb draw_cb)
{
	gy_appDrawCb = draw_cb;
}

void gySysRegisterAppQuit(GYAppQuitCb quit_cb)
{
	gy_appQuitCb = quit_cb;
}

#if GYRO_PLATFORM != GYRO_PLATFORM_Qt
int gyro_entry_point(int argc, char **argv)
{
	gyro_main();
	if (gy_appInitCb != NULL) {
		if (!gy_appInitCb(argc, argv)) {
			return -1;
		}
	}
	do {
		if (gy_appTickCb != NULL) {
			if (!gy_appTickCb()) {
				break;
			}
		}
        if (gy_appDrawCb != NULL) {
			if (!gy_appDrawCb()) {
				break;
			}
		}
	} while (1);
	if (gy_appQuitCb != NULL) {
		if (!gy_appQuitCb()) {
			return -2;
		}
	}
	return 0;
}
#endif
