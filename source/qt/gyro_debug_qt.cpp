#include <QDebug>
#include <stdio.h>
#include <stdarg.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>

int gyDebugInit(void) {
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    printf(buffer);
    fflush(stdout);

    //QString qBuff(buffer);
    //qBuff.chop(1);

    //qDebug() << qBuff;
}

int gyDebugUninit(void) {
    return 1;
}
