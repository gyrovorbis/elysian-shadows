/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_SHADER_API_H
#define GYRO_SHADER_API_H

#include "gyro_defines.h"
#include "gyro_matrix_api.h"

//#define GYRO_VIDEO_LIGHT_SOURCES_MAX 10
#define GYRO_VIDEO_LIGHT_SOURCES_MAX 4

#ifdef __cplusplus
extern "C" {
#endif

struct GYColor4;
struct GYTexture;
struct GYCamera;

typedef enum GY_CUBE_MAP_FACE {
    GY_CUBE_MAP_RIGHT,
    GY_CUBE_MAP_LEFT,
    GY_CUBE_MAP_BOTTOM,
    GY_CUBE_MAP_TOP,
    GY_CUBE_MAP_BACK,
    GY_CUBE_MAP_FRONT,
    GY_CUBE_MAP_FACE_MAX
} GY_CUBE_MAP_FACE;

typedef struct GYLightProperties {
    GYVector4	ambientColor;
    GYVector4	diffuseColor;
    GYVector4	specularColor;
    GYVector3	attenuationFactors;
    float		spotExponent;
    float		_radius;
} GYLightProperties;

typedef enum GY_LIGHT_TYPE {
    GY_LIGHT_DIRECTIONAL,
    GY_LIGHT_SPOT,
    GY_LIGHT_POINT
} GY_LIGHT_TYPE;

typedef struct GYLightSource {
    //GYMatrix4					projViewMat;
    GYVector4					position;					//position.w = 0.0f for directional light
    GYVector3					direction;
    float						spotCutoffAngle;
    GY_LIGHT_TYPE				type;
    GYLightProperties*			properties;
    struct GYCamera*			camera;
    void*						_data;
} GYLightSource;

typedef struct GYMaterial {
    GYVector4	ambientColor;
    GYVector4	diffuseColor;
    GYVector4	specularColor;
    GYVector4	emissiveColor;
    float		shininess;
} GYMaterial;

typedef struct GYShader {
    char	path[GYRO_PATH_MAX_SIZE];
    int		type;
    int		shader;
} GYShader;

typedef struct GYShaderProgram {
    char		path[GYRO_PATH_MAX_SIZE];
    GYShader*	vertShader;
    GYShader*	fragShader;
    int 		program;
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

void gyVidNormalMapBind(const struct GYTexture* const tex);
void gyVidSpecularMapBind(const struct GYTexture * const tex);

void gyVidSetGlobalAmbience(const struct GYVector4*const color);

//Shadow Multipass Rendering
int gyVidBeginNextShadowPass(void);
int gyVidShadowPass(void);

//Shader manipulation
void gyVidSetCurrentShaderProgram(struct GYShaderProgram*const program);
void gyVidSetDefaultShaderProgram(void);
//void gyVidBindDefaultVertexArray(void);

void gyVidLightSourceInit(GYLightSource* const light, const GY_LIGHT_TYPE type);
void gyVidLightSourceUninit(GYLightSource* const light);
void gyVidLightSourceBeginShadowPass(GYLightSource* const light, const GY_CUBE_MAP_FACE face);
void gyVidLightSourceEndShadowPasses(void);
void gyVidLightSourceBeginShadowPasses(void);

//Bloom post-processing effect API
void gyVidApplyBloom(void);

//Shader management
GYShader*           gyShaderCreate(const char*const path);
int                 gyShaderDestroy(GYShader* shader);
int                 gyShaderProgramDestroy(GYShaderProgram* program);
GYShaderProgram*    gyShaderProgramCreate(GYShader* vertShader, GYShader* fragShader);
int                 gyShadersReloadAll(void);
void                gyShaderWalk(void);

//Internal
int _gyVidShaderCreate(GYShader* shader);
int _gyVidShaderDestroy(GYShader* shader);
int _gyVidShaderProgramCreate(GYShaderProgram* program);
int _gyVidShaderProgramDestroy(GYShaderProgram* program);
int _gyVidShaderDefaultProgramLoadUniforms(void);

#ifdef GYRO_SHADER_PLATFORM_INCLUDE
#	include GYRO_SHADER_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif


#endif // GYRO_SHADER_H
