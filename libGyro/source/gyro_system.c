#include "gyro_system.h"
#include "gyro_defines.h"
#include "stdio.h"
#include "stdarg.h"

static void (*_dbgFunc)(const GY_DEBUG_LEVEL, const char *const) = NULL;

static GY_DEBUG_LEVEL _dbgLevel = GY_DEBUG_CRITICAL;

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
