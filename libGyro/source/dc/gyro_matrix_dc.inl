#include <kos.h>

static inline void gyMatLoad(GYMatrix4 *mat) {
    mat_load(mat);
}

static inline void gyMatStore(GYMatrix4 *mat) {
    mat_store(mat);
}

static inline void gyMatApply(GYMatrix4 *mat) {
    mat_apply(mat);
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

