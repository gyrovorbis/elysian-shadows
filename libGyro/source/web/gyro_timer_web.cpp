#include <gyro_performance_api.h>

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>

int gyTimerInit(void) {
    return 1;
}

int gyTimerUninit(void) {
    return 1;
}

int64_t gyTimerTicks(void) {
    // return SDL_GetTicks();
  return 0;
}
