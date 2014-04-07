#include <gyro_system_api.h>
#include <gyro_defines.h>
#include <stdio.h>
#include <stdarg.h>

static void (*_dbgFunc)(const GY_DEBUG_LEVEL, const char *const) = NULL;

static GY_DEBUG_LEVEL _dbgLevel = GY_DEBUG_CRITICAL;

static GYAppInitCb appInitCb = NULL;
static GYAppTickCb appTickCb = NULL;
static GYAppDrawCb appDrawCb = NULL;
static GYAppQuitCb appQuitCb = NULL;

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
	appInitCb = init_cb;
}

void gySysRegisterAppTick(GYAppTickCb tick_cb)
{
	appTickCb = tick_cb;
}

void gySysRegisterAppDraw(GYAppDrawCb draw_cb)
{
	appDrawCb = draw_cb;
}

void gySysRegisterAppQuit(GYAppQuitCb quit_cb)
{
	appQuitCb = quit_cb;
}

int main(int argc, char **argv)
{
	gyro_main();
	if (appInitCb != NULL) {
		if (!appInitCb(argc, argv)) {
			return -1;
		}
	}
	do {
		if (appTickCb != NULL) {
			if (!appTickCb()) {
				break;
			}
		}
		if (appDrawCb != NULL) {
			if (!appDrawCb()) {
				break;
			}
		}
	} while (1);
	if (appQuitCb != NULL) {
		if (!appQuitCb()) {
			return -2;
		}
	}
	return 0;
}
