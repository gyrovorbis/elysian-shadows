#ifndef GYRO_MATRIX_STACK_H
#define GYRO_MATRIX_STACK_H

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_MATH_MATRIX_STACK_DEPTH 16

struct _GYInstMat;

//INLINE ME!!!!! MACROS?! PROBABLY
/*
void gyMatLoadIdentity(void);

//General Transforms
void gyMatTranslate(const float x, const float y, const float z);
void gyMatScale(const float sx, const float sy, const float sz);

//Compound Transforms
void gyMatRotateZ(const float theta);
void gyMatTranslateScale(const float x, const float y, const float z, const float sx, const float sy, const float sz);
void gyMatInstance(const struct _GYInstMat *const inst);

void gyMatInit(void);
void gyMatPush(void);
void gyMatPop(void);
void gyMatLoadIdentity(void);*/

extern unsigned int _curStackFr;
extern GYMatrix4 _matStack[GYRO_MATH_MATRIX_STACK_DEPTH];
extern GYMatrix4 _identityMat;
extern GYMatrix4 _translationMat;
extern GYMatrix4 _scaleMat;
extern GYMatrix4 _zRotationMat;
extern GYMatrix4 _rotationMat;
extern GYMatrix4 _tranScaleMat;
extern GYMatrix4 _instanceMat;
extern GYMatrix4 _shearMat;

//===INLINE THE FUCK OUT OF EVERYTHING =====
//====MATRIX STACK OPERATIONS======
static inline void gyMatPush(void) {
  assert(_curStackFr < GYRO_MATH_MATRIX_STACK_DEPTH);
  gyMatStore(&_matStack[_curStackFr]);
  ++_curStackFr;
}

static inline void gyMatPop(void) {
  assert(_curStackFr > 0);
  --_curStackFr;
  gyMatLoad(&_matStack[_curStackFr]);
}

static inline void gyMatLoadIdentity(void) {
  gyMatLoad(&_identityMat);
}

static inline void gyMatTranslate(const float x, const float y, const float z) {
  _translationMat[3][0] = x;
  _translationMat[3][1] = y;
  _translationMat[3][2] = z;
    gyMatApply(_translationMat);
}

static inline void gyMatRotateZ(const float theta) {
  float zsin, zcos;
  gySinCos(theta, &zsin, &zcos);
  _zRotationMat[0][0] = zcos;  _zRotationMat[1][0] = zsin;
  _zRotationMat[0][1] = -zsin; _zRotationMat[1][1] = zcos;
    gyMatApply(_zRotationMat);
}

static inline void gyMatScale(const float sx, const float sy, const float sz) {
  _scaleMat[0][0] = sx;
  _scaleMat[1][1] = sy;
  _scaleMat[2][2] = sz;
    gyMatApply(_scaleMat);
}

static inline void gyMatTranslateScale(const float x, const float y, const float z, const float sx, const float sy, const float sz) {
  _tranScaleMat[0][0] = sx;
  _tranScaleMat[1][1] = sy;
  _tranScaleMat[2][2] = sz;
  _tranScaleMat[3][0] = x;
  _tranScaleMat[3][1] = y;
  _tranScaleMat[3][2] = z;
    gyMatApply(_tranScaleMat);
}



static inline void gyMatRotate(const float angle, const float x, const float y, const float z) {
  float sinTheta, cosTheta;

  gySinCos(angle, &sinTheta, &cosTheta);

#if 0
    const float xy = x*y;
    const float yz = y*z;
    const float xz = x*z;
    const float oneMinusCos = 1.0f - cosTheta;
    const float
#endif

  _rotationMat[0][0] = cosTheta + x*x*(1.0f-cosTheta);
  _rotationMat[1][0] = y*x*(1.0f-cosTheta) + z*sinTheta;
  _rotationMat[2][0] = z*x*(1.0f-cosTheta) - y*sinTheta;

  _rotationMat[0][1] = x*y*(1.0f-cosTheta) - z*sinTheta;
  _rotationMat[1][1] = cosTheta + y*y*(1.0f-cosTheta);
  _rotationMat[2][1] = z*y*(1.0f-cosTheta) + x*sinTheta;

  _rotationMat[0][2] = x*z*(1.0f-cosTheta) + y*sinTheta;
  _rotationMat[1][2] = y*z*(1.0f-cosTheta) - x*sinTheta;
  _rotationMat[2][2] = cosTheta + z*z*(1.0f-cosTheta);

    gyMatApply(_rotationMat);
}

static inline void gyMatInstance(const GYInstMat *const inst) {
    GYVector3 flip = {
        (inst->_flip & GY_INST_MAT_FLIP_X_AXIS)? -1.0f : 1.0f,
        (inst->_flip & GY_INST_MAT_FLIP_Y_AXIS)? -1.0f : 1.0f,
        (inst->_flip & GY_INST_MAT_FLIP_Z_AXIS)? -1.0f : 1.0f
    };

    gyVec3CompProdAccum(&flip, &inst->_size);
    gyVec3CompProdAccum(&flip, &inst->_scale);

    //gySinCos(inst->_orientZ, &sin, &cos);

#if 0
    _instanceMat[0][0] = sc.x*cos; _instanceMat[1][0] = -sc.y*sin; _instanceMat[3][0] = inst->_pos.x;
    _instanceMat[0][1] = sc.x*sin; _instanceMat[1][1] = sc.y*cos; _instanceMat[3][1] = inst->_pos.y;
    _instanceMat[2][2] = sc.z;
    _instanceMat[3][2] = inst->_pos.z; //and scale Z?
    gyMatApply(_instanceMat);
#else
    gyMatTranslate(inst->_pos.x, inst->_pos.y, inst->_pos.z);
    gyMatScale(flip.x, flip.y, flip.z);
    gyMatRotate(inst->_angle, inst->_orient.x, inst->_orient.y, inst->_orient.z);

#endif


}

static inline void gyMatShearZ(const float shx, const float shy) {
#if 1
  _shearMat[2][0] = shx; _shearMat[3][0] = 0;
  _shearMat[2][1] = shy; _shearMat[3][1] = 0;
#else
  _shearMat[1][0] = shx; //_shearMat[3][0] = -shx;
  _shearMat[1][2] = shy;// _shearMat[3][1] = -shy;

#endif
    gyMatApply(_shearMat);
}


#ifdef __cplusplus
}
#endif

#endif
