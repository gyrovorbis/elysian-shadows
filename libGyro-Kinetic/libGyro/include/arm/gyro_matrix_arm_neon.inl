/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include "../gyro_defines.h"
#include <string.h>
#include <assert.h>

extern GYMatrix4 _curMat;

#define VEC2(a) ((GYVector2*)a)
#define VEC3(a) ((GYVector3*)a)
#define VEC4(a) ((GYVector4*)a)

#if 0
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
#endif


#if 0
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

    #if 0






    .macro mul_col_s16 res_d, col_d
    vmull.s16   q12, d16, \col_d[0]         @ multiply col element 0 by matrix col 0
    vmlal.s16   q12, d17, \col_d[1]         @ multiply-acc col element 1 by matrix col 1
    vmlal.s16   q12, d18, \col_d[2]         @ multiply-acc col element 2 by matrix col 2
    vmlal.s16   q12, d19, \col_d[3]         @ multiply-acc col element 3 by matrix col 3
    vqrshrn.s32 \res_d, q12, #14            @ shift right and narrow accumulator into
                                            @  Q1.14 fixed point format, with saturation
    .endm

@
@ matrix_mul_fixed:
@ Calculate 4x4 (matrix 0) * (matrix 1) and store to result 4x4 matrix.
@  matrix 0, matrix 1 and result pointers can be the same,
@  ie. my_matrix = my_matrix * my_matrix is possible
@
@ r0 = pointer to 4x4 result matrix, Q1.14 fixed point, column major order
@ r1 = pointer to 4x4 matrix 0, Q1.14 fixed point, column major order
@ r2 = pointer to 4x4 matrix 1, Q1.14 fixed point, column major order
@

    .global matrix_mul_fixed
matrix_mul_fixed:
    vld1.16     {d16-d19}, [r1]             @ load sixteen elements of matrix 0
    vld1.16     {d0-d3}, [r2]               @ load sixteen elements of matrix 1

    mul_col_s16 d4, d0                      @ matrix 0 * matrix 1 col 0
    mul_col_s16 d5, d1                      @ matrix 0 * matrix 1 col 1
    mul_col_s16 d6, d2                      @ matrix 0 * matrix 1 col 2
    mul_col_s16 d7, d3                      @ matrix 0 * matrix 1 col 3

    vst1.16     {d4-d7}, [r0]               @ store sixteen elements of result

    mov         pc, lr                      @ return to caller





      #else

  __asm__ __volatile__  
    (

    "vld1.32     {d16-d19}, [%[A]]!   \n"//         @ load first eight elements of matrix 0
    "vld1.32     {d20-d23}, [%[A]]!   \n"      //   @ load second eight elements of matrix 0
    "vld1.32     {d0-d3}, [%[B]]!       \n"    //   @ load first eight elements of matrix 1
    "vld1.32     {d4-d7}, [%[B]]!    \n"        //  @ load second eight elements of matrix 1


    //PIPELINE STALLS -- SLOWER
    //"vldmia   %[A],  { q8-q11 }             \n\t" 
    //"vldmia   %[B],  { q0-q3 }             \n\t" 

    "vmul.f32    q12, q8, d0[0] \n"            // @ rslt col0  = (mat0 col0) * (mat1 col0 elt0)
    "vmul.f32    q13, q8, d2[0]    \n"         // @ rslt col1  = (mat0 col0) * (mat1 col1 elt0)
    "vmul.f32    q14, q8, d4[0]    \n"         // @ rslt col2  = (mat0 col0) * (mat1 col2 elt0)
    "vmul.f32    q15, q8, d6[0]    \n"         // @ rslt col3  = (mat0 col0) * (mat1 col3 elt0)

    "vmla.f32    q12, q9, d0[1]    \n"         // @ rslt col0 += (mat0 col1) * (mat1 col0 elt1)
    "vmla.f32    q13, q9, d2[1]      \n"       // @ rslt col1 += (mat0 col1) * (mat1 col1 elt1)
    "vmla.f32    q14, q9, d4[1]     \n"        // @ rslt col2 += (mat0 col1) * (mat1 col2 elt1)
    "vmla.f32    q15, q9, d6[1]     \n"        // @ rslt col3 += (mat0 col1) * (mat1 col3 elt1)

    "vmla.f32    q12, q10, d1[0]    \n"        // @ rslt col0 += (mat0 col2) * (mat1 col0 elt2)
    "vmla.f32    q13, q10, d3[0]    \n"        // @ rslt col1 += (mat0 col2) * (mat1 col1 elt2)
    "vmla.f32    q14, q10, d5[0]    \n"        // @ rslt col2 += (mat0 col2) * (mat1 col2 elt2)
    "vmla.f32    q15, q10, d7[0]     \n"       // @ rslt col3 += (mat0 col2) * (mat1 col2 elt2)

    "vmla.f32    q12, q11, d1[1]     \n"       // @ rslt col0 += (mat0 col3) * (mat1 col0 elt3)
    "vmla.f32    q13, q11, d3[1]     \n"      //  @ rslt col1 += (mat0 col3) * (mat1 col1 elt3)
    "vmla.f32    q14, q11, d5[1]      \n"     //  @ rslt col2 += (mat0 col3) * (mat1 col2 elt3)
    "vmla.f32    q15, q11, d7[1]      \n"     //  @ rslt col3 += (mat0 col3) * (mat1 col3 elt3)

    //"vst1.32     {d24-d27}, [%[R]]!    \n"      //  @ store first eight elements of result
    //"vst1.32     {d28-d31}, [%[R]]!    \n"      //  @ store second eight elements of result

    "vstmia   %[R],  { q12-q15 }             \n\t" 


    : [R]"+r" (res)
     : [A]"r" (_curMat), [B]"r" (mat) 
     : "memory","d0","d1","d2","d3","d4","d5","d6","d7", 
      //"d8","d9","d10","d11","d12","d13","d14","d15", 
      "d16","d17","d18","d19","d20","d21","d22","d23",
      "d24", "d25", "d26", "d27", "d28", "d29", "d30", "d31"); 

    
#endif
      
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
#if 1
  //  GYMatrix4 result;
    gyMatMultMat(_curMat, _curMat, mat);
   // memcpy(_curMat, result, sizeof(GYMatrix4));
#else

 __asm__ __volatile__ 
    (//"vld1.32     {d16-d19}, [%[A]]!   \n"//         @ load first eight elements of matrix 0
     //"vld1.32     {d20-d23}, [%[A]]!   \n"      //   @ load second eight elements of matrix 0
    //"vld1.32     {d0-d3}, [%[B]]!       \n"    //   @ load first eight elements of matrix 1
    //"vld1.32     {d4-d7}, [%[B]]!    \n"        //  @ load second eight elements of matrix 1

    //"vldmia   %[A],  { q8-q11 }             \n\t" 
    //"vldmia   %[B],  { q0-q3 }             \n\t" 



    "vmul.f32    q12, q8, d0[0] \n"            // @ rslt col0  = (mat0 col0) * (mat1 col0 elt0)
    "vmul.f32    q13, q8, d2[0]    \n"         // @ rslt col1  = (mat0 col0) * (mat1 col1 elt0)
    "vmul.f32    q14, q8, d4[0]    \n"         // @ rslt col2  = (mat0 col0) * (mat1 col2 elt0)
    "vmul.f32    q15, q8, d6[0]    \n"         // @ rslt col3  = (mat0 col0) * (mat1 col3 elt0)

    "vmla.f32    q12, q9, d0[1]    \n"         // @ rslt col0 += (mat0 col1) * (mat1 col0 elt1)
    "vmla.f32    q13, q9, d2[1]      \n"       // @ rslt col1 += (mat0 col1) * (mat1 col1 elt1)
    "vmla.f32    q14, q9, d4[1]     \n"        // @ rslt col2 += (mat0 col1) * (mat1 col2 elt1)
    "vmla.f32    q15, q9, d6[1]     \n"        // @ rslt col3 += (mat0 col1) * (mat1 col3 elt1)

    "vmla.f32    q12, q10, d1[0]    \n"        // @ rslt col0 += (mat0 col2) * (mat1 col0 elt2)
    "vmla.f32    q13, q10, d3[0]    \n"        // @ rslt col1 += (mat0 col2) * (mat1 col1 elt2)
    "vmla.f32    q14, q10, d5[0]    \n"        // @ rslt col2 += (mat0 col2) * (mat1 col2 elt2)
    "vmla.f32    q15, q10, d7[0]     \n"       // @ rslt col3 += (mat0 col2) * (mat1 col2 elt2)

    "vmla.f32    q12, q11, d1[1]     \n"       // @ rslt col0 += (mat0 col3) * (mat1 col0 elt3)
    "vmla.f32    q13, q11, d3[1]     \n"      //  @ rslt col1 += (mat0 col3) * (mat1 col1 elt3)
    "vmla.f32    q14, q11, d5[1]      \n"     //  @ rslt col2 += (mat0 col3) * (mat1 col2 elt3)
    "vmla.f32    q15, q11, d7[1]      \n"     //  @ rslt col3 += (mat0 col3) * (mat1 col3 elt3)

    //"vst1.32     {d24-d27}, [%[R]]!    \n"      //  @ store first eight elements of result
    //"vst1.32     {d28-d31}, [%[R]]!    \n"      //  @ store second eight elements of result

    "vstmia   %[R],  { q12-q15 }             \n\t" 


    : [R]"+r" (_curMat)
     : [A]"r" (mat) 
     :"memory","d0","d1","d2","d3","d4","d5","d6","d7", 
      //"d8","d9","d10","d11","d12","d13","d14","d15", 
      "d16","d17","d18","d19","d20","d21","d22","d23",
      "d24", "d25", "d26", "d27", "d28", "d29", "d30", "d31"); 

#endif

}

static inline int gyVec4Compare(const void*const src1, const void *const src2) {
    return (VEC4(src1)->x == VEC4(src2)->x && VEC4(src1)->y == VEC4(src2)->y && VEC4(src1)->z == VEC4(src2)->z && VEC4(src1)->w == VEC4(src2)->w);
}

static inline int gyMat4Compare(const GYMatrix4 m1, const GYMatrix4 m2) {
    return (gyVec4Compare(m1[0], m2[0]) && gyVec4Compare(m1[1], m2[1]) && gyVec4Compare(m1[2], m2[2]) && gyVec4Compare(m1[3], m2[3]));
}

static inline int gyVec3Compare(const void*const src1, const void *const src2) {
    return (VEC3(src1)->x == VEC3(src2)->x && VEC3(src1)->y == VEC3(src2)->y && VEC3(src1)->z == VEC3(src2)->z);
}


static inline void gyMatMultVec(const GYVector4 *src, GYVector4 * dest, unsigned int count) {
#if 1
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


  __asm__ __volatile__  
    (

    "vld1.32     {d0-d3}, [%[A]]!       \n"    //   @ load first eight elements of matrix 1
    "vld1.32     {d4-d7}, [%[A]]!    \n"        //  @ load second eight elements of matrix 1
    "mov r3, %[R]\n"


    //PIPELINE STALLS -- SLOWER
    //"vldmia   %[A],  { q8-q11 }             \n\t" 
    //"vldmia   %[B],  { q0-q3 }             \n\t" 
    ".loop:              \n"

    "vld1.32     {d16-d17}, [%[B]]! \n"



    //"sub         %[C], %[C], #1 \n"
    "vmul.f32    q12, q8, d0[0] \n"            // @ rslt col0  = (mat0 col0) * (mat1 col0 elt0)
    "vmla.f32    q12, q8, d2[0]    \n"         // @ rslt col1  = (mat0 col0) * (mat1 col1 elt0)
    "vmla.f32    q12, q8, d4[0]    \n"         // @ rslt col2  = (mat0 col0) * (mat1 col2 elt0)
    "vmla.f32    q12, q8, d6[0]    \n"         // @ rslt col3  = (mat0 col0) * (mat1 col3 elt0)

    //"vst1.32     {d24-d27}, [%[R]]!    \n"      //  @ store first eight elements of result
    //"vst1.32     {d28-d31}, [%[R]]!    \n"      //  @ store second eight elements of result

    //"vstmia   %[R],  { q12-q15 }             \n\t" 



    "vst1.32     {d24-d25}, [ r3@16 ]!    \n"      //  @ store first eight elements of result
    //"vstmia   %[R],  { q12 }             \n\t" 
    //"add %[R], %[R], #16 \n"
    //"cmp          %[C], #0\n"
    
    //"bgt        loop        \n"
    "subs %[C], %[C], $1 \n"
    "bne .loop \n"
    //dec count
    //jump not equal to 0 to loop
    //write if there is a delayed branch slot



    :[R]"+r" (dest), [B]"+r" (src),  [C]"+r"(count)
     : [A]"r" (_curMat)
     : "memory","d0","d1","d2","d3","d4","d5","d6","d7", "r3"
      //"d8","d9","d10","d11","d12","d13","d14","d15", 
      "d16","d17",//"d18","d19","d20","d21","d22","d23",
      "d24", "d25");//, "d26", "d27", "d28", "d29", "d30", "d31"); 

#endif
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

static inline void gyVec3Sub(void* const dest, const void* const v1, const void* const v2) {
    VEC3(dest)->x = VEC3(v1)->x - VEC3(v2)->x;
    VEC3(dest)->y = VEC3(v1)->y - VEC3(v2)->y;
    VEC3(dest)->z = VEC3(v1)->z - VEC3(v2)->z;
}


static inline float gyVec3Dist(const void*const v1, const void*const v2) {
    GYVector3 dist;
    gyVec3Sub(&dist, v1, v2);
    dist.x *= dist.x;
    dist.y *= dist.y;
    dist.z *= dist.z;
    return gySqrt(dist.x + dist.y + dist.z);
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

#if 0
static inline void gyVec3Sub(GYVector3* const dest, const GYVector3* const v1, const GYVector3* const v2) {
    dest->x = v1->x - v2->x;
    dest->y = v1->y - v2->y;
    dest->z = v1->z - v2->z;
}
#endif

static inline void gyVec3Scale(GYVector3*const dest, GYVector3*const source, const float scalar)
{

    //VDUP - duplicate scalar to all vector
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
#if 1
    dest->x = v1->x + v2->x;
    dest->y = v1->y + v2->y;
    dest->z = v1->z + v2->z;
    dest->w = v1->w + v2->w;
#else 
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
     :[R]"+r" (dest), [A]"+r" (v1), [B]"+r" (v2) 
     ::"memory","d0","d1","d2","d3","d4","d5","d6","d7", 
      "d8","d9","d10","d11","d12","d13","d14","d15", 
      "d16","d17","d18","d19","d20","d21","d22","d23"); 
      
}

#endif
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
