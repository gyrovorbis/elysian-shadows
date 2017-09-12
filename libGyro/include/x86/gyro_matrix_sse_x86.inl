#include <intrin.h>
#include <xmmintrin.h>
#include <string.h>
#include <assert.h>
#include "gyro_platform.h"


#ifdef _WIN32
#   define MM_LOAD_ALIGN(a)         _mm_loadu_ps(a)
#   define MM_STORE_ALIGN(a, b)     _mm_storeu_ps(a, b)
#else
#   define MM_LOAD_ALIGN(a)         _mm_load_ps(a)
#   define MM_STORE_ALIGN(a, b)     _mm_store_ps(a, b)
#endif


extern GYMatrix4 _curMat;

static inline void gyMatMultMat(GYMatrix4 dMat, const GYMatrix4 sMat1, const GYMatrix4 sMat2) {
#ifdef GYRO_MATH_NOASM
  for(int j = 0; j < 4; ++j) {
    for(int i = 0; i < 4; ++i) {
      dMat[i][j] = sMat1[0][j]*sMat2[i][0]
          + sMat1[1][j]*sMat2[i][1]
          + sMat1[2][j]*sMat2[i][2]
          + sMat1[3][j]*sMat2[i][3];
    }
  }
#else
  __m128 dest_line, src1_line, src2_line;
  unsigned i, j;

  for (i = 0; i < 16; i += 4) {
  //unroll the first step of the loop to avoid having to initialize dest_line to zero
        src1_line = MM_LOAD_ALIGN((float*)sMat1);									//first column
    src2_line = _mm_set1_ps(((float*)sMat2)[i]);									//initialize vector to first index of ith column
    dest_line = _mm_mul_ps(src1_line, src2_line);								//multiply
    for (j = 1; j < 4; j++) {
            src1_line = MM_LOAD_ALIGN(& (((float*)sMat1) [j<<2]) );					//jth column vector
      src2_line = _mm_set1_ps( ((float*)sMat2) [i+j] );							//initialized to value of mat[i][j]
      dest_line = _mm_add_ps(_mm_mul_ps(src1_line, src2_line), dest_line);	//multiply and accumulate column vector
    }
        MM_STORE_ALIGN(& (((float*)dMat)[i]) , dest_line);								//store column vector
  }

#endif
}

static inline void gyMatApply(GYMatrix4 mat) {
  GYMatrix4 result;
    gyMatMultMat(result, (const float (*)[4])_curMat, (const float (*)[4])mat);
  memcpy(_curMat, result, sizeof(GYMatrix4));
}


static inline void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, const unsigned int count) {
#ifdef GYRO_MATH_NOASM
  float res[4];
  unsigned int i;
  for(i = 0; i < count; ++i) {

    res[0] = _curMat[0][0]*src[i].x + _curMat[1][0]*src[i].y + _curMat[2][0]*src[i].z + _curMat[3][0]*src[i].w;
    res[1] = _curMat[0][1]*src[i].x + _curMat[1][1]*src[i].y + _curMat[2][1]*src[i].z + _curMat[3][1]*src[i].w;
    res[2] = _curMat[0][2]*src[i].x + _curMat[1][2]*src[i].y + _curMat[2][2]*src[i].z + _curMat[3][2]*src[i].w;
    res[3] = _curMat[0][3]*src[i].x + _curMat[1][3]*src[i].y + _curMat[2][3]*src[i].z + _curMat[3][3]*src[i].w;

    memcpy(dest+i, res, sizeof(float)*4);
  }

#else

  __m128 dest_line, src1_line, src2_line;
  unsigned i, j;
  //  assert(((unsigned long)dest & 15) == 0);

  //Unravel first iteration of the loop for dat optimization
  for(j = 0; j < count; ++j) {													//iterate over every vertex
        src1_line = MM_LOAD_ALIGN( (float*)_curMat );									//load first column vector
    src2_line = _mm_set1_ps( ((float*)src)[j<<2] );								//initialize vector to value of first element
    dest_line = _mm_mul_ps(src1_line, src2_line);								//multiply

    for (i=1; i < 4; ++i) {
            src1_line = MM_LOAD_ALIGN(& ( ((float*)_curMat)[i<<2]) );					//load ith column vector
      src2_line = _mm_set1_ps( ((float*)src)[(j<<2) + i] );					//initialize vector to value of ith element
      dest_line = _mm_add_ps(_mm_mul_ps(src1_line, src2_line), dest_line);	//multiply and accumulate
    }
        MM_STORE_ALIGN((float*)(&dest[j]) , dest_line);								//store column vector
  }


#endif
}

static inline void gyMatLoad(GYMatrix4 *mat) {
  memcpy(_curMat, mat, sizeof(GYMatrix4));
}

static inline void gyMatStore(GYMatrix4 *mat) {
  memcpy(mat, _curMat, sizeof(GYMatrix4));
}

static inline void gyVec4Add(void* const dest, const void* const v1, const void* const v2) {
#ifdef GYRO_MATH_NOASM
  for(int i = 0; i < 4; ++i) {
    ((float*)dest)[i] = ((float*)v1)[i]+((float*)v2)[i];
  }
#else
    const __m128 s1 = MM_LOAD_ALIGN((float*)v1);
    const __m128 s2 = MM_LOAD_ALIGN((float*)v2);
  const __m128 d = _mm_add_ps(s1, s2);
    MM_STORE_ALIGN((float*)dest, d);
#endif
}

static inline void gyVec4Sub(void* const dest, const void* const v1, const void* const v2) {
#ifdef GYRO_MATH_NOASM
  for(int i = 0; i < 4; ++i) {
    ((float*)dest)[i] = ((float*)v1)[i]-((float*)v2)[i];
  }
#else
  // assert(((unsigned long)dest & 15) == 0);
    const __m128 s1 = MM_LOAD_ALIGN((float*)v1);
    const __m128 s2 = MM_LOAD_ALIGN((float*)v2);
  const __m128 d = _mm_sub_ps(s1, s2);
    MM_STORE_ALIGN((float*)dest, d);
#endif
}

static inline void gyVec4Mult(void* const dest, const void* const v1, const void* const v2) {
#ifdef GYRO_MATH_NOASM
  for(int i = 0; i < 4; ++i) {
    ((float*)dest)[i] = ((float*)v1)[i]*((float*)v2)[i];
  }
#else
  const __m128 s1 = MM_LOAD_ALIGN((float*)v1);
    const __m128 s2 = MM_LOAD_ALIGN((float*)v2);
  const __m128 d = _mm_mul_ps(s1, s2);
    MM_STORE_ALIGN((float*)dest, d);
#endif
}

static inline void gyVec4Div(void* const dest, const void* const v1, const void* const v2) {
#ifdef GYRO_MATH_NOASM
  for(int i = 0; i < 4; ++i) {
    ((float*)dest)[i] = ((float*)v1)[i]/((float*)v2)[i];
  }
#else
  const __m128 s1 = MM_LOAD_ALIGN((float*)v1);
    const __m128 s2 = MM_LOAD_ALIGN((float*)v2);
  const __m128 d = _mm_div_ps(s1, s2);
    MM_STORE_ALIGN((float*)dest, d);
#endif
}

static inline void gyVec4Scale(void* const dest, const void* const v1, const float s) {
#ifdef GYRO_MATH_NOASM
  for(int i = 0; i < 4; ++i) {
    ((float*)dest)[i] = ((float*)v1)[i]*s;
  }
#else
  const __m128 s1 = MM_LOAD_ALIGN((float*)v1);
  const __m128 s2 = _mm_set1_ps(s);
  const __m128 d = _mm_mul_ps(s1, s2);
    MM_STORE_ALIGN((float*)dest, d);
#endif
}

#define VEC2(a) ((GYVector2*)a)
#define VEC3(a) ((GYVector3*)a)
#define VEC4(a) ((GYVector4*)a)

//=========== VECTOR 2 ==============

static inline void gyVec2Clear(void *const vec) { VEC2(vec)->x = VEC2(vec)->y = 0.0f; }

static inline int gyVec2IsZero(const void* const vec) { return ( VEC2(vec)->x == 0.0f && VEC2(vec)->y == 0.0f); }

static inline void gyVec2Add(void* const dest, const void* const v1, const void* const v2) {
    VEC2(dest)->x = VEC2(v1)->x + VEC2(v2)->x;
    VEC2(dest)->y = VEC2(v1)->y + VEC2(v2)->y;
}

static inline void gyVec2Sub(void* const dest, const void* const v1, const void* const v2) {
    VEC2(dest)->x = VEC2(v1)->x - VEC2(v2)->x;
    VEC2(dest)->y = VEC2(v1)->y - VEC2(v2)->y;
}

static inline void gyVec3Sub(void* const dest, const void* const v1, const void* const v2) {
    VEC3(dest)->x = VEC3(v1)->x - VEC3(v2)->x;
    VEC3(dest)->y = VEC3(v1)->y - VEC3(v2)->y;
    VEC3(dest)->z = VEC3(v1)->z - VEC3(v2)->z;
}

static inline void gyVec2SubFrom(void *const dest, const void *const val) {
    VEC2(dest)->x -= VEC2(val)->x; VEC2(dest)->y -= VEC2(val)->y;
}

static inline void gyVec2AddAccum(void* const dest, const void* const src) {
    VEC2(dest)->x += VEC2(src)->x; VEC2(dest)->y += VEC2(src)->y;
}

static inline void gyVec2Scale(void *const dst, const void *const src, const float scalar) {
    VEC2(dst)->x = VEC2(src)->x * scalar;
    VEC2(dst)->y = VEC2(src)->y * scalar;
}

static inline void gyVec2ScaleAccum(void* const src, const float scalar) {
    VEC2(src)->x *= scalar; VEC2(src)->y *= scalar;
}

static inline float gyVec2Dist(const void*const v1, const void*const v2) {
    GYVector2 dist;
    gyVec2Sub(&dist, v1, v2);
    dist.x *= dist.x;
    dist.y *= dist.y;
    return gySqrt(dist.x + dist.y);
}

static inline float gyVec3Dist(const void*const v1, const void*const v2) {
    GYVector3 dist;
    gyVec3Sub(&dist, v1, v2);
    dist.x *= dist.x;
    dist.y *= dist.y;
    dist.z *= dist.z;
    return gySqrt(dist.x + dist.y + dist.z);
}

static inline float gyVec2Magnitude(const void*const vec) {
    return gySqrt( (VEC2(vec)->x * VEC2(vec)->x) + (VEC2(vec)->y * VEC2(vec)->y) );
}

static inline float gyVec2MagAndNormalize(void*const v) {
    float mag = gySqrt( (VEC2(v)->x * VEC2(v)->x) + (VEC2(v)->y * VEC2(v)->y) );
    VEC2(v)->x /= mag;
    VEC2(v)->y /= mag;
    return mag;
}

static inline void gyVec2CompProdAccum(void* const dest, const void* const src) {
    VEC2(dest)->x *= VEC2(src)->x; VEC2(dest)->y *= VEC2(src)->y;
}

static inline void gyVec2AddScaled(void* const dst, const void* src, const float scalar) {
    VEC2(dst)->x += VEC2(src)->x * scalar;
    VEC2(dst)->y += VEC2(src)->y * scalar;
}


static inline float gyVec2DotProd(const void *const src1, const void *const src2) {
    return (VEC2(src1)->x * VEC2(src2)->x + VEC2(src1)->y * VEC2(src2)->y);
}

static inline void gyVec2Normalize(void *const v) {
    float mag = gySqrt( (VEC2(v)->x * VEC2(v)->x) + (VEC2(v)->y * VEC2(v)->y) );
    VEC2(v)->x /= mag;
    VEC2(v)->y /= mag;
}

static inline float gyVec2ScalarCrossProd(const void *const src1, const void *const src2) {
    GYVector2 perp;
    perp.x = VEC2(src1)->y;
    perp.y = -VEC2(src1)->x;
    return gyVec2DotProd(&perp, src2);
}

static inline void gyVec2VectorCrossProd(void *const dest, const void *const vec, const float scalar) {
    VEC2(dest)->x = VEC2(vec)->y * scalar;
    VEC2(dest)->y = -VEC2(vec)->x * scalar;
}

static inline void gyVec2RotOrigin(void *const dest, const void *const src, const float angle) {
    float psin, pcos;
    gySinCos(angle, &psin, &pcos);
    VEC2(dest)->x = VEC2(src)->x*pcos - VEC2(src)->x*psin;
    VEC2(dest)->y = VEC2(src)->y*psin + VEC2(src)->y*pcos;
}


//=========== VECTOR 3 ==============

static inline float gyVec3Magnitude(const void*const vec) {
    return gySqrt( (VEC3(vec)->x * VEC3(vec)->x) + (VEC3(vec)->y * VEC3(vec)->y) + (VEC3(vec)->z * VEC3(vec)->z) );
}


static inline float gyVec3MagAndNormalize(void*const v) {
    float mag = gySqrt( (VEC3(v)->x * VEC3(v)->x) + (VEC3(v)->y * VEC3(v)->y) + (VEC3(v)->z * VEC3(v)->z) );
    VEC3(v)->x /= mag;
    VEC3(v)->y /= mag;
    VEC3(v)->z /= mag;
    return mag;
}


static inline void gyVec3Add(void* const dest, const void* const v1, const void* const v2) {
    VEC3(dest)->x = VEC3(v1)->x + VEC3(v2)->x;
    VEC3(dest)->y = VEC3(v1)->y + VEC3(v2)->y;
    VEC3(dest)->z = VEC3(v1)->z + VEC3(v2)->z;
}

static inline void gyVec3Scale(void*const dest, const void*const source, const float scalar) {
    VEC3(dest)->x = VEC3(source)->x * scalar;
    VEC3(dest)->y = VEC3(source)->y * scalar;
    VEC3(dest)->z = VEC3(source)->z * scalar;
}

static inline void gyVec3AddAccum(void* const dest, const void* const src) {
    VEC3(dest)->x += VEC3(src)->x; VEC3(dest)->y += VEC3(src)->y; VEC3(dest)->z += VEC3(src)->z;
}

static inline void gyVec3CompProdAccum(void* const dest, const void* const src) {
    VEC3(dest)->x *= VEC3(src)->x; VEC3(dest)->y *= VEC3(src)->y; VEC3(dest)->z *= VEC3(src)->z;
}

static inline void gyVec3CrossProd(void* const dest, const void* const src1, const void* const src2) {
    VEC3(dest)->x = VEC3(src1)->y*VEC3(src2)->z - VEC3(src1)->z*VEC3(src2)->y;
    VEC3(dest)->y = VEC3(src1)->z*VEC3(src2)->x - VEC3(src1)->x*VEC3(src2)->z;
    VEC3(dest)->z = VEC3(src1)->x*VEC3(src2)->y - VEC3(src1)->y*VEC3(src2)->x;
}

static inline void gyVec3AddScaled(void*const dest, const void*const vec,
                             const float scalar) {
    VEC3(dest)->x += scalar * VEC3(vec)->x;
    VEC3(dest)->y += scalar * VEC3(vec)->y;
    VEC3(dest)->z += scalar * VEC3(vec)->z;
}

static inline void gyVec3ScaleAccum(void*const dest, const float scalar) {
    VEC3(dest)->x *= scalar;
    VEC3(dest)->y *= scalar;
    VEC3(dest)->z *= scalar;
}

static inline int gyVec3Compare(const void*const src1, const void *const src2) {
    return (VEC3(src1)->x == VEC3(src2)->x && VEC3(src1)->y == VEC3(src2)->y && VEC3(src1)->z == VEC3(src2)->z);
}

static inline float gyVec3DotProd(const void *const src1, const void *const src2) {
    return (VEC3(src1)->x*VEC3(src2)->x + VEC3(src1)->y*VEC3(src2)->y + VEC3(src1)->z*VEC3(src2)->z);
}

static inline void gyVec3Normalize(void *const v) {
    float mag = gySqrt( (VEC3(v)->x * VEC3(v)->x) + (VEC3(v)->y * VEC3(v)->y) + (VEC3(v)->z * VEC3(v)->z) );
    VEC3(v)->x /= mag;
    VEC3(v)->y /= mag;
    VEC3(v)->z /= mag;
}


static inline void gyVec4Normalize(void *const v) {
    float mag = gySqrt( (VEC4(v)->x * VEC4(v)->x) + (VEC4(v)->y * VEC4(v)->y) + (VEC4(v)->z * VEC4(v)->z) + (VEC4(v)->w * VEC4(v)->w) );
    VEC4(v)->x /= mag;
    VEC4(v)->y /= mag;
    VEC4(v)->z /= mag;
    VEC4(v)->w /= mag;
}

static inline int gyVec4Compare(const void*const src1, const void *const src2) {
    return (VEC4(src1)->x == VEC4(src2)->x && VEC4(src1)->y == VEC4(src2)->y && VEC4(src1)->z == VEC4(src2)->z && VEC4(src1)->w == VEC4(src2)->w);
}

static inline int gyMat4Compare(const GYMatrix4 m1, const GYMatrix4 m2) {
    return (gyVec4Compare(m1[0], m2[0]) && gyVec4Compare(m1[1], m2[1]) && gyVec4Compare(m1[2], m2[2]) && gyVec4Compare(m1[3], m2[3]));
}

/*

static inline float gyVec4Magnitude(GYVector4*const dest, GYVector4*const src) {

}

static inline void gyVec4Normalize(GYVector4*const dest, GYVector4*const src) {
    const __m128 s1 = MM_LOAD_ALIGN((float*)src);
//	const float mag = gyVec4Magnitude(src);
//	const __m128 m = _mm_load_ss(&mag);
//	const __m128 r = _mm_div_ps(s1, m);
//	MM_STORE_ALIGN((float*)dest, r);
}
*/
