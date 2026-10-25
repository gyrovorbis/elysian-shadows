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

#define GY_ANG2RAD 3.14159265358979323846/180.0


#ifdef GYRO_MATH_PLATFORM_INCLUDE
#	include	GYRO_MATH_PLATFORM_INCLUDE
#else
void gySinCos(const float theta, float *const psin, float *const pcos);
float gySqrt(const float val);
#endif

#define GYRO_LINEAR_INTERP(cur, prev, next, interval, elapsed) do { \
                                                                    cur = (float)((float)prev + (float)((float)next-(float)prev)*(float)((float)elapsed/(float)interval)); \
                                                                } while(0)



//This could so be SIMD accelerated to all fuck for vectors.
static inline void gyLinearInterp(float* current, const unsigned int count, const float *previous, const float *next, const float interval, const float elapsed) {
    for(unsigned i = 0; i < count; ++i) current[i] = (previous[i] + (next[i]-previous[i])*(elapsed/interval));
}

#ifdef __cplusplus
}
#endif
	
#endif
