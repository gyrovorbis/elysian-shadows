#include <gyro_file_api.h>
#include <stdio.h>

#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>
#include <gyro_system_api.h>

#ifdef GL_ES_VERSION_3_0
#   define GY_GL_VERSION_STR "#version 300 es"
#else
#ifdef _WIN32
#   define GY_GL_VERSION_STR "#version 330\n\r"
#else
#   define GY_GL_VERSION_STR "#version 330\n"
#endif
#endif


GLenum glalTextureMapTexUnits[GLAL_TEXTURE_MAP_MAX] = {
    GL_TEXTURE0,
    GL_TEXTURE1,
    GL_TEXTURE2,
    GL_TEXTURE3,
    GL_TEXTURE4,
    GL_TEXTURE5,
    GL_TEXTURE6,
    GL_TEXTURE7,
    GL_TEXTURE8,
    GL_TEXTURE9,
    GL_TEXTURE10,
    GL_TEXTURE11
};

int glalTextureMapUniforms[GLAL_TEXTURE_MAP_MAX] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

const GYMatrix4 glalShadowTexCoordBiasMat = {
    { 0.5f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.5f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.5f, 0.0f },
    { 0.5f, 0.5f, 0.5f, 1.0f },
};

// ========= DEFAULT UNIFORM STATE VALUES =======
static const GYMaterial _defaultMaterial = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    1.0f
};
/*
static GYLightProperties _emptyLightProperties = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    1.0f,
    200.0f
};*/
/*
const static GYLightSource _emptyLight = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f },
    360.0f,
    GY_LIGHT_DIRECTIONAL,
    &_emptyLightProperties,
    NULL,
    NULL
};

const static GYVector3 _defaultNormal = {
  0.0f, 0.0f, 1.0f
};*/

GYMatrix4 glalPointShadowFaceMatrices[GY_CUBE_MAP_FACE_MAX];


// ====== UNIFORM STATE DATA =======
static const GYMaterial*    _currentMaterial                               = &_defaultMaterial;
static GYMaterial           _materialCache;
static GYVector4			_globalAmbience									= { 0.0f, 0.0f, 0.0f, 1.0f };
static const GYTexture*		_normalMap										= NULL;
static const GYTexture*		_specularMap									= NULL;
GYVector3					glalCamWorldPos									= { 0.0f, 0.0f, 0.0f };

// ==== SHARED EXTERN VARIABLES ======
int                         glalTextureMappingEnabled                       = 1;
int                         glalNormalMappingEnabled                        = 1;
int                         glalSpecularMappingEnabled                      = 1;
int                         glalTextureMapActive                            = 0;
int                         glalNormalMapActive                             = 0;
int                         glalSpecularMapActive                           = 0;

// ==== INTERNAL STATE ===
GYShaderProgram*			glalCurShaderProg                               = NULL;
_LightingShaderProgram		glalLightingShader;
_LightingShaderProgram		glalShadowMapShader;
_LightingShaderProgram*		glalCurLightShader                              = &glalLightingShader;

// ====== PUBLIC LIGHTING API =======

void gyVidSetCurrentShaderProgram(struct GYShaderProgram*const program) {
    glalCurShaderProg = program;
    if(program) glUseProgram(program->program);
}

void gyVidSetDefaultShaderProgram(void) {
    gyVidSetCurrentShaderProgram(glalLightingShader.program);
}

void gyVidDbgTexMappingEnabled(const int enabled) { glalTextureMappingEnabled = enabled; }
void gyVidDbgNormalMappingEnabled(const int enabled) { glalNormalMappingEnabled = enabled; }
void gyVidDbgSpeculaMappingEnabled(const int enabled) { glalSpecularMappingEnabled = enabled; }

void gyVidDbgTexMapUniform(const int unit) {
    glalTextureMapUniforms[GLAL_TEXTURE_MAP] = unit;
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_MAP], unit);
    ++glalUniformChangeCount;
}

void gyVidSetCamWorldPos(const GYVector3*const pos) {
    if(pos->x == glalCamWorldPos.x && pos->y == glalCamWorldPos.y && pos->z == glalCamWorldPos.z) return;
    gyVidVertexBatchFlush();
    glalCamWorldPos = *pos;
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_CAM_WORLD_POS], 1, (float*)&glalCamWorldPos);
    ++glalUniformChangeCount;
}

void gyVidNormalMapBind(const GYTexture * const tex) {
    if(!glalLightingEnabled || gyVidShadowPass()) return;
    static int wasActive = 0;
    int active = 1;

    if(tex != _normalMap) {
        gyVidVertexBatchFlush();
        glActiveTexture(glalTextureMapTexUnits[GLAL_NORMAL_MAP]);
        if(tex && tex->_data) {
           // if(!wasActive) glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
        } else {
            active = 0;
            //if(wasActive) glDisable(GL_TEXTURE_2D);
        }
        ++glalTextureChangeCount;

        glalNormalMapActive = active && glalNormalMappingEnabled;

        if(glalNormalMapActive != wasActive) {
            glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ENABLED], glalNormalMapActive);
            ++glalUniformChangeCount;


            if(glalTextureMapUniforms[GLAL_TEXTURE_MAP] == glalTextureMapUniforms[GLAL_NORMAL_MAP]) {
                glalTextureMapActive = glalNormalMapActive;
                glalCurTex = tex;
                glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], glalNormalMapActive);
                ++glalUniformChangeCount;
            }

        }

        _normalMap = tex;
        wasActive = glalNormalMapActive;
        glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);

        const float zFactor = glalOrientNormalMapZFactor(glalRectTexOrient);
        glalVidNormalMapZFactorUpdateUniform(zFactor);


    }

}

void gyVidSpecularMapBind(const GYTexture * const tex) {
    if(!glalLightingEnabled || gyVidShadowPass()) return;
    static int wasActive = 0;
    int active = 1;

    if(tex != _specularMap) {
        gyVidVertexBatchFlush();
        glActiveTexture(glalTextureMapTexUnits[GLAL_SPECULAR_MAP]);
        if(tex && tex->_data) {
         //   if(!wasActive) glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
        } else {
            if(wasActive) active = 0;
         //   glDisable(GL_TEXTURE_2D);
        }
        ++glalTextureChangeCount;

        glalSpecularMapActive = active && glalSpecularMappingEnabled;

        if(glalSpecularMapActive != wasActive) {
            glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_SPECULAR_MAP_ENABLED], glalSpecularMapActive);
            ++glalUniformChangeCount;
            if(glalTextureMapUniforms[GLAL_TEXTURE_MAP] == glalTextureMapUniforms[GLAL_SPECULAR_MAP]) {
                glalTextureMapActive = glalSpecularMapActive;
                glalCurTex = tex;
                glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], glalSpecularMapActive);
                ++glalUniformChangeCount;
            }
        }

        _specularMap = tex;
        glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);

        wasActive = glalSpecularMapActive;
    }
}


void glalLightingInit(void) {

    //Cache cube shadow map lookat matrices
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_RIGHT], 0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_LEFT], 0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_BOTTOM], 0.0, 0.0, 0.0,  0.0, -1.0, 0.0,  0.0,0.0, -1.0); // +Y
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_TOP], 0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0,0.0, 1.0); // -Y
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_BACK], 0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
    gyVidLookAtMat(glalPointShadowFaceMatrices[GY_CUBE_MAP_FRONT], 0.0, 0.0, 0.0,  0.0, 0.0, -1.0,  0.0,1.0, 0.0); // -Z
}

void gyVidMaterialBind(const GYMaterial*const material) {
    if(!glalLightingEnabled || gyVidShadowPass()) return;

    int flushed = 0;
    if(!material) gyVidMaterialBind(&_defaultMaterial);
    else {

        //Check for individual state changes
        if(!_currentMaterial || material->shininess != _materialCache.shininess) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform1f(glalCurLightShader->uniformLoc[GLAL_SHADER_MATERIAL_SHININESS], material->shininess);
            ++glalUniformChangeCount;
        }

        if(!_currentMaterial || !gyVec4Compare(&material->ambientColor, &_materialCache.ambientColor)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_MATERIAL_AMBIENT_COLOR], 1, (float*)&material->ambientColor);
            ++glalUniformChangeCount;
        }

        if(!_currentMaterial || !gyVec4Compare(&material->diffuseColor, &_materialCache.diffuseColor)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_MATERIAL_DIFFUSE_COLOR], 1, (float*)&material->diffuseColor);
            ++glalUniformChangeCount;
        }

        if(!_currentMaterial || !gyVec4Compare(&material->specularColor, &_materialCache.specularColor)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_MATERIAL_SPECULAR_COLOR], 1, (float*)&material->specularColor);
            ++glalUniformChangeCount;
        }

        if(!_currentMaterial || !gyVec4Compare(&material->emissiveColor, &_materialCache.emissiveColor)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_MATERIAL_EMISSIVE_COLOR], 1, (float*)&material->emissiveColor);
            ++glalUniformChangeCount;
        }

        _currentMaterial = material;
        memcpy(&_materialCache, _currentMaterial, sizeof(GYMaterial));
    }
}

void gyVidSetGlobalAmbience(const GYVector4*const color) {
    if(gyVidShadowPass()) return;
    if(!(color->x == _globalAmbience.x && color->y == _globalAmbience.y && color->z == _globalAmbience.z)) {
        _globalAmbience = *color;
        glUniform4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_AMBIENCE], 1, (float*)color);
        ++glalUniformChangeCount;
    }
}

//======= INTERNAL QUERY FUNCTIONS FOR DEFAULT SHADER =======

static inline int _protectedUniformQuery(const GYShaderProgram* prog, const char* const name, const unsigned canFail) {
    int loc = glGetUniformLocation(prog->program, name);
    if(!canFail && loc == -1) {
        _gyLog(GY_DEBUG_WARNING, "Query for shader uniform %s failed!", name);
    }
    return loc;
}

static inline int _protectedAttribQuery(const GYShaderProgram* prog, const char* const name, const unsigned canFail) {
    int loc = glGetAttribLocation(prog->program, name);
    if(!canFail && loc == -1) {
        _gyLog(GY_DEBUG_WARNING, "Query for shader attribute %s failed!", name);
    }
    return loc;
}


static void _queryLightingShader(_LightingShaderProgram*const shader, const unsigned light) {
    (void)light;
    char buff[100];

    if(!shader->program) return;

    //SHARED SHADER ATTRIBS/UNIFORMS
    shader->attribLoc[GLAL_SHADER_POS]						= _protectedAttribQuery(shader->program, "aPosition", 0);
    shader->uniformLoc[GLAL_SHADER_PROJ_VIEW_MAT]			= _protectedUniformQuery(shader->program, "uProjViewMat", 0);

    shader->attribLoc[GLAL_SHADER_TEX_COORD]					= _protectedAttribQuery(shader->program, "aTexCoord", 0);
    shader->uniformLoc[GLAL_SHADER_TEXTURE_MAP]              = _protectedUniformQuery(shader->program, "uTexture", 0);
    shader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED]			= _protectedUniformQuery(shader->program, "uTextureEnabled", 0);

    //Everything beyond this point is for the light shader only
    shader->attribLoc[GLAL_SHADER_NORMAL]						= _protectedAttribQuery(shader->program, "aNormal", !light);
    shader->attribLoc[GLAL_SHADER_TANGENT]						= _protectedAttribQuery(shader->program, "aTangent", !light);
    shader->attribLoc[GLAL_SHADER_COLOR]						= _protectedAttribQuery(shader->program, "aColor", !light);

    //if(!light) return; //the rest is lighting only

    //poll for global uniform locations
    shader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ZFACTOR]             = _protectedUniformQuery(shader->program, "uNormalMapZFactor", !light);
    shader->uniformLoc[GLAL_SHADER_CAM_WORLD_POS]			= _protectedUniformQuery(shader->program, "uCamWorldPos", !light);
    shader->uniformLoc[GLAL_SHADER_AMBIENCE]					= _protectedUniformQuery(shader->program, "uAmbience", !light);
    //shader->uniformLoc[GLAL_SHADER_LIGHTING_ENABLED]			= _protectedUniformQuery(shader->program, "uLightingEnabled");
    shader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ENABLED]		= _protectedUniformQuery(shader->program, "uNormalMapEnabled", !light);
    shader->uniformLoc[GLAL_SHADER_NORMAL_MAP]				= _protectedUniformQuery(shader->program, "uNormalMap", !light);
    shader->uniformLoc[GLAL_SHADER_SPECULAR_MAP_ENABLED]		= _protectedUniformQuery(shader->program, "uSpecularMapEnabled", !light);
    shader->uniformLoc[GLAL_SHADER_SPECULAR_MAP]				= _protectedUniformQuery(shader->program, "uSpecularMap", !light);
    shader->uniformLoc[GLAL_SHADER_SHADOW_CUBE_MAP_PROJ_MAT] = _protectedUniformQuery(shader->program, "uShadowCubeMapProjMat", !light);

    //poll for GYMaterial uniform locations
    shader->uniformLoc[GLAL_SHADER_MATERIAL_SHININESS]		= _protectedUniformQuery(shader->program, "uMaterial.shininess", !light);
    shader->uniformLoc[GLAL_SHADER_MATERIAL_AMBIENT_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.ambientColor", !light);
    shader->uniformLoc[GLAL_SHADER_MATERIAL_DIFFUSE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.diffuseColor", !light);
    shader->uniformLoc[GLAL_SHADER_MATERIAL_SPECULAR_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.specularColor", !light);
    shader->uniformLoc[GLAL_SHADER_MATERIAL_EMISSIVE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.emissiveColor", !light);

    //poll for GYLightSource uniform locations
    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {

        sprintf(buff, "uLights[%d].position", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_POS]							= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].ambientColor", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_AMBIENT_COLOR]					= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].diffuseColor", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_DIFFUSE_COLOR]					= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].specularColor", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SPECULAR_COLOR]				= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].direction", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_DIRECTION]						= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].attenuationFactors", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_ATTENUATION_FACTORS]			= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].spotExponent", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SPOT_EXPONENT]					= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLights[%d].spotCutoffAngle", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SPOT_CUTDBG_OFF_ANGLE]				= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLightsEnabled[%d]", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_ENABLED]						= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uShadowMaps[%d]", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SHADOW_MAP]					= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uShadowCubeMaps[%d]", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SHADOW_CUBE_MAP]				= _protectedUniformQuery(shader->program, buff, !light);

        sprintf(buff, "uLightProjViewMats[%d]", i);
        shader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_PROJ_VIEW_MAT]					= _protectedUniformQuery(shader->program, buff, !light);

    }
}


// =========== PUBLIC INTERNAL FUNCTIONS ==========
void glalLoadDefaultShaders(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Loading default shaders.");
    _gyPush();

    GYShader* vertShader = gyShaderCreate("/assets/shaders/pixelLighting.vsh");
    GYShader* fragShader = gyShaderCreate("/assets/shaders/pixelLighting.fsh");
    glalLightingShader.program = gyShaderProgramCreate(vertShader, fragShader);
    _queryLightingShader(&glalLightingShader, 1);

    vertShader = gyShaderCreate("/assets/shaders/shadowMap.vsh");
    fragShader = gyShaderCreate("/assets/shaders/shadowMap.fsh");
    glalShadowMapShader.program = gyShaderProgramCreate(vertShader, fragShader);
    _queryLightingShader(&glalShadowMapShader, 0);

    _gyPop(1);
}

int gyShadersReloadAll(void) {
    int success = 1;
    success &= gyShaderProgramDestroy(glalLightingShader.program);
    success &= gyShaderProgramDestroy(glalShadowMapShader.program);
    success &= gyShaderDestroy(glalLightingShader.program->vertShader);
    success &= gyShaderDestroy(glalLightingShader.program->fragShader);
    success &= gyShaderDestroy(glalShadowMapShader.program->vertShader);
    success &= gyShaderDestroy(glalShadowMapShader.program->fragShader);
    glalLoadDefaultShaders();
    gyVidSetDefaultShaderProgram();
    glalDefaultShadersLoadUniforms();
    return success;
}

void glalDefaultShadersLoadUniforms(void) {
    GYLightSource* sources[GYRO_VIDEO_LIGHT_SOURCES_MAX];
    int count = glalLightSourceCount;
    const GYMaterial* material = _currentMaterial;
    _currentMaterial = NULL;

    glUniformMatrix4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)glalCurCamera->projViewMat);
//	glUniform4fv(curLightShader->uniformLoc[SHADER_CAM_POS], 1, (float*)&viewPosition);
    gyVidMaterialBind(material);

    memcpy(sources, glalLightSources, sizeof(GYLightSource*) * count);

    //Gid rid of them and readd
    gyVidLightSourceClear();
    for(int i = 0; i < count; ++i) {
        gyVidLightSourceAdd(sources[i]);
    }

    gyVidSetGlobalAmbience(&_globalAmbience);
    gyVidLightingSetEnable(glalLightingEnabled);

    glUniform1f(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ZFACTOR], glalNormalMapZFactor);
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_CAM_WORLD_POS], 1, (float*)&glalCamWorldPos);

    //set normal map to use texture bank 1
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_MAP], glalTextureMapUniforms[GLAL_TEXTURE_MAP]);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL_MAP], glalTextureMapUniforms[GLAL_NORMAL_MAP]);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_SPECULAR_MAP], glalTextureMapUniforms[GLAL_SPECULAR_MAP]);

    //next texture banks are used for shadow maps for each light
    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
        glUniform1i(glalCurLightShader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SHADOW_MAP], 3+i);
        glUniform1i(glalCurLightShader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_SHADOW_CUBE_MAP], 7+i);
    }

    //Do these not need to be physically resubmitted?
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], glalTextureMapActive);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ENABLED], glalNormalMapActive);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_SPECULAR_MAP_ENABLED], glalSpecularMapActive);

    GYMatrix4 projMat;
    GYMatrix4 finalMat;
    glalGetPointLightProjectionMatrix(projMat, NULL);
    gyMatMultMat(finalMat, glalShadowTexCoordBiasMat, (const float (*)[4])projMat);
    glUniformMatrix4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_SHADOW_CUBE_MAP_PROJ_MAT], 1, GL_FALSE, (float*)projMat); //NO TEX COORD DBG_OFFSET? WHAT THE FUCK!?

    glalUniformChangeCount += 14;

    //Initialize internal data for shadow shader
    glalLightingInit();

}

// ======== SHADER ALLOCATION AND MANAGEMENT ========

int _gyVidShaderCreate(GYShader* shader) {
    char path[GYRO_PATH_MAX_SIZE];
    GLint compiled;
    const char* extension = NULL;
    const char* token = NULL;
    GYFile *file = NULL;
    size_t charCount = 0;
    char *programSource = NULL;

    //don't fuck it up by tokenizing
    strcpy(path, shader->path);
    strtok(path, ".");

    while((token = strtok(NULL, "."))) {
        extension = token;
    }

    _gyLog(GY_DEBUG_VERBOSE, "Compiling shader: %s", shader->path);
    _gyPush();

    if(strcmp(extension, "vsh") == 0) {
        shader->type = GL_VERTEX_SHADER;
    } else if(strcmp(extension, "fsh") == 0) {
        shader->type = GL_FRAGMENT_SHADER;
    } else {
        _gyLog(GY_DEBUG_ERROR, "Unrecognized shader extension: %s", extension);
        _gyPop(1);
        return 0;
    }

    shader->shader = glCreateShader(shader->type);
    if(!shader->shader)  {
        _gyLog(GY_DEBUG_ERROR, "Could not generate shader", shader->path);
        _gyPop(1);
        return 0;
    }

    if(!gyFileOpen(shader->path, "r", &file)) {
        _gyLog(GY_DEBUG_ERROR, "Could not open shader", shader->path);
        _gyPop(1);
        return 0;
    }

    gyFileLength(file, &charCount);
    programSource = malloc(sizeof(char) * charCount + 1);
    memset(programSource, 0, sizeof(char)*charCount+1);
    gyFileRead(file, programSource, charCount, NULL);
    //programSource[charCount-1] = '\0';
    programSource[charCount] = '\0';
    gyFileClose(&file);

    GLchar* sources[2] = {
        GY_GL_VERSION_STR,
        programSource
    };

    GLint sourceLengths[2] = {
        strlen(GY_GL_VERSION_STR),
        charCount
    };

    glShaderSource(shader->shader, 2, sources, sourceLengths);
    glCompileShader(shader->shader);
    free(programSource);

    glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader->shader, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1) {
            char* infoLog = malloc(sizeof(char)*infoLen);

            glGetShaderInfoLog(shader->shader, infoLen, NULL, infoLog);
            _gyLog(GY_DEBUG_ERROR, "Error compiling shader: %s", infoLog);
            free(infoLog);
        } else {
            _gyLog(GY_DEBUG_ERROR, "Unknown compilation error");
        }

        glDeleteShader(shader->shader);
        _gyPop(1);
        return 0;
    }

    _gyPop(1);
    return 1;
}

int _gyVidShaderDestroy(GYShader* shader) {
    glDeleteShader(shader->shader);
    return 1;
}

int _gyVidShaderProgramCreate(GYShaderProgram* program) {
    GLint linked;

    _gyLog(GY_DEBUG_VERBOSE, "Linking shader program");
    _gyPush();

    program->program = glCreateProgram();
    if(!program->program) {
        _gyLog(GY_DEBUG_ERROR, "\tCould not generate program!");
        _gyPop(1);
        return 0;
    }

    if(program->vertShader) {
        glAttachShader(program->program, program->vertShader->shader);
    } else {
        _gyLog(GY_DEBUG_ERROR, "\tNo vertex shader object to attach!");
        _gyPop(1);
        return 0;
    }
    if(program->fragShader) {
        glAttachShader(program->program, program->fragShader->shader);
    } else {
        _gyLog(GY_DEBUG_ERROR, "\tNo fragment shader object to attach!");
        _gyPop(1);
        return 0;
    }

    glLinkProgram(program->program);

    glGetProgramiv(program->program, GL_LINK_STATUS, &linked);

    if(!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program->program, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(program->program, infoLen, NULL, infoLog);
            _gyLog(GY_DEBUG_ERROR, "Could not link program: %s", infoLog);
            free(infoLog);

        } else {
            _gyLog(GY_DEBUG_ERROR, "Could not link program for unknown reasons!");
        }

        _gyPop(1);
        glDeleteProgram(program->program);
        return 0;
    }

    glDetachShader(program->program, program->vertShader->shader);
    glDetachShader(program->program, program->fragShader->shader);

    _gyPop(1);
    return 1;
}

int _gyVidShaderProgramDestroy(GYShaderProgram* program) {
    glDeleteProgram(program->program);
    return 1;
}

