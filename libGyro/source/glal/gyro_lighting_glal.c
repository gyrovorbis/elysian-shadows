#include <gyro_video_api.h>
#include <gyro_shader_api.h>
#include <gyro_defines.h>
#include <glal/gyro_shaders_glal.h>
#include <glal/gyro_video_glal.h>
#include <gyro_system_api.h>
#include <stdio.h>


LightSourceUniforms _lightSourceUniforms[GYRO_VIDEO_LIGHT_SOURCES_MAX];
GYLightSource*		glalLightSources[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { NULL };
unsigned			glalLightSourceCount								= 0;
int					glalLightingEnabled									= 1;
int					glalLightsEnabled[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { 0 };
static int          _shadowPass                                         = 0;
static int          _firstShadowPass                                    = 0;

int gyVidShadowPass(void) {
    return _shadowPass;
}

void gyVidLightingSetEnable(const int enabled) {
    GYVector4 noAmbience = { 1.0f, 1.0f, 1.0f, 1.0f };
    glalLightingEnabled = enabled;

    if(glalLightingEnabled !=  enabled) {
        glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_LIGHTING_ENABLED], enabled);
        ++glalUniformChangeCount;

        if(!enabled) {
            gyVidSetGlobalAmbience(&noAmbience);
            gyVidNormalMapBind(NULL);
            gyVidSpecularMapBind(NULL);
            gyVidMaterialBind(NULL);
            gyVidLightSourceClear();
        }

        glalLightingEnabled = enabled;
    }
}

void gyVidLightSourceInit(GYLightSource* const light, const GY_LIGHT_TYPE type) {
    _LightInternals* internals = light->_data = malloc(sizeof(_LightInternals));

    glGenFramebuffers(1, &internals->fbo);
    glGenTextures(1, &internals->shadowMap);

    light->type = type;

    switch(type) {
    case GY_LIGHT_DIRECTIONAL:
        light->position.w = 0.0f;
        //light->spotCutoffAngle = 360.0f; //Shader needs this for correct texture coords
        //glEnable(GL_TEXTURE_2D);
        //glDisable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if !defined(ANDROID) /* TODO: FIXME */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      //  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
        //glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 1.0f);
#endif
        break;

    case GY_LIGHT_SPOT:
        //glEnable(GL_TEXTURE_2D);
        //glDisable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if !defined(ANDROID) /* TODO: FIXME */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      //  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
        //glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 1.0f);
#endif
        break;

    case GY_LIGHT_POINT:
        light->spotCutoffAngle = 360.0f;
       // glEnable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);
                glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
#if !defined(ANDROID) /* TODO: FIXME */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
       // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, 1.0f);
        //Does this REALLY have to have 6 faces!?
        for (unsigned i = 0; i < GY_CUBE_MAP_FACE_MAX; ++i) {
            //shadowDepth? lolz
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        }
#endif
      //  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        break;
    }
    //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_TEXTURE_CUBE_MAP);
}

void gyVidLightSourceUninit(GYLightSource* const light) {
    _LightInternals* internals = light->_data;

    glDeleteTextures(1, &internals->shadowMap);
    glDeleteFramebuffers(1, &internals->fbo);
    free(light->_data);
}

void _addLightUniform(const unsigned index, const GYLightSource*const light) {
    if(gyVidShadowPass()) return;

    int flushed = 0;

    const GYLightSource*const   old = glalLightSources[index];
    LightSourceUniforms*const   cache = &_lightSourceUniforms[index];
    _LightInternals*             internals = light->_data;

    //COMMON UNIFORMS TO ALL TYPES
    if(!old || !gyVec4Compare(&light->position, &cache->position)) {
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
        glUniform4fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_POS], 1, (float*)&light->position);
        memcpy(&cache->position, &light->position, sizeof(float)*4);
        ++glalUniformChangeCount;
    }

    if(!old || !gyVec4Compare(&light->properties->ambientColor, &cache->ambientColor)) {
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
        glUniform4fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_AMBIENT_COLOR], 1, (float*)&light->properties->ambientColor);
        memcpy(&cache->ambientColor, &light->properties->ambientColor, sizeof(float)*4);
        ++glalUniformChangeCount;
    }

    if(!old || !gyVec4Compare(&light->properties->diffuseColor, &cache->diffuseColor)) {
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
        glUniform4fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_DIFFUSE_COLOR], 1, (float*)&light->properties->diffuseColor);
        memcpy(&cache->diffuseColor, &light->properties->diffuseColor, sizeof(float)*4);
        ++glalUniformChangeCount;
    }

    if(!old || !gyVec4Compare(&light->properties->specularColor, &cache->specularColor)) {
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
        glUniform4fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SPECULAR_COLOR], 1, (float*)&light->properties->specularColor);
        memcpy(&cache->specularColor, &light->properties->specularColor, sizeof(float)*4);
        ++glalUniformChangeCount;
    }

    if(!old || !gyMat4Compare((const float(*)[4])light->camera->projViewMat, (const float(*)[4])cache->projViewMat)) { //prolly expensive as fuck comparison... still cheaper than uniform change
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
        glUniformMatrix4fv(glalLightingShader.lightsUniformLoc[index][GLAL_SHADER_LIGHT_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)light->camera->projViewMat);
        memcpy(cache->projViewMat, light->camera->projViewMat, sizeof(GYMatrix4));
        ++glalUniformChangeCount;
    }


   if(!old || !glalLightsEnabled[index]) {
       if(!flushed) {
           gyVidVertexBatchFlush();
           flushed = 1;
       }
        glUniform1i(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_ENABLED], 1);
        ++glalUniformChangeCount;
    }

   if(light->type == GY_LIGHT_POINT || light->type == GY_LIGHT_SPOT) {
        if(!old || !gyVec3Compare(&light->properties->attenuationFactors, &cache->attenuationFactors)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform3fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_ATTENUATION_FACTORS], 1, (float*)&light->properties->attenuationFactors);
            memcpy(&cache->attenuationFactors, &light->properties->attenuationFactors, sizeof(float)*3);
            ++glalUniformChangeCount;
        }
    }

    if(light->type == GY_LIGHT_DIRECTIONAL || light->type == GY_LIGHT_SPOT) {
        if(!old || !gyVec3Compare(&light->direction, &cache->direction)) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform3fv(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_DIRECTION], 1, (float*)&light->direction);
            memcpy(&cache->direction, &light->direction, sizeof(float)*3);
            ++glalUniformChangeCount;
        }
    }

   if(light->type == GY_LIGHT_SPOT || light->type == GY_LIGHT_POINT) {
        if(!old || light->properties->spotExponent != cache->spotExponent) {
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glUniform1f(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SPOT_EXPONENT], light->properties->spotExponent);
            cache->spotExponent = light->properties->spotExponent;
            ++glalUniformChangeCount;
        }

    if(!old || light->spotCutoffAngle != cache->spotCutoffAngle) {
        if(!flushed) {
            gyVidVertexBatchFlush();
            flushed = 1;
        }
            glUniform1f(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SPOT_CUTOFF_ANGLE], light->spotCutoffAngle);
            cache->spotCutoffAngle = light->spotCutoffAngle;
            ++glalUniformChangeCount;
    }
 }


    //Determine whether we're processing a point shadow's cubemap
    /* We don't have enough texture units for all shadow and shadow cube maps to
     * be bound to valid units, and the shaders crash if they aren't bound... but for
     * some reason they're still cool with being bound to bullshit texture units. So
     * bind one to the real texture unit, and bind one to a bullshit unit based on type.
     */

    if(light->type == GY_LIGHT_POINT) { //Point light + change

        if(cache->texture != internals->shadowMap) {
            glActiveTexture(glalTextureMapTexUnits[GLAL_SHADOW1_CUBE_MAP+index]);
           if(!flushed) {
               gyVidVertexBatchFlush();
               flushed = 1;
           }
         glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);
/*
            if(!old || cache->type != light->type) {
                    //glUniform1i(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SHADOW_MAP], 7+index);
                    glUniform1i(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SHADOW_CUBE_MAP], 3+index);
                    ++glalTextureChangeCount;
                    glalUniformChangeCount += 2;
             }*/
        }

    } else {  //Spot or Directional light + change
       // if(!(old && (cache->type == GY_LIGHT_DIRECTIONAL || cache->type == GY_LIGHT_SPOT))) {
          //  glDisable(GL_TEXTURE_CUBE_MAP);
          //  glEnable(GL_TEXTURE_2D);
        if(cache->texture != internals->shadowMap) {
        glActiveTexture(glalTextureMapTexUnits[GLAL_SHADOW1_MAP+index]);
            if(!flushed) {
                gyVidVertexBatchFlush();
                flushed = 1;
            }
            glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
            /*
              if(!old || cache->type != light->type) {
            glUniform1i(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SHADOW_MAP], 3+index);
            glUniform1i(glalCurLightShader->lightsUniformLoc[index][GLAL_SHADER_LIGHT_SHADOW_CUBE_MAP], 7+index);
            ++glalTextureChangeCount;
            glalUniformChangeCount += 2;
              }*/
        }
     //  }

    }

    cache->texture = internals->shadowMap;
    cache->type = light->type;
  //  glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
}

void gyVidLightSourceAdd(GYLightSource* const light) {
//	assert(lightSourceCount < GYRO_VIDEO_LIGHT_SOURCES_MAX);
    if(!glalLightingEnabled || gyVidShadowPass()) return;

    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
        if(glalLightsEnabled[i] == 0) {
            _addLightUniform(i, light);
            glalLightSources[i] = light;
            glalLightsEnabled[i] = 1;
            ++glalLightSourceCount;
            return;
        }
    }
}

void gyVidLightSourceRemove(const GYLightSource* const light) {
    if(!glalLightingEnabled || gyVidShadowPass()) return;

    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
        if(glalLightSources[i] == light) {
            if(glalLightsEnabled[i]) {
                --glalLightSourceCount;
               // glalStripComboBreaker();
                gyVidVertexBatchFlush();
                glUniform1i(glalCurLightShader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_ENABLED], 0);
                ++glalUniformChangeCount;
                glalLightsEnabled[i] = 0;
            }
            break;
        }
    }
}

void gyVidLightSourceReplace(const GYLightSource*const old, GYLightSource*const next) {
    if(!glalLightingEnabled || gyVidShadowPass()) return;

    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
        if(glalLightSources[i] == old && glalLightsEnabled[i]) {
            _addLightUniform(i, next);
            glalLightSources[i] = next;
            break;
        }
    }
}

void gyVidLightSourceClear(void) {
    if(gyVidShadowPass()) return;
    gyVidVertexBatchFlush();
    glalLightSourceCount = 0;
    for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
        if(glalLightsEnabled[i]) {
            glalLightsEnabled[i] = 0;
            //gyVidVertexBatchFlushPause();
            glUniform1i(glalCurLightShader->lightsUniformLoc[i][GLAL_SHADER_LIGHT_ENABLED], 0);
            ++glalUniformChangeCount;
        }
    }
   // gyVidVertexBatchFlushResume();
}

void glalGetSpotLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
    GYMatrix4 temp;
    GYVector3 baseVec = { 0.0f, 1.0f, 0.0f };
    GYVector3 rightVector;
    GYVector3 upVector;

    gyVec3CrossProd(&rightVector, &baseVec, &lightSrc->direction);
    gyVec3CrossProd(&upVector, &rightVector, &lightSrc->direction);


    gyMatPush();
    gyMatLoadIdentity();


    gyVidLookAtMat(temp,
                   -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
                  0.0f, 0.0f, 0.0f,
                   upVector.x, upVector.y, upVector.z);
    glalTranspose(temp);
    gyMatApply(temp);


    gyMatScale(1.0f, 1.0f, 1.0f);//4.0f

    //gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z-1000.0f);//+900.0f
    gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z);//+900.0f

    gyMatStore((void*)dest);
    gyMatPop();

}

void glalGetDirectionalLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
    GYMatrix4 temp;

    gyMatPush();
    gyMatLoadIdentity();

    //gyMatScale(2.0f, 2.0f, 1.0f);
    gyMatTranslate(GLAL_SHADOW_MAP_SIZE*0.5f, GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

    gyVidLookAtMat(temp,
                   -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
                   0.0f, 0.0f, 0.0f,
                   0.0f, -1.0f, 0.0f);

    gyMatApply(temp);

    gyMatTranslate(-glalCamWorldPos.x, -glalCamWorldPos.y, -glalCamWorldPos.z);
    //gyMatScale(2.0f, 2.0f, 1.0f);

    gyMatStore((void*)dest);
    gyMatPop();

}

void glalGetSpotLightProjectionMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc)  {
     //GYMatrix4 test;
#if 0
    //TODO: DON'T HARDCODE ANGLE FOR PERSPECTIVE PROJECTION!!!
    //ratio is wrong!!!
    gyVidPerspectiveMat(dest, 90, (float)GLAL_SHADOW_MAP_SIZE/(float)GLAL_SHADOW_MAP_SIZE, 0.5f, 150.0f);//150.0f
#else

    (void)lightSrc;

    //gyVidPerspectiveMat(dest, 90.0f, 1.0f, 1.0f, 150.0f);
    gyVidPerspectiveMat(dest, lightSrc->spotCutoffAngle, 1.0f, 1.0f, 550.0f);
#endif

    //memcpy(test, dest, sizeof(GYMatrix4));
}

void glalGetDirectionalLightProjectionMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
    (void)lightSrc;
    gyVidOrthoProjMat(dest, 0, GLAL_SHADOW_MAP_SIZE,
                             0, GLAL_SHADOW_MAP_SIZE,
                             -1500, 1500);
}

void glalGetPointLightProjectionMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc) {
    gyVidPerspectiveMat(dest, 90.0f, 1.0f, 0.5f, 901.0f);
    (void)lightSrc;

    //gyVidCamFrustum(camera, 90.0f, 1.0f, 0.5f, 901.0f);
}


void glalGetPointLightCamMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc) {
    gyMatPush();
    gyMatLoadIdentity();

    gyMatScale(1.0f, -1.0f, -1.0f);
    gyMatTranslate(-lightSrc->position.x+1, -lightSrc->position.y, -lightSrc->position.z);

    gyMatStore((void*)dest);
    gyMatPop();
}

void glalGetPointLightViewMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc, unsigned face) {
    GYMatrix4 camMat;
    gyMatPush();
    gyMatLoadIdentity();

    //Create View Matrix
    gyMatApply((void*)&glalPointShadowFaceMatrices[face]);

    glalGetPointLightCamMatrix(camMat, lightSrc);

    gyMatApply((void*)&camMat);
    gyMatStore((void*)dest);
    gyMatPop();
}

void glalGetPointLightViewVolume(GYViewVolume* const vol, GYLightSource* lightSrc, const GY_CUBE_MAP_FACE face) {
    //const float maxDist = 901.0f-0.5f;
    const float halfW = lightSrc->properties->_radius;//maxDist*0.5f;

    vol->ntl.x = lightSrc->position.x - halfW; vol->ntl.y = lightSrc->position.y - halfW;
    vol->ntr.x = lightSrc->position.x + halfW; vol->ntr.y = lightSrc->position.y - halfW;
    vol->nbl.x = lightSrc->position.x - halfW; vol->nbl.y = lightSrc->position.y + halfW;
    vol->nbr.x = lightSrc->position.x + halfW; vol->nbr.y = lightSrc->position.y + halfW;

    switch(face) {
    case GY_CUBE_MAP_RIGHT:
        vol->ntl.x += halfW;
        vol->ntr.x += halfW;
        vol->nbl.x += halfW;
        vol->nbr.x += halfW;
        break;
    case GY_CUBE_MAP_LEFT:
        vol->ntl.x -= halfW;
        vol->ntr.x -= halfW;
        vol->nbl.x -= halfW;
        vol->nbr.x -= halfW;
        break;

    case GY_CUBE_MAP_BACK:
        vol->ntl.y -= halfW;
        vol->ntr.y -= halfW;
        vol->nbl.y -= halfW;
        vol->nbr.y -= halfW;
        break;

    case GY_CUBE_MAP_FRONT:
        vol->ntl.y += halfW;
        vol->ntr.y += halfW;
        vol->nbl.y += halfW;
        vol->nbr.y += halfW;
        break;

    case GY_CUBE_MAP_BOTTOM:
    case GY_CUBE_MAP_TOP:
    default:
        break;
    }

    vol->ftl = vol->ntl;
    vol->ftr = vol->ntr;
    vol->fbl = vol->nbl;
    vol->fbr = vol->nbr;

}

void gyVidLightSourceBeginShadowPasses(void) {
    gyVidVertexBatchFlush();
    //glFlush();
    gyVidSetCurrentShaderProgram(glalShadowMapShader.program);
    glalCurLightShader = &glalShadowMapShader;
    gyVidAlphaLevel(GY_ALPHA_NONE);
    _shadowPass = 1;
    _firstShadowPass = 1;
}


void gyVidLightSourceBeginShadowPass(GYLightSource* const light, const GY_CUBE_MAP_FACE face) {
    GYMatrix4 lightProjMatrix;
    GYMatrix4 lightViewMatrix;
    GYMatrix4 lightProjViewMatrix;
    int useBias;
    _LightInternals* internals = light->_data;
    GYCamera*const cam = light->camera;

    if(!_firstShadowPass) gyVidVertexBatchFlush();
    glViewport(0, 0, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE);

    glActiveTexture(glalTextureMapTexUnits[GLAL_TEMP_MAP]);

    switch(light->type) {

    case GY_LIGHT_POINT:
        useBias = 0;

        //glDepthRange(0, 1);
        glalGetPointLightProjectionMatrix(lightProjMatrix, light);
        glalGetPointLightViewMatrix(lightViewMatrix, light, face);

       // glEnable(GL_TEXTURE_CUBE_MAP);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        //glBindTexture(GL_TEXTURE_2D, 0);

        //Only do this shit once, on the first pass
        if(face == GY_CUBE_MAP_RIGHT) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, internals->fbo);
            glalGetPointLightCamMatrix(cam->projViewMat, light);
        } //else if(face == GY_CUBE_MAP_FRONT) return;

        glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);

        //glalGetPointLightViewVolume(&cam->viewVolume, light, face);

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                               internals->shadowMap,
                               0);

        break;

    case GY_LIGHT_DIRECTIONAL:
        useBias = 1;

        glalGetDirectionalLightProjectionMatrix(lightProjMatrix, light);
        glalGetDirectionalLightViewMatrix(lightViewMatrix, light);

        //glCullFace(GL_BACK);
        //glEnable(GL_TEXTURE_2D);
                //glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, internals->fbo);
        glBindTexture(GL_TEXTURE_2D, internals->shadowMap);

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               internals->shadowMap,
                               0);
        break;


    case GY_LIGHT_SPOT:
        useBias = 1;

        glalGetSpotLightProjectionMatrix(lightProjMatrix, light);
        glalGetSpotLightViewMatrix(lightViewMatrix, light);

        //glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D, 0);

        //glCullFace(GL_BACK);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, internals->fbo);
        glBindTexture(GL_TEXTURE_2D, internals->shadowMap);

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               internals->shadowMap,
                               0);

        break;

    }

#if !defined(ANDROID) /* TODO: FIXME */
    glDrawBuffer(GL_NONE); // No color buffer is drawn to.
   // glReadBuffer(GL_NONE);
#endif

    //Check whether shit has reested with our FBO
    GLenum status;
     if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
         _gyLog(GY_DEBUG_CRITICAL, "Binding GLAL depth texture to frame buffer has reested - %d!", status);
         return;
     }

     //glEnable(GL_CULL_FACE);
    //    glCullFace(GL_FRONT);

             glClear(GL_DEPTH_BUFFER_BIT);
    if(_firstShadowPass) {
         glClearDepth(1.0f);
         glDepthRange(0.0f, 1.0f);
        glDisable(GL_BLEND);
        //gyVidDepthTest(1);
        //glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_BLEND);
        glDepthFunc(GL_LESS);
        _firstShadowPass = 0;
    }

    //find projView matrix
    gyMatMultMat(lightProjViewMatrix, lightProjMatrix, lightViewMatrix);

    //SUBMIT PROJVIEW UNIFORM TO SHADOW SHADER
    glUniformMatrix4fv(glalShadowMapShader.uniformLoc[GLAL_SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)lightProjViewMatrix);
    ++glalUniformChangeCount;

    //Store projection view matrix (plus bias) for next render pass
    if(useBias) gyMatMultMat(cam->projViewMat, glalShadowTexCoordBiasMat, lightProjViewMatrix);

}

void gyVidLightSourceEndShadowPasses(void) {
gyVidVertexBatchFlush();
   // glFlush();
    _shadowPass = 0;
    gyVidSetDefaultShaderProgram();
    //glalDefaultShadersLoadUniforms();
    glalCurLightShader = &glalLightingShader;
   // glFlush();
  // gyVidVertexBatchFlush();
    glalVidBindDefaultFbo();
   // glDrawBuffer(GL_BACK);
    unsigned width, height;
    gyVidResolution(&width, &height);
    glViewport(0, 0, (float)width, (float)height);

    //gyVidDepthTest(0);
    //glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glEnable(GL_BLEND);
  //  glDepthFunc(GL_LEQUAL);
   // glDisable(GL_CULL_FACE);


    //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    //glBindTexture(GL_TEXTURE_2D, 0);
}
