#include "gyro_timer.h"
#include "x86/gyro_sdl_x86.h"


int gyTimerInit(void) {
    return 1;
}

unsigned long int gyTimerTicks(void) {
    return SDL_GetTicks();
}

void gyTimerSleep(const unsigned long duration) {
    
}
