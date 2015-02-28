#include <gyro_timer_api.h>
#include <QElapsedTimer.h>
#include <QApplication>

static QElapsedTimer* timer = NULL;

int gyTimerInit(void) {
    timer = new QElapsedTimer;
    //timer->start();
    return 1;
}

int gyTimerUninit(void) {
    delete timer;
    return 1;
}

unsigned long int gyTimerTicks(void) {
    return timer->restart();
}


void gyTimerSleep(const unsigned long duration) {
    (void)duration;
    QApplication::processEvents();
}
