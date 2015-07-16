#include <gyro_timer_api.h>



int gyTimerInit(void) {
    return 1;
}

int gyTimerUninit(void) {
    return 1;
}

int64_t gyTimerTicks(void) {
    return 16666666;
}


void gyTimerSleep(const unsigned long duration) {
}
