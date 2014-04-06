#ifndef GYRO_SHADER_H
#define GYRO_SHADER_H

#include "gyro_defines.h"
#include "gyro_matrix_api.h"

#define GYRO_VIDEO_LIGHT_SOURCES_MAX 4

#ifdef __cplusplus
extern "C" {
#endif

struct _GYColor4;
struct _GYTexture;
struct _GYCamera;

typedef enum _GY_CUBE_MAP_FACE {
	GY_CUBE_MAP_RIGHT,
	GY_CUBE_MAP_LEFT,
	GY_CUBE_MAP_BOTTOM,
	GY_CUBE_MAP_TOP,
	GY_CUBE_MAP_BACK,
	GY_CUBE_MAP_FRONT,
	GY_CUBE_MAP_FACE_MAX
} GY_CUBE_MAP_FACE;

typedef struct _GYLightProperties {
	GYVector4	ambientColor;
	GYVector4	diffuseColor;
	GYVector4	specularColor;
	GYVector3	attenuationFactors;
	float		spotExponent;
	float		_radius;
} GYLightProperties;

typedef enum _GY_LIGHT_TYPE {
	GY_LIGHT_DIRECTIONAL,
	GY_LIGHT_SPOT,
	GY_LIGHT_POINT
} GY_LIGHT_TYPE;

typedef struct _GYLightSource {
	//GYMatrix4					projViewMat;
	GYVector4					position;					//position.w = 0.0f for directional light
	GYVector3					direction;
	float						spotCutoffAngle;
	GY_LIGHT_TYPE				type;
	GYLightProperties*			properties;
	struct _GYCamera*			camera;
	void*						_data;
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

void gyVidLightSourceAdd(GYLightSource* const light);
void gyVidLightSourceRemove(const GYLightSource* const light);
void gyVidLightSourceReplace(const GYLightSource*const old, GYLightSource*const next);
void gyVidLightSourceSetEnable(const int enabled);
void gyVidLightSourceAmbientSetEnable(const int enabled);
void gyVidLightSourceDiffuseSetEnable(const int enabled);
void gyVidLightSourceSpecularSetEnable(const int enabled);
void gyVidLightingSetEnable(const int enabled);

void gyVidMaterialBind(const GYMaterial*const material);

void gyVidNormalMapBind(const struct _GYTexture* const tex);
void gyVidSpecularMapBind(const struct _GYTexture * const tex);

void gyVidSetGlobalAmbience(const struct _GYVector4*const color);

//Shadow Multipass Rendering
int gyVidBeginNextShadowPass(void);


//Shader manipulation
void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program);
void gyVidSetDefaultShaderProgram();

void gyVidLightSourceInit(GYLightSource* const light, const GY_LIGHT_TYPE type);
void gyVidLightSourceUninit(GYLightSource* const light);
void gyVidLightSourceBeginShadowPass(GYLightSource* const light, const GY_CUBE_MAP_FACE face);
void gyVidLightSourceEndShadowPasses(void);
void gyVidLightSourceBeginShadowPasses(void);

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
