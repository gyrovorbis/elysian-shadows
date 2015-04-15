#ifndef GYRO_AL_H
#define GYRO_AL_H

#include "gyro_platform.h"

//this is no good
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#	ifdef __APPLE__
#		include <OpenAL/al.h>
#		include <OpenAL/alc.h>
#	else
#       define AL_LIBTYPE_STATIC
#		include <AL/al.h>
#       include <AL/alc.h>
#	endif
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#	include "ios/ios_system.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID

#	define AL_LIBTYPE_STATIC
#	include <AL/al.h>
#	include <AL/alc.h>

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	ifdef __APPLE__
//#       include <gl.h>
#		include <OpenAL/al.h>
#		include <OpenAL/alc.h>
#define __gl_h_

#	else
#      // define QOPENGL_H //THIS WILL REEST GLEW OTHERWISE!!!
#
#       define AL_LIBTYPE_STATIC
#		include <AL/al.h>
#       include <AL/alc.h>
#	endif
#endif

#endif // GYRO_AL_H

