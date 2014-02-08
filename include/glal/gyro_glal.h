#ifndef GYRO_GL_GLAL_H
#define GYRO_GL_GLAL_H

#include "gyro_platform.h"

#ifdef WIN32
 // :D Mmmmmm
#include <windows.h>
#endif
//this is no good
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#	ifdef __APPLE__
#		include <OpenAL/al.h>
#		include <OpenAL/alc.h>
#	else
#		include <AL/al.h>
#       include <AL/alc.h>
#	endif
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#	include "ios/ios_system.h"
#	include "ios/gyro_gles_ios.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_droid
#	include "droid/droid_system.h"
#	include "droid/gyro_gles_droid.h"
#endif

#endif

