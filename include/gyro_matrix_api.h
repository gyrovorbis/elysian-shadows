#ifndef GYRO_MATRIX_H
#define GYRO_MATRIX_H

#include "gyro_platform.h"
#include "gyro_math_api.h"

#ifdef __cplusplus
extern "C" {
#endif

//Column-Major
typedef float GYMatrix4[4][4] GYRO_MATRIX4_ALIGN;

typedef struct GYVector4 {
    float x, y, z, w;
} GYVector4 GYRO_VECTOR4_ALIGN;

typedef struct GYVector3 {
    float x, y, z;
} GYVector3;

typedef struct GYVector2 {
    float x, y;
} GYVector2;

typedef struct GYInstMat {
    GYVector3   _pos;
    GYVector3   _size;
    GYVector3   _scale;
    GYVector3   _orient;
    float       _angle;
    struct _GYInstMat *parent;
} GYInstMat;

#ifdef GYRO_MATRIX_PLATFORM_INCLUDE
#	include GYRO_MATRIX_PLATFORM_INCLUDE
#else
void gyMatApply(GYMatrix4 mat);
void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, const unsigned int count);
void gyMatLoad(GYMatrix4 *mat);
void gyMatStore(GYMatrix4 *mat);

void gyMatMultMat(GYMatrix4* dMat, const GYMatrix4* sMat1, const GYMatrix4* sMat2);

void gyVec4Add(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2);
void gyVec4Sub(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2);
void gyVec4Mult(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2);
void gyVec4Div(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2);

void gyVec2ScaleAccum(GYVector2* const src, const float scalar);
void gyVec3Add(GYVector3* const dest, const GYVector3* const v1, const GYVector3* const v2);
void gyVec3Sub(GYVector3* const dest, const GYVector3* const v1, const GYVector3* const v2);
void gyVec3AddAccum(GYVector3* const dest, const GYVector3* const src);
void gyVec3CompProdAccum(GYVector3* const dest, const GYVector3* const src);
void gyVec3Normalize(GYVector3*const v);

void gyVec2Clear(GYVector2 *const vec);
int gyVec2IsZero(const GYVector2* const vec);
void gyVec2Add(GYVector2* const dest, const GYVector2* const v1, const GYVector2* const v2);
void gyVec2Sub(GYVector2* const dest, const GYVector2* const v1, const GYVector2* const v2);
void gyVec2SubFrom(GYVector2 *const dest, const GYVector2 *const val);
void gyVec2AddAccum(GYVector2* const dest, const GYVector2* const src);
void gyVec2Scale(GYVector2 *const dst, GYVector2 *const src, const float scalar);
void gyVec2CompProdAccum(GYVector2* const dest, const GYVector2* const src);
float gyVec2DotProd(const GYVector2 *const src1, const GYVector2 *const src2);
void gyVec2Normalize(GYVector2 *const v);
float gyVec2ScalarCrossProd(const GYVector2 *const src1, const GYVector2 *const src2);
void gyVec2VectorCrossProd(GYVector2 *const dest, const GYVector2 *const vec, const float scalar);
void gyVec2RotOrigin(GYVector2 *const dest, const GYVector2 *const src, const float angle);

#endif

#include "common/gyro_matrix_stack.h"


#ifdef __cplusplus
}
#endif

#endif // GYRO_MATH_H
