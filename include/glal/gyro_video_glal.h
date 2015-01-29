#ifndef GYRO_VIDEO_GLAL_H
#define GYRO_VIDEO_GLAL_H

#include <gyro_matrix_api.h>
#include <gyro_video_api.h>
#include "gyro_shaders_glal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE	100000

struct _GYTexture;
struct _GYCamera;

extern const struct _GYCamera*                  curCamera;
extern GYMatrix4				viewMatrix;
extern float					viewZOrient;
extern float					viewScale;
extern GYVector3				camWorldPos;
extern const GYVector3*                         tangentVector;
extern const GYVector3*                         normalVector;
extern const struct _GYTexture* curTex;
extern unsigned					vertexRenderCount;
extern unsigned					curStripSize;
extern GYVector4				modelViewVertices[];
extern GYColor4					stripColors[];
extern GYTexCoord2				stripTexCoords[];
extern TEXTURE_FLIP                             textureFlip;

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight);

void glalVidTangentUpdateUniform(void);
void glalVidNormalUpdateUniform(void);
void glalVidTextureFlipUpdateUniform(void);

static inline void glalStripComboBreaker(void) {
    if(curStripSize) {
        gyVidQuadListEnd();
    }
}

static inline TEXTURE_FLIP glalTextureFlipFromOrient(const GY_RECT_TEX_ORIENT orient) {
    switch(orient) {
    case GY_RECT_TEX_ORIENT_FLIP_VERT:
        return TEXTURE_FLIP_VERT;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
        return TEXTURE_FLIP_HORIZ;
    default:
        return TEXTURE_FLIP_NONE;
    }
}

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
