#ifndef GYRO_VIDEO_GLAL_H
#define GYRO_VIDEO_GLAL_H

#include <gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _GYTexture;
struct _GYCamera;

extern const struct _GYCamera*	curCamera;
extern GYMatrix4				viewMatrix;
extern float					viewZOrient;
extern float					viewScale;
extern GYVector3				camWorldPos;
extern const GYVector3*			tangentVector;
extern const GYVector3*			normalVector;
extern const struct _GYTexture* curTex;
extern unsigned					polyCount;

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
