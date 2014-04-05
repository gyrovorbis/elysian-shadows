#include <gyro_video_api.h>
#include <gyro_shader_api.h>
#include <gyro_defines.h>
#include <gyro_system_api.h>
#include "gyro_shaders_glal.h"
#include "gyro_video_glal.h"
#include <stdio.h>

const static GLenum _shadowMapTexUnits[GYRO_VIDEO_LIGHT_SOURCES_MAX] = {
	GL_TEXTURE3,
	GL_TEXTURE4,
	GL_TEXTURE5,
	GL_TEXTURE6
};

GYLightSource*		lightSources[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { NULL };
unsigned			lightSourceCount								= 0;
int					lightingEnabled									= 1;
int					lightsEnabled[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { 0 };

void gyVidLightingSetEnable(const int enabled) {
	lightingEnabled = enabled;
	glUniform1i(curLightShader->uniformLoc[SHADER_LIGHTING_ENABLED], enabled);
}

void gyVidLightSourceInit(GYLightSource* const light, const GY_LIGHT_TYPE type) {
	_LightInternals* internals = light->_data = malloc(sizeof(_LightInternals));

	glGenFramebuffers(1, &internals->fbo);
	glGenTextures(1, &internals->shadowMap);

	light->type = type;

	switch(type) {
	case GY_LIGHT_DIRECTIONAL:
	case GY_LIGHT_SPOT:
		glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 1.0f);
		break;

	case GY_LIGHT_POINT:
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, 1.0f);
		//Does this REALLY have to have 6 faces!?
		for (unsigned i = 0; i < GY_CUBE_MAP_FACE_MAX; ++i) {
			//shadowDepth? lolz
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		break;
	}
	glEnable(GL_TEXTURE_2D);
}

void gyVidLightSourceUninit(GYLightSource* const light) {
	_LightInternals* internals = light->_data;

	glDeleteTextures(1, &internals->shadowMap);
	glDeleteFramebuffers(1, &internals->fbo);
	free(light->_data);
}


void _addLightUniform(const unsigned index, const GYLightSource*const light) {
	_LightInternals* internals = light->_data;

	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_POS], 1, (float*)&light->position);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_AMBIENT_COLOR], 1, (float*)&light->properties->ambientColor);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_DIFFUSE_COLOR], 1, (float*)&light->properties->diffuseColor);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPECULAR_COLOR], 1, (float*)&light->properties->specularColor);

	glUniform3fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_DIRECTION], 1, (float*)&light->direction);
	glUniform3fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_ATTENUATION_FACTORS], 1, (float*)&light->properties->attenuationFactors);

	glUniform1f(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPOT_EXPONENT], light->properties->spotExponent);
	glUniform1f(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPOT_CUTOFF_ANGLE], light->spotCutoffAngle);

	glUniformMatrix4fv(_lightingShader.lightsUniformLoc[index][SHADER_LIGHT_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)light->camera->projViewMat);

	glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_ENABLED], 1);

	//Determine whether we're processing a point shadow's cubemap
	/* We don't have enough texture units for all shadow and shadow cube maps to
	 * be bound to valid units, and the shaders crash if they aren't bound... but for
	 * some reason they're still cool with being bound to bullshit texture units. So
	 * bind one to the real texture unit, and bind one to a bullshit unit based on type.
	 */
	glActiveTexture(_shadowMapTexUnits[index]);
	if(light->type == GY_LIGHT_POINT) {
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_MAP], 7+index);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_CUBE_MAP], 3+index);
	} else {
		glDisable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_MAP], 3+index);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_CUBE_MAP], 7+index);
	}
	glActiveTexture(GL_TEXTURE0);
}

void gyVidLightSourceAdd(GYLightSource* const light) {
	assert(lightSourceCount < GYRO_VIDEO_LIGHT_SOURCES_MAX);

	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		if(lightsEnabled[i] == 0) {
			lightSources[i] = light;
			_addLightUniform(i, light);
			lightsEnabled[i] = 1;
			++lightSourceCount;
			return;
		}
	}
}

void gyVidLightSourceRemove(const GYLightSource* const light) {
	--lightSourceCount;
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		if(lightSources[i] == light) {
			lightsEnabled[i] = 0;
			glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_ENABLED], 0);
			break;
		}
	}
}

void gyVidLightSourceReplace(const GYLightSource*const old, GYLightSource*const next) {
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		if(lightSources[i] == old) {
			lightSources[i] = next;
			_addLightUniform(i, next);
			break;
		}
	}
}

void gyVidLightSourceClear(void) {
	lightSourceCount = 0;
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		lightsEnabled[i] = 0;
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_ENABLED], 0);
	}
}


void glalGetSpotLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;

	gyMatPush();
	gyMatLoadIdentity();

#if 0
	gyMatRotateZ(0.785398163f);

	gyVidLookAtMat(temp,
				   -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
				   0.0f, 0.0f, 0.0f,
				   0.0f, 0.0f, 1.0f);
	gyMatApply(temp);


	gyMatScale(1.0f, -1.0f, -1.0f);//4.0f

	//gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z-1000.0f);//+900.0f
	gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z);//+900.0f
#else
//gyMatRotateZ(-0.785398163f);

	gyVidLookAtMat(temp,
				   0.0, 0.0, 0.0,
				   lightSrc->direction.x, lightSrc->direction.y, lightSrc->direction.z,
				   0.0,0.0, 1.0);

	gyMatApply(temp);
	gyMatScale(1.0f, -1.0f, -1.0f);
	gyMatTranslate(-lightSrc->position.x+1, -lightSrc->position.y, -lightSrc->position.z);

#endif

	gyMatStore((void*)&dest);
	gyMatPop();

}

void glalGetDirectionalLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;

	gyMatPush();
	gyMatLoadIdentity();

	gyMatTranslate(GLAL_SHADOW_MAP_SIZE*0.5f, GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

	gyVidLookAtMat(temp,
				   -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
				   0.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f);

	//glalTranspose(temp);

	gyMatApply(temp);

	gyMatTranslate(-GLAL_SHADOW_MAP_SIZE*0.5f, -GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

	gyMatTranslate(GLAL_SHADOW_MAP_SIZE*0.5f, GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

//	gyMatRotateZ(viewZOrient);
	gyMatScale(1.0f, 1.0f, 1.0f);
	gyMatTranslate(0.0f, 0.0f, 0.0f);

	gyMatStore((void*)dest);
	gyMatPop();

}

void glalGetDirectionalLightProjectionMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	(void)lightSrc;
	gyVidOrthoProjMat(dest, 0, GLAL_SHADOW_MAP_SIZE,
							 0, GLAL_SHADOW_MAP_SIZE,
							 -1500, 1500);
}

void glalGetSpotLightProjectionMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc)  {
	 //GYMatrix4 test;
#if 0
	//TODO: DON'T HARDCODE ANGLE FOR PERSPECTIVE PROJECTION!!!
	//ratio is wrong!!!
	gyVidPerspectiveMat(dest, 90, (float)GLAL_SHADOW_MAP_SIZE/(float)GLAL_SHADOW_MAP_SIZE, 0.5f, 150.0f);//150.0f
#else

	(void)lightSrc;

	gyVidPerspectiveMat(dest, 90.0f, 1.0f, 0.5f, 150.0f);
#endif

	//memcpy(test, dest, sizeof(GYMatrix4));
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
	gyMatApply((void*)&pointShadowFaceMatrices[face]);

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
	gyVidSetCurrentShaderProgram(_shadowMapShader.program);
	curLightShader = &_shadowMapShader;
}


void gyVidLightSourceBeginShadowPass(GYLightSource* const light, const GY_CUBE_MAP_FACE face) {
	GYMatrix4 lightProjMatrix;
	GYMatrix4 lightViewMatrix;
	GYMatrix4 lightProjViewMatrix;
	int useBias;
	_LightInternals* internals = light->_data;
	GYCamera*const cam = light->camera;

	glViewport(0, 0, GLAL_SHADOW_MAP_SIZE, GLAL_SHADOW_MAP_SIZE);

	switch(light->type) {

	case GY_LIGHT_POINT:
		useBias = 0;

		//glDepthRange(0, 1);
		glalGetPointLightProjectionMatrix(lightProjMatrix, light);
		glalGetPointLightViewMatrix(lightViewMatrix, light, face);

		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, internals->shadowMap);

		//Only do this shit once, on the first pass
		if(face == GY_CUBE_MAP_RIGHT) {
			glBindFramebuffer(GL_FRAMEBUFFER, internals->fbo);
			glalGetPointLightCamMatrix(cam->projViewMat, light);
		}

		//glalGetPointLightViewVolume(&cam->viewVolume, light, face);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
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
		glBindFramebuffer(GL_FRAMEBUFFER, internals->fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D,
							   internals->shadowMap,
							   0);
		break;


	case GY_LIGHT_SPOT:
		useBias = 1;

		glalGetSpotLightProjectionMatrix(lightProjMatrix, light);
		//glalGetSpotLightViewMatrix(lightViewMatrix, light);
		glalGetSpotLightViewMatrix(lightViewMatrix, light);

		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, internals->fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D,
							   internals->shadowMap,
							   0);

		break;

	}

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	glReadBuffer(GL_NONE);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Check whether shit has reested with our FBO
	GLenum status;
	 if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		 _gyLog(GY_DEBUG_CRITICAL, "Binding GLAL depth texture to frame buffer has reested - %d!", status);
		 return;
	 }
	glEnable(GL_DEPTH_TEST);

	//find projView matrix
	gyMatMultMat(lightProjViewMatrix, lightProjMatrix, lightViewMatrix);

	//SUBMIT PROJVIEW UNIFORM TO SHADOW SHADER
	glUniformMatrix4fv(_shadowMapShader.uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)lightProjViewMatrix);

	//Store projection view matrix (plus bias) for next render pass
	if(useBias) gyMatMultMat(cam->projViewMat, shadowTexCoordBiasMat, lightProjViewMatrix);

}

void gyVidLightSourceEndShadowPasses(void) {
	gyVidSetDefaultShaderProgram();
	curLightShader = &_lightingShader;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	unsigned width, height;
	gyVidResolution(&width, &height);
	glViewport(0, 0, width, height);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}


\
