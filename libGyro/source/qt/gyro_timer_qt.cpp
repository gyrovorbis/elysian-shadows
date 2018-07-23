#include <gyro_timer_api.h>
#include <QElapsedTimer>
#include <QApplication>

void gyTimerSleep(const unsigned long duration) {
    (void)duration;
    QApplication::processEvents();
}
