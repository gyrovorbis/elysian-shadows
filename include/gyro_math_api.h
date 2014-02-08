#ifndef GYRO_MATH_H
#define GYRO_MATH_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

//Fucking VS has no PI!
#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795f
#endif


#ifdef GYRO_MATH_PLATFORM_INCLUDE
#	include	GYRO_MATH_PLATFORM_INCLUDE
#else
void gySinCos(const float theta, float *const psin, float *const pcos);
float gySqrt(const float val);
#endif

#ifdef __cplusplus
}
#endif
	
#endif
