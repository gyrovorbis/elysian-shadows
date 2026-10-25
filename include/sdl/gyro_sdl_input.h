/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_SDL_INPUT_H
#define GYRO_SDL_INPUT_H

#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_haptic.h>

#elif defined(__WIN32)
#include <sdl/SDL.h>
#include <sdl/SDL_joystick.h>
#include <sdl/SDL_gamecontroller.h>
#include <sdl/SDL_haptic.h>

#elif defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_haptic.h>

#else
#include <SDL.h>
#include <SDL_joystick.h>
#include <SDL_gamecontroller.h>
#include <SDL_haptic.h>
#endif

#endif // GYRO_SDL_INPUT_H
