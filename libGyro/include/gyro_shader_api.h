#ifndef GYRO_SHADER_H
#define GYRO_SHADER_H

#include "gyro_defines.h"
#include "gyro_matrix_api.h"

#define GYRO_VIDEO_LIGHT_SOURCES_MAX 20

#ifdef __cplusplus
extern "C" {
#endif

struct _GYColor4;
struct _GYTexture;

typedef struct _GYLightProperties {
	GYVector4	ambientColor;
	GYVector4	diffuseColor;
	GYVector4	specularColor;
	GYVector3	attenuationFactors;
	float		spotExponent;
} GYLightProperties;

typedef struct _GYLightSource {
	GYMatrix4					projViewMat;
	GYVector4					position;					//position.w = 0.0f for directional light
	GYVector3					direction;
	float						spotCutoffAngle;
	GYLightProperties*			properties;
} GYLightSource;

typedef struct _GYMaterial {
	GYVector4	ambientColor;
	GYVector4	diffuseColor;
	GYVector4	specularColor;
	GYVector4	emissiveColor;
	float		shininess;
} GYMaterial;

typedef struct _GYShader {
	char	path[GYRO_PATH_MAX_SIZE];
	int		type;
	int		shader;
} GYShader;

typedef struct _GYShaderProgram {
	char		path[GYRO_PATH_MAX_SIZE];
	GYShader*	vertShader;
	GYShader*	fragShader;
	int			program;
} GYShaderProgram;

//Lighting API
void gyVidLightSourceClear(void);
void gyVidLightSourceAdd(const GYLightSource* const light);
void gyVidLightSourceSetEnable(const int enabled);
void gyVidLightSourceAmbientSetEnable(const int enabled);
void gyVidLightSourceDiffuseSetEnable(const int enabled);
void gyVidLightSourceSpecularSetEnable(const int enabled);
void gyVidLightingSetEnable(const int enabled);

void gyVidMaterialBind(const GYMaterial*const material);

void gyVidNormalMapBind(const struct _GYTexture* const tex);

void gyVidSetGlobalAmbience(const struct _GYVector4*const color);

//Shadow Multipass Rendering
int gyVidBeginNextShadowPass(void);

//Shader manipulation
void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program);
void gyVidSetDefaultShaderProgram();

//Shader management
GYShader* gyShaderCreate(const char*const path);
int gyShaderDestroy(GYShader* shader);
GYShaderProgram* gyShaderProgramCreate(GYShader* vertShader, GYShader* fragShader);
int gyShaderProgramDestory(GYShaderProgram* program);
int gyShadersReloadAll(void);
void gyShaderWalk(void);

//Internal
int _gyVidShaderCreate(GYShader* shader);
int _gyVidShaderDestroy(GYShader* shader);
int _gyVidShaderProgramCreate(GYShaderProgram* program);
int _gyVidShaderProgramDestroy(GYShaderProgram* program);

#ifdef GYRO_SHADER_PLATFORM_INCLUDE
#	include GYRO_SHADER_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif


#endif // GYRO_SHADER_H
