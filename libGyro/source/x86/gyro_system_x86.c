#include "gyro_system.h"
#include "gyro_defines.h"
#include "x86/gyro_sdl_x86.h"
#include "glal/gyro_video_glal.h"
#include "stdio.h"
#include "stdarg.h"


int gySysInit(void) {
	_gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_x86 v%s\n", GYRO_LIB_VERSION);
    return 1;
}

int gySysUninit(void) {
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    fprintf(stdout, "%s", buffer);
	fflush(stdout);
}

unsigned int gySysStatsVram(void) {
    return 0;
}

unsigned int gySysStatsRam(void) {
    return 0;
}

