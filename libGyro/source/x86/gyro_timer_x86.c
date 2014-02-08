#include <gyro_timer_api.h>
#include <x86/gyro_sdl_x86.inl>

int gyTimerInit(void) {
    return 1;
}

unsigned long int gyTimerTicks(void) {
    return SDL_GetTicks();
}

void gyTimerSleep(const unsigned long duration) {
	(void)duration;
}
