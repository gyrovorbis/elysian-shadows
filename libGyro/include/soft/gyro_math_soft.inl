#include <math.h>
#include <xmmintrin.h>

inline static void gySinCos(const float theta, float *const psin, float *const pcos) {
    *psin = sinf(theta); *pcos = cosf(theta);
}

inline static float gySqrt(const float val) {
	float retVal;
	__m128 regs = _mm_load1_ps(&val);
	regs = _mm_sqrt_ss(regs);
	_mm_store_ss(&retVal, regs);
	return retVal;
}
