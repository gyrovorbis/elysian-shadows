#ifndef GYRO_SDL_X86_H
#define GYRO_SDL_X86_H

#ifdef WIN32 // :D Mmmmmm
#   include <windows.h>
#   include <SDL/SDL.h>
#endif

#ifdef __linux__ // :) Sweet!
#   include <SDL/SDL.h>
#endif

#ifdef __APPLE__ // D: Fuck my life...
#	include <SDL.h>
#endif

/*
#ifdef WIN32 Yeah, WHAT?
#   include "sdl.h"
#endif
 */
//#else
//#endif

#endif // GYRO_SDL_X86_H
