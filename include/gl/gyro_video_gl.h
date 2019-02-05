#ifndef GYRO_VIDEO_GL_H
#define GYRO_VIDEO_GL_H

#include <libGyro/gyro_matrix_api.h>
#include <libGyro/gyro_video_api.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE	(1024*4)

#define GLAL_RENDERBUFFER_WIDTH     1024
#define GLAL_RENDERBUFFER_HEIGHT    1024

struct GYTexture;
struct GYCamera;

typedef enum GY_GL_VERION_TYPE {
    GY_GL_VERSION_DESKTOP,
    GY_GL_VERSION_ES
} GY_GL_VERSION_TYPE;

typedef enum GY_GL_VENDOR_TYPE {
    GY_GL_VENDOR_NVIDIA,
    GY_GL_VENDOR_AMD,
    GY_GL_VENDOR_INTEL,
    GY_GL_VENDOR_IMAGINATION,
    GY_GL_VENDOR_APPLE,
    GY_GL_VENDOR_UNKNOWN
} GY_GL_VENDOR_TYPE;

typedef enum GY_GL_API_VERSION {
    GY_GL_API_VERSION_NONE,
    GY_GL_API_VERSION_COMPATABILITY,
    GY_GL_API_VERSION_CORE_3_2,
    GY_GL_API_VERSION_CORE_3_3,
    GY_GL_API_VERSION_CORE_4_0,
    GY_GL_API_VERSION_CORE_4_1,
    GY_GL_API_VERSION_CORE_4_2,
    GY_GL_API_VERSION_CORE_4_3,
    GY_GL_API_VERSION_CORE_4_4,
    GY_GL_API_VERSION_CORE_4_5,
    GY_GL_API_VERSION_CORE_4_6,
    GY_GL_API_VERSION_CORE_4_6_PLUS,
    GY_GL_API_VERSION_ES_1_0,
    GY_GL_API_VERSION_ES_1_1,
    GY_GL_API_VERSION_ES_2_0,
    GY_GL_API_VERSION_ES_3_0,
    GY_GL_API_VERSION_ES_3_1,
    GY_GL_API_VERSION_ES_3_2,
    GY_GL_API_VERSION_ES_3_2_PLUS,
    GY_GL_API_VERSION_UNKNOWN
} GY_GL_API_VERSION;

typedef enum GY_GL_SHADER_VERSION {
    GY_GL_SHADER_VERSION_110,
    GY_GL_SHADER_VERSION_120,
    GY_GL_SHADER_VERSION_130,
    GY_GL_SHADER_VERSION_140,
    GY_GL_SHADER_VERSION_150,
    GY_GL_SHADER_VERSION_330,
    GY_GL_SHADER_VERSION_400,
    GY_GL_SHADER_VERSION_410,
    GY_GL_SHADER_VERSION_420,
    GY_GL_SHADER_VERSION_430,
    GY_GL_SHADER_VERSION_440,
    GY_GL_SHADER_VERSION_450,
    GY_GL_SHADER_VERSION_460,
    GY_GL_SHADER_VERSION_UNKNOWN
} GY_GL_SHADER_VERSION;

typedef struct GYGLInfo {
    GY_GL_VERSION_TYPE  versionType;
    GY_GL_API_VERSION   apiVersion;
    GY_GL_VENDOR_TYPE   vendorType;

    //GL Limits
    int                 depthMax;
    int                 textureMaxSize;
    int                 texture2DMaxSize[2];
    int                 texture3DMaxSize[3];


    //Basic feature support
    int                 hasCompute;
    int                 hasTessellation;
    int                 hasDrawIndirect;
    int                 hasPinnedMemory;
    int                 hasShaderBufferObjects;
    int                 hasTextureCubeMapArrays;
    int                 hasFloatTextures;
    int                 hasOcclusionQueries;
    int                 hasTransformFeedback;
    char**              extensions;
} GYGLInfo;


extern GYVidPipelineState           _glState;
extern const struct GYCamera*       glalCurCamera;
extern GYMatrix4                    glalViewMatrix;
extern float                        glalViewZOrient;
extern float                        glalViewScale;
extern GYVector3                    glalCamWorldPos;
extern const GYVector3*             glalTangentVector;
extern const GYVector3*             glalNormalVector;
//extern const struct GYTexture*      glalCurTex;
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
extern GY_BLEND_FACTOR              glalBlendMode;
extern int                          glalDepthTest;
extern int                          glalVersionMajor;
extern int                          glalVersionMinor;
extern GYGLInfo                     glalGlInfo;

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

void gyVidPipelineInvalidate(void);

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
