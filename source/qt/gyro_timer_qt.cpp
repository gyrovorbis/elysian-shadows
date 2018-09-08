#include <libGyro/gyro_timer_api.h>
#include <QElapsedTimer>
#include <QApplication>

static QElapsedTimer* timer = NULL;

int gyTimerInit(void) {
    timer = new QElapsedTimer;
    timer->start();
    return 1;
}

int gyTimerUninit(void) {
    delete timer;
    return 1;
}

int64_t gyTimerTicks(void) {
    return timer->nsecsElapsed();
}


void gyTimerSleep(const unsigned long duration) {
    (void)duration;
    QApplication::processEvents();
}
