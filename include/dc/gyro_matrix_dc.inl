#include <kos.h>

#define VEC2(a) ((GYVector2*)a)
#define VEC3(a) ((GYVector3*)a)
#define VEC4(a) ((GYVector4*)a)

static inline void gyMatLoad(GYMatrix4 *mat) {
    mat_load(mat);
}

static inline void gyMatStore(GYMatrix4 *mat) {
    mat_store(mat);
}

static inline void gyMatApply(GYMatrix4 *mat) {
    mat_apply(mat);
}

static inline void gyMatMultMat(GYMatrix4 dMat, const GYMatrix4 sMat1, const GYMatrix4 sMat2) {

}

static inline void gyVec3CompProdAccum(void* const dest, const void* const src) {
    VEC3(dest)->x *= VEC3(src)->x; VEC3(dest)->y *= VEC3(src)->y; VEC3(dest)->z *= VEC3(src)->z;
}

static inline void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, int count) {
    //MOTHER FUCKING PERSPECTIVE DIVISION!!!!
    //mat_transform((vector_t*)(void *)src, (vector_t*)(void *)dest, count, 16);
    unsigned i = 0;
    float x, y, z, w;
    for(; i < count; ++i) {
        x = src[i].x;
	y = src[i].y;
	z = src[i].z;
	w = src[i].w;

	mat_trans_nodiv(x, y, z, w);
	
	dest[i].x = x;
	dest[i].y = y;
	dest[i].z = z;
	dest[i].w = w;
    }
}

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

