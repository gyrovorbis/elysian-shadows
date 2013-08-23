#ifndef GYRO_MATH_H
#define GYRO_MATH_H

#include "gyro_platform.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

//Fucking VS has no PI!
#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795f
#endif

#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#   ifdef GYRO_MATH_ACCELERATION
#       include "dc/gyro_math_dc.inl"
#   else
#       include "soft/gyro_math_soft.inl"
#   endif
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#   ifdef GYRO_MATH_SOFTWARE
#       include "soft/gyro_math_soft.inl"
#   endif
#else 
#	include "soft/gyro_math_soft.inl"
#endif

#ifdef __cplusplus
}
#endif
	
#endif
