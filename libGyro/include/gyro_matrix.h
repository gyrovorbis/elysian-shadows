#ifndef GYRO_MATRIX_H
#define GYRO_MATRIX_H

#include "gyro_platform.h"
#include "gyro_math.h"

#ifdef __cplusplus
extern "C" {
#endif

//Column-Major
typedef float GYMatrix4[4][4]
#if GYRO_PLATFORM == GYRO_PLATFORM_DC
__attribute__((aligned(32)))
#elif GYRO_PLATFORM == GYRO_PLATFORM_x86
__attribute__ ((aligned (16)));
#endif
;

typedef struct _GYVector4 {
    float x, y, z, w;
} GYVector4 
#if GYRO_PLATFORM == GYRO_PLATFORM_DC || GYRO_PLATFORM == GYRO_PLATFORM_x86
__attribute__((aligned(16)))
#endif
;

typedef struct _GYVector3 {
    float x, y, z;
} GYVector3;

typedef struct _GYVector2 {
    float x, y;
} GYVector2;

typedef struct _GYInstMat {
	GYVector2 _pos;
	float _posZ;
	GYVector2 _size;
	GYVector2 _scale;
	struct _GYInstMat *parent;
	float _orientZ;
} GYInstMat;

#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#   ifdef GYRO_MATRIX_ACCELERATION
#       include "dc/gyro_matrix_dc.inl"
#   endif
#else
#	include "soft/gyro_matrix_sse.inl"
//void gyMatLoad(GYMatrix4 *mat);
//void gyMatStore(GYMatrix4 *mat);
//void gyMatApply(GYMatrix4 mat);
//void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, const unsigned int count);
#endif

static inline void gyVec2Clear(GYVector2 *const vec) { vec->x = vec->y = 0.0f; }

static inline int gyVec2IsZero(const GYVector2* const vec) { return (vec->x == 0.0f && vec->y == 0.0f); }

static inline void gyVec2Add(GYVector2* const dest, const GYVector2* const v1, const GYVector2* const v2) {
	dest->x = v1->x + v2->x;
	dest->y = v1->y + v2->y;
}

static inline void gyVec2Sub(GYVector2* const dest, const GYVector2* const v1, const GYVector2* const v2) {
	dest->x = v1->x - v2->x;
	dest->y = v1->y - v2->y;
}

static inline void gyVec2SubFrom(GYVector2 *const dest, const GYVector2 *const val) {
	dest->x -= val->x; dest->y -= val->y;
}

static inline void gyVec2AddAccum(GYVector2* const dest, const GYVector2* const src) {
	dest->x += src->x; dest->y += src->y;
}

static inline void gyVec2Scale(GYVector2 *const dst, GYVector2 *const src, const float scalar) {
	dst->x = src->x * scalar;
	dst->y = src->y * scalar;
}

static inline void gyVec2ScaleAccum(GYVector2* const src, const float scalar) {
	src->x *= scalar; src->y *= scalar; 
}

static inline void gyVec3Add(GYVector3* const dest, const GYVector3* const v1, const GYVector3* const v2) {
	dest->x = v1->x + v2->x;
	dest->y = v1->y + v2->y;
	dest->z = v1->z + v2->z;
}

static inline void gyVec3Sub(GYVector3* const dest, const GYVector3* const v1, const GYVector3* const v2) {
	dest->x = v1->x - v2->x;
	dest->y = v1->y - v2->y;
	dest->z = v1->z - v2->z;
}

static inline void gyVec3AddAccum(GYVector3* const dest, const GYVector3* const src) {
	dest->x += src->x; dest->y += src->y; dest->z += src->z;
}

static inline void gyVec3CompProdAccum(GYVector3* const dest, const GYVector3* const src) {
	dest->x *= src->x; dest->y *= src->y; dest->z *= src->z;
}

static inline void gyVec2CompProdAccum(GYVector2* const dest, const GYVector2* const src) {
	dest->x *= src->x; dest->y *= src->y;
}

static inline float gyVec2DotProd(const GYVector2 *const src1, const GYVector2 *const src2) {
	return (src1->x*src2->x + src1->y*src2->y);
}

static inline void gyVec2Normalize(GYVector2 *const v) {
	float mag = gySqrt(v->x*v->x + v->y*v->y);
	v->x /= mag;
	v->y /= mag;
}

static inline float gyVec2ScalarCrossProd(const GYVector2 *const src1, const GYVector2 *const src2) {
	GYVector2 perp;
	perp.x = src1->y;
	perp.y = -src1->x;
	return gyVec2DotProd(&perp, src2);
}

static inline void gyVec2VectorCrossProd(GYVector2 *const dest, const GYVector2 *const vec, const float scalar) {
	dest->x = vec->y * scalar;
	dest->y = -vec->x * scalar;
}

static inline void gyVec2RotOrigin(GYVector2 *const dest, const GYVector2 *const src, const float angle) {
	float psin, pcos;
	gySinCos(angle, &psin, &pcos);
	dest->x = src->x*pcos - src->x*psin;
	dest->y = src->y*psin + src->y*pcos;
}


#ifdef __cplusplus
}
#endif

#endif // GYRO_MATH_H
