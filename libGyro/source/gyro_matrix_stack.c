#include "gyro_matrix.h"
#include "gyro_matrix_stack.h"
#include "gyro_math.h"
#include <assert.h>

#include "string.h"

//INLINE ALL THIS SHIT!!!!

#define GYRO_MATH_MATRIX_STACK_DEPTH 16

static GYMatrix4 _matStack[GYRO_MATH_MATRIX_STACK_DEPTH];
static unsigned int _curStackFr = 0;

static GYMatrix4 _identityMat =   
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

static GYMatrix4 _translationMat =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

static GYMatrix4 _scaleMat =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

static GYMatrix4 _zRotationMat =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

static GYMatrix4  _tranScaleMat =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

static GYMatrix4 _instanceMat =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

//====MATRIX STACK OPERATIONS======
void gyMatPush(void) {
    assert(_curStackFr < GYRO_MATH_MATRIX_STACK_DEPTH);
    gyMatStore(&_matStack[_curStackFr]);
    ++_curStackFr;
}

void gyMatPop(void) {
    --_curStackFr;
    gyMatLoad(&_matStack[_curStackFr]);
}

void gyMatLoadIdentity(void) {
    gyMatLoad(&_identityMat);
}

void gyMatTranslate(const float x, const float y, const float z) {
    _translationMat[3][0] = x;
    _translationMat[3][1] = y;
    _translationMat[3][2] = z;
	gyMatApply(_translationMat);
}

void gyMatRotateZ(const float theta) {
    float zsin, zcos;
    gySinCos(theta, &zsin, &zcos);
    _zRotationMat[0][0] = zcos;  _zRotationMat[1][0] = zsin;
    _zRotationMat[0][1] = -zsin; _zRotationMat[1][1] = zcos;
	gyMatApply(_zRotationMat);
}

void gyMatScale(const float sx, const float sy, const float sz) {
    _scaleMat[0][0] = sx;
    _scaleMat[1][1] = sy;
    _scaleMat[2][2] = sz;
	gyMatApply(_scaleMat);
}

void gyMatTranslateScale(const float x, const float y, const float z, const float sx, const float sy, const float sz) {
	_tranScaleMat[0][0] = sx;
    _tranScaleMat[1][1] = sy;
    _tranScaleMat[2][2] = sz;
	_tranScaleMat[3][0] = x;
    _tranScaleMat[3][1] = y;
    _tranScaleMat[3][2] = z;
	gyMatApply(_tranScaleMat);
}

void gyMatInstance(const GYInstMat *const inst) {
	GYVector2 sc = inst->_size;
    float sin, cos;
	gyVec2CompProdAccum(&sc, &inst->_scale);
	gySinCos(inst->_orientZ, &sin, &cos);
    _instanceMat[0][0] = sc.x*cos; _instanceMat[1][0] = -sc.y*sin; _instanceMat[3][0] = inst->_pos.x;
    _instanceMat[0][1] = sc.x*sin; _instanceMat[1][1] = sc.y*cos; _instanceMat[3][1] = inst->_pos.y;
	_instanceMat[3][2] = inst->_posZ; //and scale Z?
	gyMatApply(_instanceMat);
}
