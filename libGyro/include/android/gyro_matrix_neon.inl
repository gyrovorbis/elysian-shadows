#include "../gyro_defines.h"
#include <string.h>
#include <assert.h>

extern GYMatrix4 _curMat;

inline static void brighten_neon(unsigned char* src, unsigned char* dst, int numPixels, int intensity) {
__asm volatile ( 
              "\t mov r4, #0\n"
              "\t vdup.8 d1, %3\n"
              "Lloop2:\n"
              "\t cmp r4, %2\n"
              "\t bge Lend2\n"
              "\t vld1.8 d0, [%0]!\n"
              "\t vqadd.s8 d0, d0, d1\n"
              "\t vst1.8 d0, [%1]!\n"
              "\t add r4, r4, #8\n"
              "\t b Lloop2\n"
              "Lend2:\n"
              : "=r"(src), "=r"(dst), "=r"(numPixels), "=r"(intensity)
              : "0"(src), "1"(dst), "2"(numPixels), "3"(intensity)
              : "cc", "r4", "d1", "d0");
}


#if 1
static inline void gyMatMultMat(GYMatrix4 res, const GYMatrix4 _curMat, const GYMatrix4 mat) {

        res[0][0] = _curMat[0][0]*mat[0][0] + _curMat[1][0]*mat[0][1]+ _curMat[2][0]*mat[0][2]+ _curMat[3][0]*mat[0][3];
        res[1][0] = _curMat[0][0]*mat[1][0] + _curMat[1][0]*mat[1][1]+ _curMat[2][0]*mat[1][2]+ _curMat[3][0]*mat[1][3];
        res[2][0] = _curMat[0][0]*mat[2][0] + _curMat[1][0]*mat[2][1]+ _curMat[2][0]*mat[2][2]+ _curMat[3][0]*mat[2][3];
        res[3][0] = _curMat[0][0]*mat[3][0] + _curMat[1][0]*mat[3][1]+ _curMat[2][0]*mat[3][2]+ _curMat[3][0]*mat[3][3];

        res[0][1] = _curMat[0][1]*mat[0][0] + _curMat[1][1]*mat[0][1]+ _curMat[2][1]*mat[0][2]+ _curMat[3][1]*mat[0][3];
        res[1][1] = _curMat[0][1]*mat[1][0] + _curMat[1][1]*mat[1][1]+ _curMat[2][1]*mat[1][2]+ _curMat[3][1]*mat[1][3];
        res[2][1] = _curMat[0][1]*mat[2][0] + _curMat[1][1]*mat[2][1]+ _curMat[2][1]*mat[2][2]+ _curMat[3][1]*mat[2][3];
        res[3][1] = _curMat[0][1]*mat[3][0] + _curMat[1][1]*mat[3][1]+ _curMat[2][1]*mat[3][2]+ _curMat[3][1]*mat[3][3];

        res[0][2] = _curMat[0][2]*mat[0][0] + _curMat[1][2]*mat[0][1]+ _curMat[2][2]*mat[0][2]+ _curMat[3][2]*mat[0][3];
        res[1][2] = _curMat[0][2]*mat[1][0] + _curMat[1][2]*mat[1][1]+ _curMat[2][2]*mat[1][2]+ _curMat[3][2]*mat[1][3];
        res[2][2] = _curMat[0][2]*mat[2][0] + _curMat[1][2]*mat[2][1]+ _curMat[2][2]*mat[2][2]+ _curMat[3][2]*mat[2][3];
        res[3][2] = _curMat[0][2]*mat[3][0] + _curMat[1][2]*mat[3][1]+ _curMat[2][2]*mat[3][2]+ _curMat[3][2]*mat[3][3];

        res[0][3] = _curMat[0][3]*mat[0][0] + _curMat[1][3]*mat[0][1]+ _curMat[2][3]*mat[0][2]+ _curMat[3][3]*mat[0][3];
        res[1][3] = _curMat[0][3]*mat[1][0] + _curMat[1][3]*mat[1][1]+ _curMat[2][3]*mat[1][2]+ _curMat[3][3]*mat[1][3];
        res[2][3] = _curMat[0][3]*mat[2][0] + _curMat[1][3]*mat[2][1]+ _curMat[2][3]*mat[2][2]+ _curMat[3][3]*mat[2][3];
        res[3][3] = _curMat[0][3]*mat[0][0] + _curMat[1][3]*mat[3][1]+ _curMat[2][3]*mat[3][2]+ _curMat[3][3]*mat[3][3];

}

#else 


static inline void gyMatMultMat(GYMatrix4 res, const GYMatrix4 _curMat, const GYMatrix4 mat) {

#if 1
  __asm__ __volatile__ 
    ("vldmia   %[A],  { q4-q7 }             \n\t" 
     "vldmia   %[B],  { q8-q11}             \n\t" 
     "vmul.f32   q0,   q8,    d8[0]         \n\t" 
     "vmul.f32   q1,   q8,   d10[0]         \n\t" 
     "vmul.f32   q2,   q8,   d12[0]         \n\t" 
     "vmul.f32   q3,   q8,   d14[0]         \n\t" 
     "vmla.f32   q0,   q9,    d8[1]         \n\t" 
     "vmla.f32   q1,   q9,   d10[1]         \n\t" 
     "vmla.f32   q2,   q9,   d12[1]         \n\t" 
     "vmla.f32   q3,   q9,   d14[1]         \n\t" 
     "vmla.f32   q0,  q10,    d9[0]         \n\t" 
     "vmla.f32   q1,  q10,   d11[0]         \n\t" 
     "vmla.f32   q2,  q10,   d13[0]         \n\t" 
     "vmla.f32   q3,  q10,   d15[0]         \n\t" 
     "vmla.f32   q0,  q11,    d9[1]         \n\t" 
     "vmla.f32   q1,  q11,   d11[1]         \n\t" 
     "vmla.f32   q2,  q11,   d13[1]         \n\t" 
     "vmla.f32   q3,  q11,   d15[1]         \n\t" 
     "vstmia   %[R],  { q0-q3 }             \n\t" 
     :[R]"+r" (res), [A]"+r" (_curMat), [B]"+r" (mat) 
     ::"memory","d0","d1","d2","d3","d4","d5","d6","d7", 
      "d8","d9","d10","d11","d12","d13","d14","d15", 
      "d16","d17","d18","d19","d20","d21","d22","d23"); 
      
}
#else
  __asm volatile (
    // result = first column of B x first row of A
    "vmul.f32 %q0, %q4, %e8[0]\n\t" // %e
    "vmul.f32 %q1, %q4, %e9[0]\n\t"
    "vmul.f32 %q2, %q4, %e10[0]\n\t"
    "vmul.f32 %q3, %q4, %e11[0]\n\t"
    // result += second column of B x second row of A
    "vmla.f32 %q0, %q5, %e8[1]\n\t"
    "vmla.f32 %q1, %q5, %e9[1]\n\t"
    "vmla.f32 %q2, %q5, %e10[1]\n\t"
    "vmla.f32 %q3, %q5, %e11[1]\n\t"
    // result += third column of B x third row of A
    "vmla.f32 %q0, %q6, %f8[0]\n\t"
    "vmla.f32 %q1, %q6, %f9[0]\n\t"
    "vmla.f32 %q2, %q6, %f10[0]\n\t"
    "vmla.f32 %q3, %q6, %f11[0]\n\t"
    // result += last column of B x last row of A
    "vmla.f32 %q0, %q7, %f8[1]\n\t"
    "vmla.f32 %q1, %q7, %f9[1]\n\t"
    "vmla.f32 %q2, %q7, %f10[1]\n\t"
    "vmla.f32 %q3, %q7, %f11[1]\n\t"
    : "=w" (result[0]), "=w" (result[1]), "=w" (result[2]), "=w" (result[3])
    :  "w" (b[0]), "w" (b[1]), "w" (b[2]), "w" (b[3]),
       "w" (a[0]), "w" (a[1]), "w" (a[2]), "w" (a[3])
    :   
    );  

#endif

#endif 

static inline void gyMatApply(const GYMatrix4 mat) {
    GYMatrix4 result;
    gyMatMultMat(result, _curMat, mat);
    memcpy(_curMat, result, sizeof(GYMatrix4));
}


static inline void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, const unsigned int count) {

    float res[4];
    unsigned int i;
    for(i = 0; i < count; ++i) {

        res[0] = _curMat[0][0]*src[i].x + _curMat[1][0]*src[i].y + _curMat[2][0]*src[i].z + _curMat[3][0]*src[i].w;
        res[1] = _curMat[0][1]*src[i].x + _curMat[1][1]*src[i].y + _curMat[2][1]*src[i].z + _curMat[3][1]*src[i].w;
        res[2] = _curMat[0][2]*src[i].x + _curMat[1][2]*src[i].y + _curMat[2][2]*src[i].z + _curMat[3][2]*src[i].w;
        res[3] = _curMat[0][3]*src[i].x + _curMat[1][3]*src[i].y + _curMat[2][3]*src[i].z + _curMat[3][3]*src[i].w;

        memcpy(dest+i, res, sizeof(float)*4);
    }

}

static inline void gyMatLoad(GYMatrix4 *mat) {
    memcpy(_curMat, mat, sizeof(GYMatrix4));
}

static inline void gyMatStore(GYMatrix4 *mat) {
    memcpy(mat, _curMat, sizeof(GYMatrix4));
}

static inline float gyVec2MagAndNormalize(GYVector2*const v) {
    float mag = gySqrt(v->x*v->x + v->y*v->y);
    v->x /= mag;
    v->y /= mag;
    return mag;
}


static inline float gyVec3MagAndNormalize(GYVector3*const v) {
    float mag = gySqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= mag;
    v->y /= mag;
    v->z /= mag;
    return mag;
}

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

static inline void gyVec2Scale(GYVector2 *const dst, const GYVector2 *const src, const float scalar)
{
    dst->x = src->x * scalar;
    dst->y = src->y * scalar;
}
static inline float gyVec3DotProd(const GYVector3 *const src1, const GYVector3 *const src2) {
    return (src1->x*src2->x + src1->y*src2->y + src1->z*src2->z);
}


static inline void gyVec2ScaleAccum(GYVector2* const src, const float scalar) {
    src->x *= scalar; src->y *= scalar;
}

static inline float gyVec2Magnitude(const GYVector2*const vec) {
    return gySqrt(vec->x*vec->x + vec->y*vec->y);
}

static inline float gyVec3Magnitude(const GYVector3*const vec) {
    return gySqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
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

static inline void gyVec3Scale(GYVector3*const dest, GYVector3*const source, const float scalar)
{
    dest->x = source->x * scalar;
    dest->y = source->y * scalar;
    dest->z = source->z * scalar;
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

static inline void gyVec3CrossProd(GYVector3* const dest, const GYVector3* const src1, const GYVector3* const src2) {
    dest->x = src1->y*src2->z - src1->z*src2->y;
    dest->y = src1->z*src2->x - src1->x*src2->z;
    dest->z = src1->x*src2->y - src1->y*src2->x;
}

static inline void gyVec3AddScaled(GYVector3*const dest, const GYVector3*const vec, const float scalar)
{
    dest->x += scalar * vec->x;
    dest->y += scalar * vec->y;
    dest->z += scalar * vec->z;
}

static inline void gyVec3ScaleAccum(GYVector3*const dest, const float scalar)
{
    dest->x *= scalar;
    dest->y *= scalar;
    dest->z *= scalar;
}

static inline float gyVec2DotProd(const GYVector2 *const src1, const GYVector2 *const src2) {
    return (src1->x*src2->x + src1->y*src2->y);
}

static inline void gyVec2Normalize(GYVector2 *const v) {
    float mag = gySqrt(v->x*v->x + v->y*v->y);
    v->x /= mag;
    v->y /= mag;
}

static inline void gyVec4Normalize(GYVector4 *const v) {
    float mag = gySqrt(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
    v->x /= mag;
    v->y /= mag;
    v->z /= mag;
    v->w /= mag;
}
    
static inline void gyVec3Normalize(GYVector3 *const v) {
    float mag = gySqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= mag;
    v->y /= mag;
    v->z /= mag;
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

    
static inline void gyVec4Add(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2) {
    dest->x = v1->x + v2->x;
    dest->y = v1->y + v2->y;
    dest->z = v1->z + v2->z;
    dest->w = v1->w + v2->w;
}
    
static inline void gyVec4Sub(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2) {
    dest->x = v1->x - v2->x;
    dest->y = v1->y - v2->y;
    dest->z = v1->z - v2->z;
    dest->w = v1->w - v2->w;
}
    
static inline void gyVec4Mult(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2) {
    dest->x = v1->x * v2->x;
    dest->y = v1->y * v2->y;
    dest->z = v1->z * v2->z;
    dest->w = v1->w * v2->w;
}
    
static inline void gyVec4Div(GYVector4* const dest, const GYVector4* const v1, const GYVector4* const v2) {
    dest->x = v1->x / v2->x;
    dest->y = v1->y / v2->y;
    dest->z = v1->z / v2->z;
    dest->w = v1->w / v2->w;
}
    
static inline void gyVec4Scale(GYVector4* const dest, const GYVector4* const v1, const float s) {
    dest->x = v1->x * s;
    dest->y = v1->y * s;
    dest->z = v1->z * s;
    dest->w = v1->w * s;
}
