#ifndef GYRO_VIDEO_H
#define GYRO_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif

#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#   include "x86/gyro_video_x86.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
#   include "dc/gyro_video_dc.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#   include "ios/gyro_video_ios.h"
#else
    typedef void GYVidInitParams;
#endif
    
#include "gyro_texture.h"

struct _GYVector4;
struct _GYVector3;
struct _GYShaderProgram;
struct _GYShader;

typedef struct _GYColor4 {
	unsigned char r, g, b, a;
} GYColor4;

typedef enum _GY_ALPHA_LEVEL { GY_ALPHA_NONE, GY_ALPHA_PUNCHTHRU, GY_ALPHA_FULL } GY_ALPHA_LEVEL;


//########PLATFORM-SPECIFIC IMPLEMENTATIONS#######################

//Internal state
void gyVidTexBind(const struct _GYTexture *const tex);
void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev);
void gyVidResolution(unsigned int *const width, unsigned int *const height);

//General Video API
int gyVidInit(const GYVidInitParams *const params); //MUST SET PROJWIDTH/PROJHEIGHT
int gyVidUninit(void);
void gyVidSceneBegin(void);
void gyVidSceneEnd(void);

//Quad API
void gyVidQuadListBegin(const GYColor4 *const color);
void gyVidQuadListEnd(void);
void gyVidQuad(const struct _GYVector4 *const verts, const unsigned int frame);
void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient);
void gyVidRect(const unsigned int frame);

//Triangle strip API 
void gyVidColorArray(const GYColor4 *const ptr);
void gyVidTexCoordArray(const GYTexCoord2 *const ptr);
void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount);

//Shader API
void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program);


//#########/PLATFORM-SPECIFIC IMPLEMENTATIONS####################

//Private Internal Shader Functions
int _gyVidShaderCreate(struct _GYShader* shader);
int _gyVidShaderDestroy(struct _GYShader* shader);
int _gyVidShaderProgramCreate(struct _GYShaderProgram* program);
int _gyVidShaderProgramDestroy(struct _GYShaderProgram* program);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VIDEO_H
