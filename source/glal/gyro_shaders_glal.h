#ifndef GYRO_SHADERS_GLAL_H
#define GYRO_SHADERS_GLAL_H

#include <glal/gyro_glal.h>
#include <gyro_shader_api.h>

#define GLAL_SHADOW_MAP_SIZE	1024

#ifdef __cplusplus
extern "C" {
#endif

struct _GYShaderProgram;

typedef enum __SHADER_ATTRIB {
	SHADER_POS,
	SHADER_COLOR,
	SHADER_TEX_COORD,
	SHADER_ATTRIB_MAX
} _SHADER_ATTRIB;

typedef enum __SHADER_UNIFORM {
	SHADER_TEXTURE_ENABLED,
	SHADER_NORMAL,
	SHADER_TANGENT,
	SHADER_PROJ_VIEW_MAT,
	SHADER_AMBIENCE,
	SHADER_MATERIAL_SHININESS,
	SHADER_LIGHTING_ENABLED,
	SHADER_MATERIAL_AMBIENT_COLOR,
	SHADER_MATERIAL_DIFFUSE_COLOR,
	SHADER_MATERIAL_SPECULAR_COLOR,
	SHADER_MATERIAL_EMISSIVE_COLOR,
	SHADER_NORMAL_MAP_ENABLED,
	SHADER_NORMAL_MAP,
	SHADER_SPECULAR_MAP_ENABLED,
	SHADER_SPECULAR_MAP,
	SHADER_SHADOW_CUBE_MAP_PROJ_MAT,
	SHADER_UNIFORM_MAX
} _SHADER_UNIFORM;

typedef enum __SHADER_LIGHT_UNIFORM_FIELDS {
	SHADER_LIGHT_POS,
	SHADER_LIGHT_AMBIENT_COLOR,
	SHADER_LIGHT_DIFFUSE_COLOR,
	SHADER_LIGHT_SPECULAR_COLOR,
	SHADER_LIGHT_DIRECTION,
	SHADER_LIGHT_ATTENUATION_FACTORS,
	SHADER_LIGHT_SPOT_EXPONENT,
	SHADER_LIGHT_SPOT_CUTOFF_ANGLE,
	SHADER_LIGHT_COMPUTE_DISTANCE_ATTENUATION,
	SHADER_LIGHT_ENABLED,
	SHADER_LIGHT_SHADOW_MAP,
	SHADER_LIGHT_SHADOW_CUBE_MAP,
	SHADER_LIGHT_PROJ_VIEW_MAT,
	SHADER_LIGHT_UNIFORMS_MAX
} _SHADER_LIGHT_UNIFORM_FIELDS;

typedef struct __LightingShaderProgram {
	struct _GYShaderProgram*	program;
	GLint						attribLoc[SHADER_ATTRIB_MAX];
	GLint						uniformLoc[SHADER_UNIFORM_MAX];
	GLint						lightsUniformLoc[GYRO_VIDEO_LIGHT_SOURCES_MAX][SHADER_LIGHT_UNIFORMS_MAX];
} _LightingShaderProgram;

typedef struct __LightInternals {
	GLuint fbo;
	GLuint shadowMap;
} _LightInternals;

extern GYShaderProgram*				_curShaderProg;
extern _LightingShaderProgram		_lightingShader;
extern _LightingShaderProgram		_shadowMapShader;
extern _LightingShaderProgram*		curLightShader;
extern const GYMatrix4				shadowTexCoordBiasMat;
extern GYMatrix4					pointShadowFaceMatrices[GY_CUBE_MAP_FACE_MAX];

extern GYLightSource*		lightSources[GYRO_VIDEO_LIGHT_SOURCES_MAX];
extern unsigned				lightSourceCount;
extern int					lightingEnabled;
extern int					lightsEnabled[GYRO_VIDEO_LIGHT_SOURCES_MAX];

void glalLoadDefaultShaders(void);
void glalGetLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc);

#ifdef __cplusplus
}
#endif


#endif // GYRO_SHADERS_GLAL_H
