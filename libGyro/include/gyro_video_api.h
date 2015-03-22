#ifndef GYRO_VIDEO_H
#define GYRO_VIDEO_H

#include "gyro_texture_api.h"
#include "gyro_matrix_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _GYVidInitParams;
struct _GYCamera;

typedef struct _GYColor4 {
	unsigned char r, g, b, a;
} GYColor4;

typedef struct GYVertex {
    GYVector4	pos;
    GYTexCoord2	texCoord;
    GYColor4	color;
    GYVector3   normal;
    GYVector3   tangent;
} GYVertex;

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
    GY_BLEND_ONE_MINUS_SRC_ALPHA,
    GY_BLEND_ONE
} GY_BLEND_MODE;

//Internal state
void gyVidTexBind(const struct _GYTexture *const tex);
void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev);
void gyVidBlendMode(const GY_BLEND_MODE mode);
void gyVidDepthTest(const int enable);
void gyVidResolution(unsigned int *const width, unsigned int *const height);
void gyVidClearColor(const GYColor4 color);

//Camera API
void gyVidCamUpdate(const struct _GYCamera *const cam);
void gyVidSetCamWorldPos(const GYVector3*const pos);

//General Video API
int         gyVidInit(const struct _GYVidInitParams *const params); //MUST SET PROJWIDTH/PROJHEIGHT
int         gyVidUninit(void);
void        gyVidSceneBegin(void);
void        gyVidSceneEnd(void);

//Quad API
void gyVidQuadListBegin(const GYColor4 *const color);
void gyVidQuadListEnd(void);
void gyVidQuad(const struct _GYVector4 *const verts, const int frame);
void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient);
void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type);
void gyVidRect(const int frame);

//Triangle strip API 
void gyVidColorArray(const GYColor4 *const ptr);
void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr);
//void gyVidNormalArray(const struct _GYVector3 *const ptr);
//void gyVidTangentArray(const struct _GYVector3 *const ptr);
void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned startIndex, const unsigned amount);

//Spine interop
struct spSkeleton;
void gyVidSkeleton(const struct spSkeleton*const skeleton);



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
