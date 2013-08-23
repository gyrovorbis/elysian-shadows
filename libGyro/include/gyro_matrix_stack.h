#ifndef GYRO_MATRIX_STACK_H
#define GYRO_MATRIX_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

struct _GYInstMat;

//INLINE ME!!!!! MACROS?! PROBABLY

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
void gyMatLoadIdentity(void);
    
#ifdef __cplusplus
}
#endif

#endif
