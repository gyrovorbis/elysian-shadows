#include <gyro_system_api.h>
#include <stdio.h>
#include <stdarg.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>
#import <UIKit/UIKit.h>

int gyDebugUninit(void) {
    return 1;
}


int gyDebugInit(void) {
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);
    NSLog(@"%s", buffer);
    printf("%s", buffer);
    fflush(stdout);
}
