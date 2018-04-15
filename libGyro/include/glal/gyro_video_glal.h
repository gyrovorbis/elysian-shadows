#ifndef GYRO_VIDEO_GLAL_H
#define GYRO_VIDEO_GLAL_H

#include <gyro_matrix_api.h>
#include <gyro_video_api.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE	(1024*4)

#define GLAL_RENDERBUFFER_WIDTH     1024
#define GLAL_RENDERBUFFER_HEIGHT    1024

struct GYTexture;
struct GYCamera;

extern const struct GYCamera*       glalCurCamera;
extern GYMatrix4                    glalViewMatrix;
extern float                        glalViewZOrient;
extern float                        glalViewScale;
extern GYVector3                    glalCamWorldPos;
extern const GYVector3*             glalTangentVector;
extern const GYVector3*             glalNormalVector;
extern const struct GYTexture*      glalCurTex;
extern unsigned                     glalCurStripSize;
extern GYVector4                    glalModelViewVertices[];
extern GYTexCoord2                  glalStripTexCoords[];
extern GY_RECT_TEX_ORIENT           glalRectTexOrient;
extern GY_RECT_PLANE_TYPE           glalPlaneType;
extern unsigned                     glalVertexRenderCount;
extern unsigned                     glalUniformChangeCount;
extern unsigned                     glalPrevVertexRenderCount;
extern unsigned                     glalPrevUniformChangeCount;
extern unsigned                     glalTextureChangeCount;
extern unsigned                     glalPrevTextureChangeCount;
extern GY_ALPHA_LEVEL               glalAlphaLevel;
extern GY_BLEND_MODE                glalBlendMode;
extern int                          glalDepthTest;
extern int                          glalVersionMajor;
extern int                          glalVersionMinor;

extern const GYVector4 glalRectVerts[GY_RECT_PLANE_MAX][4];
extern const GYVector3 glalPlaneNormals[GY_RECT_PLANE_MAX];
extern const GYVector3 glalRectTangents[GY_RECT_TEX_ORIENT_MAX];
extern const GYVector3 glalPlaneNormalMapNormals[GY_RECT_PLANE_MAX][GY_RECT_TEX_ORIENT_MAX];

int glalVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight);
int glalVidGlewInit(void);

void glalVidTangentUpdateUniform(const GYVector3*const tangent);
void glalVidNormalUpdateUniform(const GYVector3*const normal);
void glalVidBindDefaultFbo(void);
int  glalVidDefaultFbo(void);

static inline float glalOrientNormalMapZFactor(const GY_RECT_TEX_ORIENT orient) {
    switch(orient) {
    case GY_RECT_TEX_ORIENT_FLIP_VERT:
        //return GLAL_TEXTURE_FLIP_VERT;
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
    case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
        return -1.0f;
    default:
        return 1.0f;
    }
}

static inline const GYVector3* glalPlaneNormalVector(const int normalMapActive, const GY_RECT_PLANE_TYPE planeType, const GY_RECT_TEX_ORIENT texOrient) {
   return (!normalMapActive)? &glalPlaneNormals[planeType] : &glalPlaneNormalMapNormals[planeType][texOrient];
}

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_GLAL_H
