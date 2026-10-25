#include <gyro_timer_api.h>
#include <x86/gyro_glfw_x86.inl>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

int gyTimerInit(void) {
    return 1;
}

unsigned long int gyTimerTicks(void) {
    return (unsigned long int) (glfwGetTime() * 1000);
}

void gyTimerSleep(const unsigned long duration) {
    (void)duration;
    Sleep(duration);
}
