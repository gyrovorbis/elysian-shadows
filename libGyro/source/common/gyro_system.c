#include <stdio.h>
#include <stdarg.h>

#include <gyro_defines.h>
#include <gyro_system_api.h>

#include "gyro_system_callbacks.h"

static void (*_dbgFunc)(const GY_DEBUG_LEVEL, const char *const) = NULL;

static GY_DEBUG_LEVEL _dbgLevel = GY_DEBUG_CRITICAL;

GYAppInitCb gy_appInitCb = NULL;
GYAppTickCb gy_appTickCb = NULL;
GYAppDrawCb gy_appDrawCb = NULL;
GYAppQuitCb gy_appQuitCb = NULL;

void _gyLog(GY_DEBUG_LEVEL level, const char*const str, ...) {
    char buffer[GYRO_PRINTF_BUFFER_SIZE];
    va_list argptr;

	if(_dbgLevel == GY_DEBUG_OFF || level < _dbgLevel) return;

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    if(_dbgFunc) _dbgFunc(level, buffer);
    else gyPrintf(buffer);
}

void gySysDebugEnable(GY_DEBUG_LEVEL level, void(*func)(const GY_DEBUG_LEVEL, const char *const)) {
    _dbgLevel = level;
    _dbgFunc = func;
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
