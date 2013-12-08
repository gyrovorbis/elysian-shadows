#include "gyro_platform.h"

#ifdef GYRO_MATRIX_SOFTWARE

#include "gyro_matrix.h"
#include "stdio.h"
#include "string.h"

#include <xmmintrin.h>

static GYMatrix4 _curMat;

void gyMatLoad(GYMatrix4 *mat) {
    memcpy(_curMat, mat, sizeof(float)*16);
}

void gyMatStore(GYMatrix4 *mat) {
    memcpy(mat, _curMat, sizeof(float)*16);
}

void gyMatApply(GYMatrix4 mat) {
    float res[4][4];
    //float **mat = _mat;
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
    memcpy(_curMat, res, sizeof(float)*16);
}

void gyMatMultVec(const GYVector4 *const src, GYVector4 *const dest, const unsigned int count) {
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

#endif
