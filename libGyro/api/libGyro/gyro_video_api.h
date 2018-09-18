#ifndef GYRO_VIDEO_H
#define GYRO_VIDEO_H

#include "gyro_texture_api.h"
#include "gyro_matrix_api.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct GYVidFrameStats {
    size_t primCount;
    size_t vertCount;
    size_t primCullCount;
    size_t batchCount;
    size_t avgBatchSize;
} GYVidFrameStats;

typedef enum _GY_ALPHA_LEVEL { GY_ALPHA_NONE, GY_ALPHA_PUNCHTHRU, GY_ALPHA_FULL } GY_ALPHA_LEVEL;

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

typedef enum GY_BLEND_MODE {
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
} GY_BLEND_MODE;

//Internal state
void gyVidTexBind(const struct GYTexture *const tex);
void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev);
void gyVidBlendMode(const GY_BLEND_MODE src, const GY_BLEND_MODE dst);
void gyVidDepthTest(const int enable);
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

#include "common/gyro_video_common.h"


#ifdef GYRO_VIDEO_PLATFORM_INCLUDE
#	include GYRO_VIDEO_PLATFORM_INCLUDE
#endif


#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_H
