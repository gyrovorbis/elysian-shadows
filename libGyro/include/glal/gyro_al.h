#ifndef GYRO_AL_H
#define GYRO_AL_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	ifdef __APPLE__
#		include <OpenAL/al.h>
#		include <OpenAL/alc.h>
#       define __gl_h_

#	else
#       define AL_LIBTYPE_STATIC
#		include <AL/al.h>
#       include <AL/alc.h>
#	endif
#endif

extern ALenum              _alFmtStereo32;
extern ALenum              _alFmtMono32;
extern ALboolean           _alExtFloat32;

#ifdef __cplusplus
}
#endif


#endif // GYRO_AL_H

