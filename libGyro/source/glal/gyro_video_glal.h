#ifndef GYRO_VIDEO_GLAL_H
#define GYRO_VIDEO_GLAL_H

#include <gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _GYTexture;

extern GYMatrix4				projectionViewMatrix;
extern GYMatrix4				projectionMatrix;
extern GYMatrix4				viewMatrix;
extern float					viewZOrient;
extern float					viewScale;
extern GYVector4				viewPosition;
extern const GYVector3*			tangentVector;
extern const GYVector3*			normalVector;
extern const struct _GYTexture* curTex;

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight);
void glalOrthoProjMatrix(GYMatrix4 mat, const float pleft, const float pright,
						 const float pbottom, const float ptop,
						 const float pnear, const float pfar);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
