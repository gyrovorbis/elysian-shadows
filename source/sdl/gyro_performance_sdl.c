#include <libGyro/gyro_performance_api.h>
#include <libGyro/gyro_platform.h>
#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
#include <SDL2/SDL_timer.h>

#elif defined(__WIN32)
#include <sdl/SDL.h>
#include <sdl/SDL_timer.h>

#elif defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#else
#include <SDL.h>
#include <SDL_timer.h>
#endif

uint64_t _gyPerfFreqCached = 0;

uint64_t gyPerfCounter(void) {
    return SDL_GetPerformanceCounter();
}

uint64_t gyPerfFrequency(void) {
    return (_gyPerfFreqCached = SDL_GetPerformanceFrequency());
}

int gyPerfInit(const GYPerfInitParams* params) {
    (void)params;
    gyPerfFrequency(); //fill cache
    return 1;
}

int gyPerfUninit(void) {
    return 1;
}

