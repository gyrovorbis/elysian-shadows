#ifndef GYRO_VIDEO_H
#define GYRO_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_texture_api.h"
#include "gyro_matrix_api.h"

struct _GYVidInitParams;

typedef struct _GYColor4 {
	unsigned char r, g, b, a;
} GYColor4;

typedef enum _GY_ALPHA_LEVEL { GY_ALPHA_NONE, GY_ALPHA_PUNCHTHRU, GY_ALPHA_FULL } GY_ALPHA_LEVEL;

typedef enum _GY_RECT_NORMAL_TYPE { GY_RECT_NORMAL_FLAT, GY_RECT_NORMAL_DEPTH, GY_RECT_NORMAL_MAX } GY_RECT_NORMAL_TYPE;

//Internal state
void gyVidTexBind(const struct _GYTexture *const tex);
void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev);
void gyVidResolution(unsigned int *const width, unsigned int *const height);

//Camera API
void gyVidProjectionSetTransform(const GYMatrix4 mat);
void gyVidCameraSetTransform(const GYMatrix4 mat);
//void gyVidCameraSetPosition(const struct _GYVector4* pos);
void gyVidCameraSetViewCenter(const GYVector4* const pos, const float scale, const float zOrient);


//General Video API
int gyVidInit(const struct _GYVidInitParams *const params); //MUST SET PROJWIDTH/PROJHEIGHT
int gyVidUninit(void);
void gyVidSceneBegin(void);
void gyVidSceneEnd(void);

//Quad API
void gyVidQuadListBegin(const GYColor4 *const color);
void gyVidQuadListEnd(void);
void gyVidQuad(const struct _GYVector4 *const verts, const unsigned int frame);
void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient);
void gyVidRectNormalType(const GY_RECT_NORMAL_TYPE type);
void gyVidRect(const unsigned int frame);

//Triangle strip API 
void gyVidColorArray(const GYColor4 *const ptr);
void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr);
//void gyVidNormalArray(const struct _GYVector3 *const ptr);
//void gyVidTangentArray(const struct _GYVector3 *const ptr);
void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned startIndex, const unsigned amount);

#include "common/gyro_video_common.h"


#ifdef GYRO_VIDEO_PLATFORM_INCLUDE
#	include GYRO_VIDEO_PLATFORM_INCLUDE
#endif


#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_H
