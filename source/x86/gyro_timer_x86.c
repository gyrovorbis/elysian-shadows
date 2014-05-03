#include <gyro_timer_api.h>
#include <x86/gyro_glfw_x86.inl>

int gyTimerInit(void) {
    return 1;
}

unsigned long int gyTimerTicks(void) {
    return (unsigned long int) (glfwGetTime() * 1000);
}

void gyTimerSleep(const unsigned long duration) {
	(void)duration;
	SDL_Delay(duration);
}
