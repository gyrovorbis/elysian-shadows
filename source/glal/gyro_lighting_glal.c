#include <gyro_video_api.h>
#include <gyro_shader_api.h>
#include <gyro_defines.h>
#include <gyro_system_api.h>
#include "gyro_shaders_glal.h"
#include "gyro_video_glal.h"
#include <stdio.h>

const static GLenum _shadowMapTexUnits[GYRO_VIDEO_LIGHT_SOURCES_MAX] = {
	GL_TEXTURE2,
	GL_TEXTURE3,
	GL_TEXTURE4,
	GL_TEXTURE5
};

GYLightSource*		lightSources[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { NULL };
unsigned			lightSourceCount								= 0;
int					lightingEnabled									= 1;
int					lightsEnabled[GYRO_VIDEO_LIGHT_SOURCES_MAX]		= { 0 };

void glalTranspose(GYMatrix4 mat) {
	GYMatrix4 mat2;
	memcpy(mat2, mat, sizeof(GYMatrix4));
	for(unsigned i = 0; i < 4; ++i)
		for(unsigned j = 0; j < 4; ++j)
			mat[i][j] = mat2[j][i];
}

void gyVidLightingSetEnable(const int enabled) {
	lightingEnabled = enabled;
	glUniform1i(curLightShader->uniformLoc[SHADER_LIGHTING_ENABLED], enabled);
}

void gyVidLightSourceClear(void) {
	lightSourceCount = 0;
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		lightsEnabled[i] = 0;
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_ENABLED], 0);
	}
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
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_MAP], 6+index);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_CUBE_MAP], 2+index);
	} else {
		glDisable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, internals->shadowMap);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_MAP], 2+index);
		glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SHADOW_CUBE_MAP], 6+index);
	}
	glActiveTexture(GL_TEXTURE0);
}

void gyVidLightSourceAdd(const GYLightSource* const light) {
	assert(lightSourceCount < GYRO_VIDEO_LIGHT_SOURCES_MAX);
	lightSources[lightSourceCount] = light;

	_addLightUniform(lightSourceCount, light);
	++lightSourceCount;
}


void glalGetSpotLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;

	gyMatPush();
	gyMatLoadIdentity();

	gyMatRotateZ(0.785398163f);

	gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 0.0f, 1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
	glLoadIdentity();
	gyMatApply((float*)&temp);


	gyMatScale(1.0f, 1.0f, -1.0f);//4.0f

	gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z-1000.0f);//+900.0f

	gyMatStore((float*)dest);
	gyMatPop();
}

void glalGetDirectionalLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;

	gyMatPush();
	gyMatLoadIdentity();

	gyMatTranslate(GLAL_SHADOW_MAP_SIZE*0.5f, GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
	glLoadIdentity();
	//glalTranspose(temp);

	gyMatApply((float*)&temp);

	gyMatTranslate(-GLAL_SHADOW_MAP_SIZE*0.5f, -GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

	gyMatTranslate(GLAL_SHADOW_MAP_SIZE*0.5f, GLAL_SHADOW_MAP_SIZE*0.5f, 0.0f);

//	gyMatRotateZ(viewZOrient);
	gyMatScale(viewScale, viewScale, viewScale);
	gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);

	gyMatStore((float*)dest);
	gyMatPop();
}

void glalGetDirectionalLightProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc) {
	gyVidOrthoProjMatrix(dest, 0, GLAL_SHADOW_MAP_SIZE,
							 0, GLAL_SHADOW_MAP_SIZE,
							 -1500, 1500);
}

void glalGetSpotLightProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc)  {
	 GYMatrix4 test;

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	//TODO: DON'T HARDCODE ANGLE FOR PERSPECTIVE PROJECTION!!!
	//ratio is wrong!!!
	gluPerspective(90, (float)GLAL_SHADOW_MAP_SIZE/(float)GLAL_SHADOW_MAP_SIZE, 0.5f, 500.0f);//150.0f
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)test);
	glLoadIdentity();

	memcpy(test, dest, sizeof(GYMatrix4));
}

void glalGetPointLightProjectionMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc) {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, 0.5f, 901.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)dest);
	glalTranspose(dest);
	glLoadIdentity();
}

void glalGetPointLightCamMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc) {
	gyMatPush();
	gyMatLoadIdentity();

	gyMatScale(1.0f, -1.0f, -1.0f);
	gyMatTranslate(-lightSrc->position.x+1, -lightSrc->position.y, -lightSrc->position.z);

	gyMatStore(dest);
	gyMatPop();
}

void glalGetPointLightViewMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc, unsigned face) {
	GYMatrix4 camMat;
	gyMatPush();
	gyMatLoadIdentity();

	//Create View Matrix
	gyMatApply(&pointShadowFaceMatrices[face]);

	glalGetPointLightCamMatrix(camMat, lightSrc);
	gyMatApply(&camMat);
	gyMatStore(dest);
	gyMatPop();
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
			glalGetPointLightCamMatrix(light->projViewMat, light);
		}

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
	if(useBias) gyMatMultMat(&light->projViewMat, &shadowTexCoordBiasMat, &lightProjViewMatrix);

}

void gyVidLightSourceEndShadowPasses(void) {
	gyVidSetDefaultShaderProgram();
	curLightShader = &_lightingShader;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	unsigned width, height;
	gyVidResolution(&width, &height);
	glViewport(0, 0, width, height);

	GYMatrix4 mat;
	//SUBMIT PROJVIEW UNIFORM TO LIGHT SHADER
	for(unsigned i = 0; i < lightSourceCount; ++i) {
		glUniformMatrix4fv(_lightingShader.lightsUniformLoc[i][SHADER_LIGHT_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)&lightSources[i]->projViewMat);
		memcpy(mat, lightSources[i]->projViewMat, sizeof(GYMatrix4));
	}
}


#if 0 //REMOVE ME ONCE SHIT IS PRISTINE




int gyVidBeginNextShadowPass(void) {
	GYMatrix4 lightProjMatrix;
	GYMatrix4 lightViewMatrix;
	GYMatrix4 lightProjViewMatrix;

	GYVector3 center3D = { 0.0f, 0.0f, 0.0f };
	GYVector3 upVector3D = { 0.0f, -1.0f, 0.0f };

	int useBias;

	if(_curShadowPass == 0) {
		if(_lightSourceCount != 0) {
			gyVidSetCurrentShaderProgram(_shadowMapShader.program);
			curLightShader = &_shadowMapShader;

		} else {
			return 0;
		}
	} else if(_curShadowPass >= _lightSourceCount) {
		_curShadowPass = 0;
		gyVidSetDefaultShaderProgram();
		curLightShader = &_lightingShader;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		unsigned width, height;
		gyVidResolution(&width, &height);
		glViewport(0, 0, width, height);

		//SUBMIT PROJVIEW UNIFORM TO LIGHT SHADER
		for(unsigned i = 0; i < _lightSourceCount; ++i)
		glUniformMatrix4fv(_lightingShader.lightsUniformLoc[i][SHADER_LIGHT_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)&_lightSources[i]->projViewMat);
		return 0;
	}

	GYLightSource* curLight = _lightSources[_curShadowPass];

	//glEnable(GL_DEPTH_TEST);
#define LAST_POINT_FACE CUBE_MAP_FACES_MAX
	//Determine whether we're processing a point shadow's cubemap
	if(curLight->spotCutoffAngle >= 180.0f && curLight->position.w != 0.0f) {
		useBias = 0;

		if(_curPointShadowFace == CUBE_MAP_FACES_MAX)
			_curPointShadowFace = CUBE_MAP_RIGHT;
		//else if(_curPointShadowFace == CUBE_MAP_TOP)
			//_curPointShadowFace = CUBE_MAP_BACK;

		glViewport(0, 0, 1024, 1024);
		//glDepthRange(0, 1);
		glalGetPointLightProjectionMatrix(lightProjMatrix);
		glalGetPointLightViewMatrix(lightViewMatrix, curLight, _curPointShadowFace);

		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMaps[_curShadowPass]);

		//glViewport(0, 0, 720, 720);

		//glCullFace(GL_FRONT);

		//glBindFramebuffer(GL_FRAMEBUFFER, _shadowCubeFBOs[_curPointShadowFace]);
		if(_curPointShadowFace == CUBE_MAP_RIGHT) glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBOs[_curShadowPass]);
		//else if(_curPointShadowFace == CUBE_MAP_TOP) _curPointShadowFace = CUBE_MAP_BACK;

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_CUBE_MAP_POSITIVE_X + _curPointShadowFace,
							   _shadowCubeMaps[_curShadowPass],
							   0);

	} else {
		useBias = 1;

		glalGetLightProjectionMatrix(lightProjMatrix, curLight);
		glalGetLightViewMatrix(lightViewMatrix, curLight);

		//glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBOs[_curShadowPass]);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D,
							   _shadowMaps[_curShadowPass],
							   0);
	}

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	glReadBuffer(GL_NONE);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Check whether shit has reested with our FBO
	GLenum status;
	 if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		 _gyLog(GY_DEBUG_CRITICAL, "Binding GLAL depth texture to frame buffer has reested - %d!", status);
		 _curShadowPass = 0;
		 gyVidSetDefaultShaderProgram();
		 glBindFramebuffer(GL_FRAMEBUFFER, 0);
		 curLightShader = &_lightingShader;
		 return 0;
	 }
	glEnable(GL_DEPTH_TEST);

	//find projView matrix
	gyMatMultMat(lightProjViewMatrix, lightProjMatrix, lightViewMatrix);

	//SUBMIT PROJVIEW UNIFORM TO SHADOW SHADER
	glUniformMatrix4fv(_shadowMapShader.uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)lightProjViewMatrix);

	//Store projection view matrix (plus bias) for next render pass
	if(useBias) gyMatMultMat(&curLight->projViewMat, &shadowTexCoordBiasMat, &lightProjViewMatrix);
	else if(_curPointShadowFace+1 == LAST_POINT_FACE) {
		glalGetPointLightCamMatrix(curLight->projViewMat, curLight);
	}

	if(_curPointShadowFace != CUBE_MAP_FACES_MAX)	 ++_curPointShadowFace;
	if(_curPointShadowFace == LAST_POINT_FACE)		_curPointShadowFace = CUBE_MAP_FACES_MAX;
	if(_curPointShadowFace == CUBE_MAP_FACES_MAX)	++_curShadowPass;

	return 1;
}




void glalGetLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;
	unsigned scrWidth, scrHeight;
	gyVidResolution(&scrWidth, &scrHeight);

	if(lightSrc->position.w == 0.0f) {
		gyMatPush();
		gyMatLoadIdentity();

		//gyMatScale(1.0f/viewScale, 1.0f/viewScale, 0.0f);
		//gyMatRotateZ(-viewZOrient);

		gyMatTranslate(shadowWidth*0.5f, shadowHeight*0.5f, 0.0f);
		//gyMatTranslate(viewPosition.x, viewPosition.y, 100.0f);

		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadIdentity();
		gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
				  0.0f, 0.0f, 0.0f,
				  0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
		glLoadIdentity();
		//glalTranspose(temp);

		gyMatApply((float*)&temp);

		gyMatTranslate(-shadowWidth*0.5f, -shadowHeight*0.5f, 0.0f);

		gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);

	//	gyMatRotateZ(viewZOrient);
		gyMatScale(viewScale, viewScale, viewScale);
		gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);


		/* camera's transform. */
	//	gyMatTranslate(shadowWidth/2.0f, shadowHeight/2.0f, 0.0f);
		//gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);


	//	gyMatApply(&viewMatrix);

		//gyMatTranslate(-1280.0f*0.5f, -720.0f*0.5f, 0.0f);
		//gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);

		gyMatStore((float*)dest);
		gyMatPop();

	} else {
		//FIXME: Shadows appear to be rotated about the Z-axis, for
		//some fuck-knows-why mathematical reason. Also not sure the Peter-panning
		//going on is correct... could be a fundamental incompatibility with viewing
		//a perspective projection in an orthographic projection?

		if(lightSrc->spotCutoffAngle < 180) {
			gyMatPush();
			gyMatLoadIdentity();

			//glScalef(-1.0f, -1.0f, 0.0f);

			gyMatRotateZ(0.785398163f);

			gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
			glLoadIdentity();
			gyMatApply((float*)&temp);

			//gyMatRotate(0.785398163f*2.0f, -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z);

			gyMatScale(1.0f, 1.0f, -1.0f);//4.0f

			gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z-1000.0f);//+900.0f

			gyMatStore((float*)dest);
			gyMatPop();
		}


	}
}





void glalGetLightViewMatrix(GYMatrix4 dest, const GYLightSource*const lightSrc) {
	GYMatrix4 temp;
	unsigned scrWidth, scrHeight;
	gyVidResolution(&scrWidth, &scrHeight);

	if(lightSrc->position.w == 0.0f) {
		gyMatPush();
		gyMatLoadIdentity();

		//gyMatScale(1.0f/viewScale, 1.0f/viewScale, 0.0f);
		//gyMatRotateZ(-viewZOrient);

		gyMatTranslate(SHADOW_MAP*0.5f, shadowHeight*0.5f, 0.0f);
		//gyMatTranslate(viewPosition.x, viewPosition.y, 100.0f);

		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadIdentity();
		gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
				  0.0f, 0.0f, 0.0f,
				  0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
		glLoadIdentity();
		//glalTranspose(temp);

		gyMatApply((float*)&temp);

		gyMatTranslate(-shadowWidth*0.5f, -shadowHeight*0.5f, 0.0f);

		gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);

	//	gyMatRotateZ(viewZOrient);
		gyMatScale(viewScale, viewScale, viewScale);
		gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);


		/* camera's transform. */
	//	gyMatTranslate(shadowWidth/2.0f, shadowHeight/2.0f, 0.0f);
		//gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);


	//	gyMatApply(&viewMatrix);

		//gyMatTranslate(-1280.0f*0.5f, -720.0f*0.5f, 0.0f);
		//gyMatTranslate(-viewPosition.x, -viewPosition.y, 0.0f);

		gyMatStore((float*)dest);
		gyMatPop();

	} else {
		//FIXME: Shadows appear to be rotated about the Z-axis, for
		//some fuck-knows-why mathematical reason. Also not sure the Peter-panning
		//going on is correct... could be a fundamental incompatibility with viewing
		//a perspective projection in an orthographic projection?

		if(lightSrc->spotCutoffAngle < 180) {
			gyMatPush();
			gyMatLoadIdentity();

			//glScalef(-1.0f, -1.0f, 0.0f);

			gyMatRotateZ(0.785398163f);

			gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
			glLoadIdentity();
			gyMatApply((float*)&temp);

			//gyMatRotate(0.785398163f*2.0f, -lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z);

			gyMatScale(1.0f, 1.0f, -1.0f);//4.0f

			gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z-1000.0f);//+900.0f

			gyMatStore((float*)dest);
			gyMatPop();
		}


	}
}

void glalGetLightOrthoProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc) {
	gyVidOrthoProjMatrix(dest, 0, shadowWidth,
							 0, shadowHeight,
							 -1500, 1500);
}

void glalGetLightPerspectiveProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc)  {
	 GYMatrix4 test;

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	//TODO: DON'T HARDCODE ANGLE FOR PERSPECTIVE PROJECTION!!!
	//ratio is wrong!!!
	gluPerspective(90, (float)shadowWidth/(float)shadowHeight, 0.5f, 500.0f);//150.0f
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)test);
	glLoadIdentity();

	memcpy(test, dest, sizeof(GYMatrix4));
}

void glalGetLightProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc) {
	if(lightSrc->position.w == 0) {
		glalGetLightOrthoProjectionMatrix(dest, lightSrc);
	} else {
		if(lightSrc->spotCutoffAngle < 180) {
			glalGetLightPerspectiveProjectionMatrix(dest, lightSrc);
			glalGetLightOrthoProjectionMatrix(dest, lightSrc);
		} else {

		}
	}
}

void glalGetPointLightProjectionMatrix(GYMatrix4 dest) {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, 0.5f, 901.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)dest);
	glalTranspose(dest);
	glLoadIdentity();
}

void glalGetPointLightCamMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc) {
	gyMatPush();
	//SEEMED LEGIT FOR UP DIR
	gyMatLoadIdentity();

#if 0
	gyMatTranslate(-lightSrc->position.x, lightSrc->position.y-30.0f, -lightSrc->position.z);
	gyMatTranslate(0.0f, 30.0f, -30.0);
	gyMatScale(1.0f, -1.0f, 1.0f);
#else


		//gyMatTranslate(0.0f, 0.0f, 0.0f);

	gyMatScale(1.0f, -1.0f, -1.0f);
	gyMatTranslate(-lightSrc->position.x+1, -lightSrc->position.y, -lightSrc->position.z);

#endif

/*
	gyMatTranslate(-lightSrc->position.x, lightSrc->position.y, -lightSrc->position.z-30.0f);

		gyMatScale(1.0f, -1.0f, 1.0f);*/

	//gyMatTranslate(0.0f, 50.0f, 0.0);

	//gyMatTranslate(0.0f, )


//	gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y/*-30.0f*/, -lightSrc->position.z);


	//gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z);


	//gyMatTranslate(-lightSrc->position.x, -lightSrc->position.y, -lightSrc->position.z);
	//gyMatTranslate(0.0f, 30.0f, -30.0f);
	//gyMatScale(1.0f, -1.0f, 1.0f);


	gyMatStore(dest);
	gyMatPop();
}

void glalGetPointLightViewMatrix(GYMatrix4 dest, const GYLightSource* const lightSrc, unsigned face) {
	GYMatrix4 camMat;
	gyMatPush();
	gyMatLoadIdentity();
	//gyMatRotateZ(0.785398163f);
	//Create View Matrix
	gyMatApply(&_pointShadowFaceMatrices[face]);

		//gyMatScale(1.0f, 1.0f, -1.0f);

	glalGetPointLightCamMatrix(camMat, lightSrc);
	gyMatApply(&camMat);
	gyMatStore(dest);
	gyMatPop();
}

static const GYMatrix4 shadowTexCoordBiasMat = {
	{ 0.5f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.5f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.5f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f },
};







void _glalShadowMultiPassInit(void) {
	static int initialized = 0;

	if(initialized) {
		glDeleteTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowMaps);
		glDeleteFramebuffers(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowFBOs);
	}

	glGenFramebuffers(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowFBOs);
	glGenFramebuffers(6, _shadowCubeFBOs);
	glGenTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowMaps);
	glGenTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowCubeMaps);

	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {

		//glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBOs[i]);

		glBindTexture(GL_TEXTURE_2D, _shadowMaps[i]);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		 glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		 glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 1.0f);

		//initialize cubemaps
		 glEnable(GL_TEXTURE_CUBE_MAP);
		 glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMaps[i]);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		 //probably should be intensity?
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		 //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		 glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, 1.0f);
		 //Does this REALLY have to have 6 faces!?
		 for (size_t i = 0; i < 6; ++i) {
			 //shadowDepth? lolz
			 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, blankCubeFace);
		 }
		 glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	 }
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_TEXTURE_2D);

	//glActiveTexture(GL_TEXTURE7);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMaps[0]);
	//glUniform1i(curLightShader->lightsUniformLoc[0][SHADER_LIGHT_SHADOW_CUBE_MAP], 50);
	//glActiveTexture(GL_TEXTURE0);

	// create point light face view matrices
	glMatrixMode(GL_MODELVIEW);

/*	gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 0.0f, 1.0f);*/

	GYMatrix4 lookAt;


	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_RIGHT]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_RIGHT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_LEFT]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_LEFT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, -1.0, 0.0,  0.0,0.0, -1.0); // +Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BOTTOM]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_BOTTOM]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0,0.0, 1.0); // -Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_TOP]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_TOP]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BACK]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_BACK]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, -1.0,  0.0,1.0, 0.0); // -Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_FRONT]);
	glalTranspose(_pointShadowFaceMatrices[CUBE_MAP_FRONT]);

	glLoadIdentity();

/*
	gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 0.0f, 1.0f);*/
/*
	glLoadIdentity(); gluLookAt(-1.0, 0.0, 0.0,   0.0f, 0.0f, 0.0f,   0.0,0.0, 1.0); // +X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_RIGHT]);

	glLoadIdentity(); gluLookAt(1.0, 0.0, 0.0,  0.0, 0.0, 0.0,   0.0,0.0, 1.0); // -X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_LEFT]);

	glLoadIdentity(); gluLookAt(0.0, -1.0, 0.0,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0); // +Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BOTTOM]);

	glLoadIdentity(); gluLookAt(0.0, 1.0, 0.0,  0.0, 0.0, 0.0,  0.0, 0.0, -1.0); // -Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_TOP]);

	glLoadIdentity(); gluLookAt(  0.0, 0.0, 1.0,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0); // +Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BACK]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, -1.0,  0.0, 0.0, 0.0,   0.0, 0.0, 1.0); // -Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_FRONT]);*/

	/*
	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_RIGHT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_LEFT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0, 1.0); // +Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BOTTOM]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BACK]);

	*/

	initialized = 1;
}



#endif
\
