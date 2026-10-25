/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_video_api.h>
#include <gyro_system_api.h>
#include "gyro_defines.h"
#include <gl/gyro_gl.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

GYVidInitParams vidParams;
int vidBpp;


int gyVidInit(const GYVidInitParams *const params) {
     SDL_Window* window = SDL_CreateWindow("Elysian VMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 144, 96, SDL_WINDOW_OPENGL);

      if (!window) {
        _gyLog(GY_DEBUG_VERBOSE, "SDL_CreateWindow failed: v%s\n", SDL_GetError());
        return 0;
      }

      if (!SDL_GL_CreateContext(window)) {
        _gyLog(GY_DEBUG_VERBOSE, "SDL_GL_CreateContext failed: v%s\n", SDL_GetError());
        return 0;
      }


    return 1;
}

int gyVidUninit() {

}
