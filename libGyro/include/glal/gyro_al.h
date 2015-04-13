#ifndef GYRO_GL_AL_H
#define GYRO_GL_AL_H

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
#	include "ios/gyro_gles_ios.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID

#	define AL_LIBTYPE_STATIC
#	include <AL/al.h>
#	include <AL/alc.h>

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	ifdef __APPLE__
#		include <OpenAL/al.h>
#		include <OpenAL/alc.h>

#	else
//#if !defined(__gl_h_) && !defined(__GL_H__) && !defined(__X_GL_H)
//#endif
#      // define QOPENGL_H //THIS WILL REEST GLEW OTHERWISE!!!
#       include <windows.h>
#       define AL_LIBTYPE_STATIC
#		include <AL/al.h>
#       include <AL/alc.h>
#	endif
#endif

#endif

