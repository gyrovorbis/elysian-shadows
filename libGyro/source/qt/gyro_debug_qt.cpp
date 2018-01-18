#include <QDebug>
#include <stdio.h>
#include <stdarg.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>

static QTextStream* _stream = nullptr;

extern "C" int gyQtInit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Initializing Qt");
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "%-30s: %20s", "Qt Version (Compile-Time)",    QT_VERSION_STR);
    _gyLog(GY_DEBUG_VERBOSE, "%-30s: %20s", "Qt Version (Run-Time)",        qVersion());
    _gyPop(1);
    return 1;
}

int gyDebugInit(void) {
    _stream = new QTextStream(stdout);
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    //printf("%s", buffer); Still won't force Qt's fucking debug log to update!
    //fflush(stdout);

    QString qBuff(buffer);
    /*if(qBuff.endsWith("\n")) {
        qBuff.chop(1);

    }*/
    *_stream << qBuff;
    _stream->flush();

    //QString qBuff(buffer);
    //qBuff.chop(1);

    //qDebug() << qBuff;
}

int gyDebugUninit(void) {
    delete _stream;
    return 1;
}
