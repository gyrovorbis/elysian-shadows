/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VIDEO_H
#define GYRO_VIDEO_H

#include "gyro_texture_api.h"
#include "gyro_matrix_api.h"

#ifdef __cplusplus
extern "C" {
#endif


#define GY_VIDEO_DEBUG

struct GYVidInitParams;
struct GYCamera;

typedef struct GYColor4 {
    unsigned char r, g, b, a;
} GYColor4;

typedef struct GYVertex {
    GYVector4   pos;        //16 bytes
    GYTexCoord2 texCoord;   //8 bytes
    GYColor4    color;      //16 bytes
    GYVector3   normal;     //12 bytes
    GYVector3   tangent;    //12 bytes
    float       normalZ;    //4 bytes
} GYVertex;                 //68 bytes


typedef enum _GY_ALPHA_LEVEL {
    GY_ALPHA_NONE,
    GY_ALPHA_PUNCHTHRU,
    GY_ALPHA_FULL
} GY_ALPHA_LEVEL;

typedef enum _GY_RECT_PLANE_TYPE {
    GY_RECT_PLANE_BOTTOM,
    GY_RECT_PLANE_FRONT,
    GY_RECT_PLANE_LEFT,
    GY_RECT_PLANE_RIGHT,
    GY_RECT_PLANE_FRONT_SLOPE_45,
    GY_RECT_PLANE_LEFT_SLOPE_45,
    GY_RECT_PLANE_RIGHT_SLOPE_45,
    GY_RECT_PLANE_MAX
} GY_RECT_PLANE_TYPE;

typedef enum GY_BLEND_FACTOR {
    GY_BLEND_NONE,
    GY_BLEND_ZERO,
    GY_BLEND_ONE,
    GY_BLEND_SRC_COLOR,
    GY_BLEND_ONE_MINUS_SRC_COLOR,
    GY_BLEND_DST_COLOR,
    GY_BLEND_ONE_MINUS_DST_COLOR,
    GY_BLEND_SRC_ALPHA,
    GY_BLEND_ONE_MINUS_SRC_ALPHA,
    GY_BLEND_DST_ALPHA,
    GY_BLEND_ONE_MINUS_DST_ALPHA,
    GY_BLEND_CONSTANT_COLOR,
    GY_BLEND_ONE_MINUS_CONSTANT_COLOR,
    GY_BLEND_CONSTANT_ALPHA,
    GY_BLEND_ONE_MINUS_CONSTANT_ALPHA,
    GY_BLEND_SRC_ALPHA_SATURATE,
    GY_BLEND_SRC1_COLOR,
    GY_BLEND_ONE_MINUS_SRC1_COLOR,
    GY_BLEND_SRC1_ALPHA,
    GY_BLEND_ONE_MINUS_SRC1_ALPHA,
    GY_BLEND_UNKNOWN
} GY_BLEND_FACTOR;

typedef enum GY_VID_DEPTH_MODE {
    GY_VID_DEPTH_NEVER,
    GY_VID_DEPTH_LESS,
    GY_VID_DEPTH_EQUAL,
    GY_VID_DEPTH_LEQUAL,
    GY_VID_DEPTH_GREATER,
    GY_VID_DEPTH_NOTEQUAL,
    GY_VID_DEPTH_GEQUAL,
    GY_VID_DEPTH_ALWAYS,
    GY_VID_DEPTH_COUNT
} GY_VID_DEPTH_MODE;


typedef struct GYVidPipelineState {
    int 				shader;
    GYTexture*          texture;
    int					normalMapId;
    int					emissiveMapId;
    GY_ALPHA_LEVEL      alphaLevel;
    GY_BLEND_FACTOR     srcBlendFactor;
    GY_BLEND_FACTOR     dstBlendFactor;
    int					depthTestEnabled;
    GY_VID_DEPTH_MODE   depthTestMode;
} GYVidPipelineState;

typedef enum GY_VID_CULL_MODE {
    GY_VID_CULL_NONE,
    GY_VID_CULL_CW,
    GY_VID_CULL_CCW
} GY_VID_CULL_MODE;

typedef enum GY_VID_TEXTURE_WRAP {
    GY_VID_TEX_REPEAT,
    GY_VID_TEX_MIRRORED_REPEAT,
    GY_VID_TEX_CLAMP_TO_EDGE,
    GY_VID_TEX_CLAMP_TO_BORDER
} GY_VID_TEXTURE_WRAP;

typedef enum GY_VID_TEXTURE_FILTER {
    GY_VID_TEX_FILTER_NONE,
    GY_VID_TEX_FILTER_NEAREST,
    GY_VID_TEX_FILTER_BILINEAR,
    GY_VID_TEX_FILTER_TRILINEAR
    //,GY_VID_TEX_FILTER_TRILINEAR2 //trilinear pass 2 for Dreamcast wtf?
} GY_VID_TEXTURE_FILTER;



#ifdef GY_VIDEO_DEBUG

typedef struct GYVidFrameStats {
    size_t primCount;
    size_t vertCount;
    size_t primCullCount;
    size_t quadCount;
    size_t stripCount;
    size_t batchCount;
    size_t avgBatchSize;

    size_t pipelineStateChangeCount;
    size_t shaderChangeCount;
    size_t textureChangeCount;
    size_t alphaChangeCount;
    size_t blendChangeCount;
    size_t depthTestChangeCount;
    size_t depthModeChangeCount;
} GYVidFrameStats;


typedef struct GYVidDebugInfo {
    GYVidFrameStats     frameStats;
    size_t              vramFreeBytes;
    size_t              vramUsedBytes;
    size_t              vramUsedWatermark;
    size_t              totalVramUsedBytes;
    size_t              totalVramFreedBytes;
    size_t              allocatedTextureCount;
    size_t              totalTextureFreedCount;
    size_t              totalTextureAllocatedCount;
    size_t              textureCountWatermark;
} GYVidDebugInfo;

#endif


//Internal state
void gyVidTexBind(const struct GYTexture *const tex);
void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev);
void gyVidBlendMode(const GY_BLEND_FACTOR src, const GY_BLEND_FACTOR dst);
//void gyVidDepthTest(const int enable);
void gyVidResolution(unsigned int *const width, unsigned int *const height);
void gyVidClearColor(const GYColor4 color);
void gyVidHighDynamicRangeExposure(float exposure);

void gyVidCpuFrustumCulling(int value);

//Camera API
void gyVidCamUpdate(const struct GYCamera *const cam);
void gyVidSetCamWorldPos(const GYVector3*const pos);

//General Video API
int         gyVidInit(const struct GYVidInitParams *const params); //MUST SET PROJWIDTH/PROJHEIGHT
int         gyVidUninit(void);
void        gyVidSceneBegin(void);
void        gyVidSceneEnd(void);
const GYVidFrameStats*
            gyVidFrameStatistics(void);
void        gyVidScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

//Quad API
void gyVidQuadListBegin(const GYColor4 *const color);
void gyVidQuadListEnd(void);
void gyVidQuad(const struct GYVector4 *const verts, const int frame);
void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient);
void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type);
void gyVidRect(const int frame);

//Triangle strip API
void gyVidColorArray(const GYColor4 *const ptr);
void gyVidTexCoordArray(const struct GYTexCoord2 *const ptr);
//void gyVidNormalArray(const struct _GYVector3 *const ptr);
//void gyVidTangentArray(const struct _GYVector3 *const ptr);
void gyVidTriangleStrip(const struct GYVector4 *const vertexArray, const unsigned startIndex, const unsigned amount);
void gyVidTriangleStripVertices(const GYVertex* vertex, unsigned count);

//Spine interop
struct spSkeleton;
void gyVidSkeleton(const struct spSkeleton*const skeleton);
struct spAttachment;
void gyVidSkeletonAttachment(struct spAttachment*const attachment);
void gyVidSkeletonAttachmentCallback(const int type, void (*callback)(struct spAttachment*));

void gyVidVertexArrayBind(int id);
void gyVidVertexBatchBegin(void (*flushCallback)(void*), void* obj);
void gyVidVertexBatchEnd(void);
void gyVidVertexBatchFlush(void);
void gyVidVertexBatchFlushPause(void);
void gyVidVertexBatchFlushResume(void);


//CHANGE THIS INTO ONE SHARED FUNCTION, NOT PLATFORM-SPECIFIC!!!
void gyVidViewFrustumMat(GYMatrix4 mat, const float fov, const float aspect, const float znear, const float zfar);
/*void gyVidLookAtMat(GYMatrix4 mat, const float ex, const float ey, const float ez,
                                    const float dx, const float dy, const float dz,
                                    const float ux, const float uy, const float uz);*/


//Optional Video debug API
void gyVidDbgTexMapUniform(const int uniform);
void gyVidDbgTexMappingEnabled(const int enabled);
void gyVidDbgNormalMappingEnabled(const int enabled);
void gyVidDbgSpeculaMappingEnabled(const int enabled);
unsigned gyVidDbgVertexRenderCount(void);
unsigned gyVidDbgShaderUniformChangeCount(void);
unsigned gyVidDbgTextureChangeCount(void);





#ifdef GY_VIDEO_DEBUG

const GYVidDebugInfo*   gyVidDebugQueryInfo(void);
int                     gyVidDebugStatsDump(void);
#endif


#if 0
typedef struct GYVidInitParams {
    GYColor4 clearColor;
} GYVidInitParams;
#endif




void gyVidPipelineState(const GYVidPipelineState* state);
//void gyVidScissorRect(mode);


//Remaining DC features:
#if 0
//1) hardware fog
//2) modifier volumes/cheap shadows
//3) texture paletting
//4) mipmapping
//5) texture compression
//6) depth write enable/disable
//7) shade flat/gourad
//pvr_txrenv_modes      Texture color calculation modes
#define PVR_TXRENV_REPLACE          0   /**< \brief C = Ct, A = At */
#define PVR_TXRENV_MODULATE         1   /**< \brief  C = Cs * Ct, A = At */
#define PVR_TXRENV_DECAL            2   /**< \brief C = (Cs * At) + (Cs * (1-At)), A = As */
#define PVR_TXRENV_MODULATEALPHA    3   /**< \brief C = Cs * Ct, A = As * At */

#define PVR_SPECULAR_DISABLE    0   /**< \brief Disable offset colors */
#define PVR_SPECULAR_ENABLE     1   /**< \brief Enable offset colors */
#endif







#include "common/gyro_video_common.h"


#ifdef GYRO_VIDEO_PLATFORM_INCLUDE
#	include GYRO_VIDEO_PLATFORM_INCLUDE
#endif


#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_H
