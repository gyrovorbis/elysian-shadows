/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_MATRIX_H
#define GYRO_MATRIX_H

#include "gyro_platform.h"
#include "gyro_math_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GYRO_MATRIX4_ALIGN
#	define GYRO_MATRIX4_ALIGN               __attribute__ ((aligned (16)))
#endif

#ifndef GYRO_VECTOR4_ALIGN
#	define GYRO_VECTOR4_ALIGN               __attribute__ ((aligned (16)))
#endif

// Column-Major
typedef float GYMatrix4[4][4] GYRO_ALIGN(GYRO_MATRIX4_ALIGNMENT);

typedef struct GYVector2 { float x, y; } GYVector2;

typedef struct GYVector3 { float x, y, z; } GYVector3;

typedef struct GYVector4 {
  float x, y, z, w;
} GYVector4 GYRO_ALIGN(GYRO_VECTOR4_ALIGNMENT);

typedef struct GYQuaternion {
    float w, x, y, z;
} GYQuaternion GYRO_ALIGN(GYRO_QUATERNION_ALIGNMENT);

typedef union {
  GYVector4 vec4;
  GYVector3 vec3;
  GYVector2 vec2;
} GYVectorPunned;

typedef enum GY_INST_MAT_FLIP {
  GY_INST_MAT_FLIP_NONE = 0x0,
  GY_INST_MAT_FLIP_X_AXIS = 0x1,
  GY_INST_MAT_FLIP_Y_AXIS = 0x2,
  GY_INST_MAT_FLIP_Z_AXIS = 0x4
} GY_INST_MAT_FLIP;

typedef struct GYInstMat {
  GYVector3 _pos;
  GYVector3 _size;
  GYVector3 _scale;
  GYVector3 _orient;
  float _angle;
  unsigned _flip;
} GYInstMat;





#ifdef GYRO_MATRIX_PLATFORM_INCLUDE
#include GYRO_MATRIX_PLATFORM_INCLUDE
#else
void gyMatApply(GYMatrix4 mat);
void gyMatMultVec(const GYVector4* const src, GYVector4* const dest,
                  const unsigned int count);
void gyMatLoad(GYMatrix4* mat);
void gyMatStore(GYMatrix4* mat);

void gyMatMultMat(GYMatrix4* dMat, const GYMatrix4* sMat1,
                  const GYMatrix4* sMat2);

void gyMatMultQuaternion(GYQuaternion* dstQuat, const GYMatrix4* srcMat, const GYQuaternion* srcQuat);

void gyVec4Add(GYVector4* const dest, const GYVector4* const v1,
               const GYVector4* const v2);
void gyVec4Sub(GYVector4* const dest, const GYVector4* const v1,
               const GYVector4* const v2);
void gyVec4Mult(GYVector4* const dest, const GYVector4* const v1,
                const GYVector4* const v2);
void gyVec4Div(GYVector4* const dest, const GYVector4* const v1,
               const GYVector4* const v2);

void gyVec2ScaleAccum(GYVector2* const src, const float scalar);
void gyVec3Add(GYVector3* const dest, const GYVector3* const v1,
               const GYVector3* const v2);
void gyVec3Sub(GYVector3* const dest, const GYVector3* const v1,
               const GYVector3* const v2);
void gyVec3AddAccum(GYVector3* const dest, const GYVector3* const src);
void gyVec3CompProdAccum(GYVector3* const dest, const GYVector3* const src);
void gyVec3Normalize(GYVector3* const v);

void gyVec2Clear(GYVector2* const vec);
int gyVec2IsZero(const GYVector2* const vec);
void gyVec2Add(GYVector2* const dest, const GYVector2* const v1,
               const GYVector2* const v2);
void gyVec2Sub(GYVector2* const dest, const GYVector2* const v1,
               const GYVector2* const v2);
void gyVec2SubFrom(GYVector2* const dest, const GYVector2* const val);
void gyVec2AddAccum(GYVector2* const dest, const GYVector2* const src);
void gyVec2Scale(GYVector2* const dst, GYVector2* const src,
                 const float scalar);
void gyVec2CompProdAccum(GYVector2* const dest, const GYVector2* const src);
float gyVec2DotProd(const GYVector2* const src1, const GYVector2* const src2);
void gyVec2Normalize(GYVector2* const v);
float gyVec2ScalarCrossProd(const GYVector2* const src1,
                            const GYVector2* const src2);
void gyVec2VectorCrossProd(GYVector2* const dest, const GYVector2* const vec,
                           const float scalar);
void gyVec2RotOrigin(GYVector2* const dest, const GYVector2* const src,
                     const float angle);

#endif

#if 0
const GYQuaternion* gyQuaternionIdentity(void);
void                gyQuaternionFromAngleAxis(GYQuaternion* dst, const GYVector3* vec, float angle);
void                gyQuaternionFromMatrix4(GYQuaternion* dst, const GYMatrix4* mat);
void                gyQuaternionToAngleAxis(const GYQuaternion* src, GYVector3* axis, float* angle);
float               gyQuaternionDotProduct(const GYQuaternion* src1, const GYQuaternion* src2);
void                gyQuaternionFromEulerAngles(GYQuaternion* dst, float x, float y, float z)
void                gyQuaternionToEulerAngles(const GYQuaternion* src, float* x, float* y, float* z);
void                gyQuaternionReorientDirection(GYQuaternion* dst, const GYVector3* fromDir, const GYVector3* toDir);
void                gyQuaternionInverse(GYQuaternion* dst, const GYQuaternion* src);
void                gyQuaternionLerp(GYQuaternion* dst, const GYQuaternion* start, const GYQuaternion* stop, float elapsed);
void                gyQuaternionLookRotation(GYQuaternion* dst, const GYVector3* forward, const GYVector3* upwards)
void                gyQuaternionNormalize(GYQuaternion* dst, const GYQuaternion* src);
void                gyQuaternionRotateTowards(GYQuaternion* dst, const GYQuaternion* from, const GYQuaternion* to, float maxDegreesDelta);
void                gyQuaternionSlerp(GYQuaternion* dst, const GYQuaternion* sfart, const GYQuaternion* stop, float elapsed);
void                gyQuaternionMult(GYQuaternion* dst, const GYQuaternion* src1, const GYQuaternion* src2);
#endif
inline static const GYQuaternion* gyQuaternionIdentity(void) {
    static const GYQuaternion identity = {
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return &identity;
}

inline static void gyQuaternionFromAngleAxis(GYQuaternion* dst, const GYVector3* vec, float angle) {
    const float halfAngle = angle / 2.0f;
    float sinAngle;
    gySinCos(halfAngle, &sinAngle, &dst->w);
    gyVec3Scale(dst, vec, sinAngle);
}

inline static void gyQuaternionFromMatrix4(GYQuaternion* dst, const GYMatrix4 mat) {
    dst->w = sqrt(1.0f + mat[0][0] + mat[1][1] + mat[2][2]) / 2.0f;
    dst->x = (mat[2][1] - mat[1][2]) / (4.0f * dst->w);
    dst->y = (mat[0][2] - mat[2][0]) / (4.0f * dst->w);
    dst->z = (mat[1][0] - mat[0][1]) / (4.0f * dst->w);
}

inline static void gyQuaternionToAngleAxis(const GYQuaternion* src, GYVector3* axis, float* angle) {
    float mag = sqrt(1.0f - src->w*src->w);
    gyVec3Scale(axis, src, 1.0f/mag);
    *angle = 2.0f * acos(src->w);
}

inline static float gyQuaternionDotProduct(const GYQuaternion* src1, const GYQuaternion* src2) {
    return gyVec4DotProd(src1, src2);
}
#if 0
void gyQuaternionCrossProduct(GYQuaternion* dst, const GYQuaternion* src1, const GYQuaternion* src2) {
    gyVec3CrossProd(dst, &src1->x, &src2->x);
    dst->w =
}
#endif

inline static void gyQuaternionFromEulerAngles(GYQuaternion* dst, float x, float y, float z) {
    float cy, sy, cp, sp, cr, sr;
    gySinCos(z * 0.5f, &sy, &cy);
    gySinCos(y * 0.5f, &sp, &cp);
    gySinCos(x * 0.5f, &sr, &cr);
    dst->w = cy * cp * cr + sy * sp * sr;
    dst->x = cy * cp * sr - sy * sp * cr;
    dst->y = sy * cp * sr + cy * sp * cr;
    dst->z = sy * cp * cr - cy * sp * sr;
}

inline static void gyQuaternionToEulerAngles(const GYQuaternion* src, float* x, float* y, float* z) {
    // roll (x-axis rotation)
        float sinr_cosp = +2.0 * (src->w * src->x + src->y * src->z);
        float cosr_cosp = +1.0 - 2.0 * (src->x * src->x + src->y * src->y);
        *x = atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        float sinp = 2.0 * (src->w * src->y - src->z * src->x);
        if (fabs(sinp) >= 1)
            *y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
        else
           *y = asin(sinp);

        // yaw (z-axis rotation)
        float siny_cosp = +2.0 * (src->w * src->z + src->x * src->y);
        float cosy_cosp = +1.0 - 2.0 * (src->y * src->y + src->z * src->z);
        *z = atan2(siny_cosp, cosy_cosp);
}

inline static void gyQuaternionReorientDirection(GYQuaternion* dst, const GYVector3* fromDir, const GYVector3* toDir) {
    gyVec3CrossProd(&dst->x, fromDir, toDir);
    dst->w = sqrt( pow(gyVec3Magnitude(fromDir), 2.0f) * pow(gyVec3Magnitude(toDir), 2.0f)) + gyVec3DotProd(fromDir, toDir);
}

inline static void gyQuaternionConjugate(GYQuaternion* dst, const GYQuaternion* src) {
    dst->x = src->x;
    dst->y = -src->y;
    dst->z = -src->z;
    dst->w = -src->w;
}

inline static float gyQuaternionSquaredMagnitude(const GYQuaternion* q) {
    return q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z;
}

inline static void gyQuaternionInverse(GYQuaternion* dst, const GYQuaternion* src) {
    gyQuaternionConjugate(dst, src);
    gyVec4ScaleAccum(dst, gyQuaternionSquaredMagnitude(dst));
}


inline static void                gyQuaternionLerp(GYQuaternion* dst, const GYQuaternion* start, const GYQuaternion* stop, float elapsed) {
//p′=p1+t(p2−p1)
    /*
    Compute the difference between p1 and p2.
    Take the fractional part of that difference.
    Adjust the original value by the fractional difference between the two points.*/
}

inline static void                gyQuaternionFromLookRotation(GYQuaternion* dst, const GYVector3* forward, const GYVector3* upwards) {
#if 0
    mx::Vector3f forward_l = mx::normalize(target - position);
    mx::Vector3f forward_w(1, 0, 0);
    mx::Vector3f axis  = forward_l % forward_w;
    float        angle = mx::rad_to_deg(acos(forward_l * forward_w));

    mx::Vector3f third = axis % forward_w;
    if (third * forward_l < 0)
    {
        angle = - angle;
    }
    mx::Quaternionf q1 = mx::axis_angle_to_quaternion(angle, axis);

    mx::Vector3f up_l  = mx::transform(q1, mx::normalize(up));
    mx::Vector3f right = mx::normalize(forward_l % up);
    mx::Vector3f up_w  = mx::normalize(right % forward_l);

    mx::Vector3f axis2  = up_l % up_w;
    float        angle2 = mx::rad_to_deg(acos(up_l * up_w));

    mx::Quaternionf q2 = mx::axis_angle_to_quaternion(angle2, axis2);

    orientation = q2 * q1;
#endif
}

//quaternion product = cross product - dot product


inline static void gyQuaternionNormalize(GYQuaternion* dst, const GYQuaternion* src) {
    memcpy(dst, src, sizeof(GYQuaternion));
    gyVec4Normalize(&dst);
}

inline static float gyQuaternionMagnitude(const GYQuaternion* q) {
    return gyVec4Magnitude(q);
}

inline static void gyQuaternionRotateTowards(GYQuaternion* dst, const GYQuaternion* from, const GYQuaternion* to, float maxDegreesDelta) {
    //needs to be save to use dst as a src
}

inline static void gyQuaternionSlerp(GYQuaternion* dst, const GYQuaternion* start, const GYQuaternion* stop, float elapsed) {
#if 0
    // Only unit quaternions are valid rotations.
        // Normalize to avoid undefined behavior.
        v0.normalize();
        v1.normalize();

        // Compute the cosine of the angle between the two vectors.
        double dot = dot_product(v0, v1);

        // If the dot product is negative, slerp won't take
        // the shorter path. Note that v1 and -v1 are equivalent when
        // the negation is applied to all four components. Fix by
        // reversing one quaternion.
        if (dot < 0.0f) {
            v1 = -v1;
            dot = -dot;
        }

        const double DOT_THRESHOLD = 0.9995;
        if (dot > DOT_THRESHOLD) {
            // If the inputs are too close for comfort, linearly interpolate
            // and normalize the result.

            Quaternion result = v0 + t*(v1 - v0);
            result.normalize();
            return result;
        }

        // Since dot is in range [0, DOT_THRESHOLD], acos is safe
        double theta_0 = acos(dot);        // theta_0 = angle between input vectors
        double theta = theta_0*t;          // theta = angle between v0 and result
        double sin_theta = sin(theta);     // compute this value only once
        double sin_theta_0 = sin(theta_0); // compute this value only once

        double s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
        double s1 = sin_theta / sin_theta_0;

        return (s0 * v0) + (s1 * v1);
#endif
}

//PROBABLY WRONG AS FUCK!
inline static void gyQuaternionMult(GYQuaternion* d, const GYQuaternion* src1, const GYQuaternion* src2) {

#if 0

    float scalar=s*q.s - v.dot(q.v);

    R4DVector3n imaginary=q.v*s + v*q.s + v.cross(q.v);

    return R4DQuaternion(scalar,imaginary);

#else
    GYQuaternion ret = {
        src1->w * src2->w - src1->x * src2->x - src1->y * src2->y - src1->z * src2->z,  // 1
        src1->w * src2->x + src1->x * src2->w + src1->y * src2->z - src1->z * src2->y,  // i
        src1->w * src2->y - src1->x * src2->z + src1->y * src2->w + src1->z * src2->x,  // j
        src1->w * src2->z + src1->x * src2->y - src1->y * src2->x + src1->z * src2->w   // k
    };
    memcpy(d, &ret, sizeof(GYQuaternion));
#endif
}


inline static float gyQuaternionAngle(const GYQuaternion* x, const GYQuaternion* y) {
    GYQuaternion yConj, z;
    gyQuaternionInverse(&yConj, y);
    gyQuaternionMult(&z, x, &yConj);
    return 2.0f * acos(z.w);
}

inline static void gyQuaternionUnitNorm(GYQuaternion* q, const GYQuaternion* src) {
    gyQuaternionNormalize(q, src);
    float sinf, cosf;
    gySinCos(src->w * 0.5f, &sinf, &cosf);
    q->w = cosf;
    gyVec3ScaleAccum(&q->x, sinf);
}


//ONLY VALID IF Q IS A "PURE QUATERNION" -- NO REAL TERM, W MUST BE ZERO!!!
inline static void gyQuaternionMultVec3(GYVector3* d, const GYQuaternion* srcQ, const GYVector3* srcV) {
    GYQuaternion p = { 0.0f, srcV->x, srcV->y, srcV->z };
    GYVector3 axis;
    GYQuaternion q, qInv;
    float angle;

    gyQuaternionToAngleAxis(srcQ, &axis, &angle);
    gyVec3Normalize(&axis);
    gyQuaternionFromAngleAxis(&q, &axis, angle);
    gyQuaternionUnitNorm(&q, &q);
    gyQuaternionInverse(&qInv, &q);

    gyQuaternionMult(&q, &q, &p);
    gyQuaternionMult(&q, &q, &qInv);

    memcpy(d, &q.x, sizeof(GYVector3));
}




#include "common/gyro_matrix_stack.h"

#ifdef __cplusplus
}
#endif

#endif  // GYRO_MATH_H
