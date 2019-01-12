#ifndef GYRO_SHADERS_GL_H
#define GYRO_SHADERS_GL_H

#include <libGyro/gyro_shader_api.h>
#include <gl/gyro_gl.h>

#define GLAL_SHADOW_MAP_SIZE        1024
#define GLAL_SHADOW_MAP_ARRAY_SIZE  10

#ifdef __cplusplus
extern "C" {
#endif

typedef enum __GLAL_TEXTURE_UNITS {
    GLAL_TEXTURE_MAP,
    GLAL_NORMAL_MAP,
    GLAL_SPECULAR_MAP,
    GLAL_SPOT_SHADOW_MAP,
    GLAL_DIRECTIONAL_SHADOW_MAP,
    GLAL_POINT_SHADOW_CUBE_MAP,
    GLAL_TEMP_MAP,
    GLAL_TEXTURE_MAP_MAX
} GLAL_TEXTURE_UNITS;

struct _GYShaderProgram;

typedef enum __GLAL_SHADER_ATTRIB {
    GLAL_SHADER_POS,
    GLAL_SHADER_COLOR,
    GLAL_SHADER_TEX_COORD,
    GLAL_SHADER_NORMAL,
    GLAL_SHADER_TANGENT,
    GLAL_SHADER_NORMAL_Z,
    GLAL_SHADER_ATTRIB_MAX
} _GLAL_SHADER_ATTRIB;

typedef enum __GLAL_SHADER_UNIFORM {
    GLAL_SHADER_TEXTURE_MAP,
    GLAL_SHADER_TEXTURE_ENABLED,
    GLAL_SHADER_CAM_WORLD_POS,
    GLAL_SHADER_PROJ_VIEW_MAT,
    GLAL_SHADER_AMBIENCE,
    GLAL_SHADER_MATERIAL_SHININESS,
    GLAL_SHADER_LIGHTING_ENABLED,
    GLAL_SHADER_MATERIAL_AMBIENT_COLOR,
    GLAL_SHADER_MATERIAL_DIFFUSE_COLOR,
    GLAL_SHADER_MATERIAL_SPECULAR_COLOR,
    GLAL_SHADER_MATERIAL_EMISSIVE_COLOR,
    GLAL_SHADER_NORMAL_MAP_ENABLED,
    GLAL_SHADER_NORMAL_MAP,
    GLAL_SHADER_SPECULAR_MAP_ENABLED,
    GLAL_SHADER_SPECULAR_MAP,
    GLAL_SHADER_DIRECTIONAL_SHADOW_MAPS,
    GLAL_SHADER_SPOT_SHADOW_MAPS,
    GLAL_SHADER_POINT_SHADOW_CUBE_MAPS,
    GLAL_SHADER_SHADOW_CUBE_MAP_PROJ_MAT,
    GLAL_SHADER_HDR_EXPOSURE,
    GLAL_SHADER_UNIFORM_MAX
} _GLAL_SHADER_UNIFORM;

typedef enum __GLAL_SHADER_LIGHT_UNIFORM_FIELDS {
    GLAL_SHADER_LIGHT_POS,
    //GLAL_SHADER_LIGHT_AMBIENT_COLOR,
    GLAL_SHADER_LIGHT_DIFFUSE_COLOR,
    GLAL_SHADER_LIGHT_SPECULAR_COLOR,
    GLAL_SHADER_LIGHT_DIRECTION,
    GLAL_SHADER_LIGHT_ATTENUATION_FACTORS,
    GLAL_SHADER_LIGHT_SPOT_EXPONENT,
    GLAL_SHADER_LIGHT_SPOT_CUTLOG_OFF_ANGLE,
    GLAL_SHADER_LIGHT_COMPUTE_DISTANCE_ATTENUATION,
    GLAL_SHADER_LIGHT_ENABLED,
    GLAL_SHADER_LIGHT_SHADOW_MAP,
    GLAL_SHADER_LIGHT_SHADOW_CUBE_MAP,
    GLAL_SHADER_LIGHT_PROJ_VIEW_MAT,
    GLAL_SHADER_LIGHT_UNIFORMS_MAX
} _GLAL_SHADER_LIGHT_UNIFORM_FIELDS;

typedef struct __LightingShaderProgram {
    GYShaderProgram*            program;
    GLint						attribLoc[GLAL_SHADER_ATTRIB_MAX];
    GLint						uniformLoc[GLAL_SHADER_UNIFORM_MAX];
    GLint						lightsUniformLoc[GYRO_VIDEO_LIGHT_SOURCES_MAX][GLAL_SHADER_LIGHT_UNIFORMS_MAX];
    GLuint                      lightingUBOIndex;
} _LightingShaderProgram;

typedef struct __LightInternals {
    GLuint fbo;
    GLuint shadowMap;
    int shadowMapIdx;
} _LightInternals;

typedef struct LightingUBOLightSource {
    GYMatrix4       projViewMat;
    GYVector4       position;
    //GYVector4       ambientColor;
    GYVector4       diffuseColor;
    GYVector4       specularColor;
    GYVector3       direction;
    float           pad1;
    GYVector3       attenuationFactors;
    float           spotExponent;
    float           spotCutoffAngle;
    GLint           shadowMapIdx;
} LightingUBOLightSource;

typedef struct LightingUBOData {
#if 1
    GLint                   count;
    LightingUBOLightSource  sources[32];
#else
    float color[32*32*3];
#endif
} LightingUBOData;

typedef struct LightSourceUniforms {
    GYMatrix4       projViewMat;
    GYVector4       position;
    GYVector4       ambientColor;
    GYVector4       diffuseColor;
    GYVector4       specularColor;
    GYVector3       attenuationFactors;
    GYVector3       direction;
    float           spotExponent;
    float           spotCutoffAngle;
    GY_LIGHT_TYPE   type;
    int             valid;
    GLuint          texture;
} LightSourceUniforms;

extern GYShaderProgram*				glalCurShaderProg;
extern _LightingShaderProgram		glalLightingShader;
extern _LightingShaderProgram		glalShadowMapShader;
extern _LightingShaderProgram*		glalCurLightShader;
extern const GYMatrix4				glalShadowTexCoordBiasMat;
extern GYMatrix4                    glalPointShadowFaceMatrices[];
extern LightingUBOData              glalLightingUBOData;

extern GYLightSource*               glalLightSources[];
extern unsigned                     glalLightSourceCount;
extern int                          glalLightingEnabled;
extern int                          glalLightsEnabled[];
extern GLenum                       glalTextureMapTexUnits[];
extern int                          glalTextureMapUniforms[];
extern int                          glalTextureMapActive;
extern int                          glalTextureMappingEnabled;
extern int                          glalNormalMappingEnabled;
extern int                          glalSpecularMappingEnabled;
extern int                          glalNormalMapActive;
extern int                          glalSpecularMapActive;
extern GLuint                       glalLightingUBO;

int glalLoadDefaultShaders(void);
void glalDefaultShadersLoadUniforms(void);
void glalGetLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc);
void glalGetPointLightProjectionMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc);
int _protectedAttribQuery(const GYShaderProgram* prog, const char* const name, const unsigned canFail);
int _protectedUniformQuery(const GYShaderProgram* prog, const char* const name, const unsigned canFail);
void _updateLightingUbo(void);

#ifdef __cplusplus
}
#endif


#endif // GYRO_SHADERS_GLAL_H
