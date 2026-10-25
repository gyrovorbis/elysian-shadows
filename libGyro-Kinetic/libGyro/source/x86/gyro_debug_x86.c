#include <stdio.h>
#include <stdarg.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    fprintf(stdout, "%s", buffer);
    fflush(stdout);
}

