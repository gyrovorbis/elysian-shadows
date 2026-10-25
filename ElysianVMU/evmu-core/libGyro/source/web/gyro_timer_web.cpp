#include <gyro_performance_api.h>

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>

extern "C" int gyTimerInit(void) {
    return 1;
}

extern "C" int gyTimerUninit(void) {
    return 1;
}

extern "C" int64_t gyTimerTicks(void) {
    // return SDL_GetTicks();
  return 0;
}
