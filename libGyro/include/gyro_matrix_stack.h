#ifndef GYRO_MATRIX_STACK_H
#define GYRO_MATRIX_STACK_H

#include "gyro_math.h"
#include "gyro_matrix.h"

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


extern GYMatrix4 _matStack[GYRO_MATH_MATRIX_STACK_DEPTH];
extern unsigned int _curStackFr;
extern GYMatrix4 _identityMat;
extern GYMatrix4 _translationMat;
extern GYMatrix4 _scaleMat;
extern GYMatrix4 _zRotationMat;
extern GYMatrix4 _tranScaleMat;
extern GYMatrix4 _instanceMat;

//===INLINE THE FUCK OUT OF EVERYTHING =====
//====MATRIX STACK OPERATIONS======
static inline void gyMatPush(void) {
	//assert(_curStackFr < GYRO_MATH_MATRIX_STACK_DEPTH);
	gyMatStore(&_matStack[_curStackFr]);
	++_curStackFr;
}

static inline void gyMatPop(void) {
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

static inline void gyMatInstance(const GYInstMat *const inst) {
	GYVector2 sc = inst->_size;
	float sin, cos;
	gyVec2CompProdAccum(&sc, &inst->_scale);
	gySinCos(inst->_orientZ, &sin, &cos);
	_instanceMat[0][0] = sc.x*cos; _instanceMat[1][0] = -sc.y*sin; _instanceMat[3][0] = inst->_pos.x;
	_instanceMat[0][1] = sc.x*sin; _instanceMat[1][1] = sc.y*cos; _instanceMat[3][1] = inst->_pos.y;
	_instanceMat[3][2] = inst->_posZ; //and scale Z?
	gyMatApply(_instanceMat);
}

    
#ifdef __cplusplus
}
#endif

#endif
