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

extern const struct _GYCamera*      glalCurCamera;
extern GYMatrix4                    glalViewMatrix;
extern float                        glalViewZOrient;
extern float                        glalViewScale;
extern GYVector3                    glalCamWorldPos;
extern const GYVector3*             glalTangentVector;
extern const GYVector3*             glalNormalVector;
extern const struct _GYTexture*     glalCurTex;
extern unsigned                     glalVertexRenderCount;
extern unsigned                     glalCurStripSize;
extern GYVector4                    glalModelViewVertices[];
extern GYColor4                     _stripColors[];
extern GYTexCoord2                  glalStripTexCoords[];
extern GLAL_TEXTURE_FLIP            glalTextureFlip;
extern GY_RECT_TEX_ORIENT           glalRectTexOrient;
extern GY_RECT_PLANE_TYPE           glalPlaneType;
extern unsigned                     glalVertexRenderCount;

int glalVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight);

void glalVidTangentUpdateUniform(void);
void glalVidNormalUpdateUniform(void);
void glalVidTextureFlipUpdateUniform(void);

static inline void glalStripComboBreaker(void) {
    if(glalCurStripSize) {
        gyVidQuadListEnd();
    }
}

static inline GLAL_TEXTURE_FLIP glalTextureFlipFromOrientAndPlane(const GY_RECT_TEX_ORIENT orient) {
    switch(orient) {
    case GY_RECT_TEX_ORIENT_FLIP_VERT:
        //return GLAL_TEXTURE_FLIP_VERT;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
        return GLAL_TEXTURE_FLIP_HORIZ;
    default:
        return GLAL_TEXTURE_FLIP_NONE;
    }
}

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
