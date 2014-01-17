#include <gyro_video_api.h>
#include <gyro_shader_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>
#include <stdio.h>

#include "gyro_video_glal.h"
#include "gyro_shaders_glal.h"

// ========= DEFAULT UNIFORM STATE VALUES =======
const static GYMaterial _defaultMaterial = {
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	1.0f
};

const static GYLightProperties _emptyLightProperties = {
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },
	1.0f,
};

const static GYLightSource _emptyLight = {
	{
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	},
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f },
	360.0f,
	&_emptyLightProperties
};

const static GYVector3 _defaultNormal = {
  0.0f, 0.0f, 1.0f
};

// ====== UNIFORM STATE DATA =======
static GYLightSource*		_lightSources[GYRO_VIDEO_LIGHT_SOURCES_MAX] = { NULL };
static unsigned				_lightSourceCount							= 0;
static int					_lightingEnabled							= 1;
static int					_lightsEnabled[SHADER_LIGHTS_PER_PASS]		= { 0 };
static const GYMaterial*	_currentMaterial							= &_defaultMaterial;
static GYVector4			_globalAmbience								= { 0.0f, 0.0f, 0.0f, 1.0f };
static const GYTexture*		_normalMap									= NULL;
static GYShaderProgram*		_curShaderProg								= NULL;

static _LightingShaderProgram _lightingShader;
static _LightingShaderProgram _shadowMapShader;
_LightingShaderProgram*	curLightShader									= &_lightingShader;
static int _curShadowPass = 0;
static GLuint _shadowFBOs[GYRO_VIDEO_LIGHT_SOURCES_MAX] = { 0 };
static GLuint _shadowMaps[GYRO_VIDEO_LIGHT_SOURCES_MAX] = { 0 };
static GLuint _shadowCubeMaps[GYRO_VIDEO_LIGHT_SOURCES_MAX] = { 0 };
typedef enum __CUBE_MAP_FACES {
	CUBE_MAP_LEFT,
	CUBE_MAP_RIGHT,
	CUBE_MAP_BACK,
	CUBE_MAP_BOTTOM,
	CUBE_MAP_FACES_MAX
} _CUBE_MAP_FACES;
static GYMatrix4 _pointShadowFaceMatrices[CUBE_MAP_FACES_MAX];
static _CUBE_MAP_FACES _curPointShadowFace = CUBE_MAP_FACES_MAX;

const static GLenum _shadowMapTexUnits[GYRO_VIDEO_LIGHT_SOURCES_MAX] = {
	GL_TEXTURE2,
	GL_TEXTURE3,
	GL_TEXTURE4,
	GL_TEXTURE5
};

// ====== PUBLIC LIGHTING API =======

void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program) {
	_curShaderProg = program;
	glUseProgram(program->program);
}

void gyVidSetDefaultShaderProgram(void) {
	gyVidSetCurrentShaderProgram(_lightingShader.program);
}

void gyVidNormalMapBind(const GYTexture * const tex) {
	glActiveTexture(GL_TEXTURE1);
	int enabled;
	/*if(tex) {
		enabled = 1;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {*/
		enabled = 0;
		glDisable(GL_TEXTURE_2D);
	//}
	glUniform1i(curLightShader->uniformLoc[SHADER_NORMAL_MAP_ENABLED], enabled);
	_normalMap = tex;
	glActiveTexture(GL_TEXTURE0);
}

void gyVidLightingSetEnable(const int enabled) {
	_lightingEnabled = enabled;
	glUniform1i(curLightShader->uniformLoc[SHADER_LIGHTING_ENABLED], enabled);
}

void gyVidLightSourceClear(void) {
	_lightSourceCount = 0;
	for(unsigned i = 0; i < SHADER_LIGHTS_PER_PASS; ++i) {
		_lightsEnabled[i] = 0;
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_ENABLED], 0);
	}
}

void _addLightUniform(const unsigned index, const GYLightSource*const light) {
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_POS], 1, (float*)&light->position);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_AMBIENT_COLOR], 1, (float*)&light->properties->ambientColor);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_DIFFUSE_COLOR], 1, (float*)&light->properties->diffuseColor);
	glUniform4fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPECULAR_COLOR], 1, (float*)&light->properties->specularColor);

	glUniform3fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_DIRECTION], 1, (float*)&light->direction);
	glUniform3fv(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_ATTENUATION_FACTORS], 1, (float*)&light->properties->attenuationFactors);

	glUniform1f(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPOT_EXPONENT], light->properties->spotExponent);
	glUniform1f(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_SPOT_CUTOFF_ANGLE], light->spotCutoffAngle);

	glUniform1i(curLightShader->lightsUniformLoc[index][SHADER_LIGHT_ENABLED], 1);

	glActiveTexture(_shadowMapTexUnits[index]);
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _shadowMaps[index]);
	glActiveTexture(GL_TEXTURE0);

}

void gyVidLightSourceAdd(const GYLightSource* const light) {
	assert(_lightSourceCount < GYRO_VIDEO_LIGHT_SOURCES_MAX);
	_lightSources[_lightSourceCount] = light;

	_addLightUniform(_lightSourceCount, light);
	++_lightSourceCount;
}

void gyVidMaterialBind(const GYMaterial*const material) {
	_currentMaterial = material;

	glUniform1f(curLightShader->uniformLoc[SHADER_MATERIAL_SHININESS], material->shininess);
	glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_AMBIENT_COLOR], 1, (float*)&material->ambientColor);
	glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_DIFFUSE_COLOR], 1, (float*)&material->diffuseColor);
	glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_SPECULAR_COLOR], 1, (float*)&material->specularColor);
	glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_EMISSIVE_COLOR], 1, (float*)&material->emissiveColor);
}

void gyVidSetGlobalAmbience(const GYVector4*const color) {
	_globalAmbience = *color;
	glUniform4fv(curLightShader->uniformLoc[SHADER_AMBIENCE], 1, (float*)color);
}

static inline void ComputeNormalOfPlane(float *normal, const float *pvector1, const float *pvector2)
{
	normal[0]=(pvector1[1]*pvector2[2])-(pvector1[2]*pvector2[1]);
	normal[1]=(pvector1[2]*pvector2[0])-(pvector1[0]*pvector2[2]);
	normal[2]=(pvector1[0]*pvector2[1])-(pvector1[1]*pvector2[0]);
}

static inline void NormalizeVector(float *pvector)
{
	float normalizingConstant;
	normalizingConstant=1.0/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]+pvector[2]*pvector[2]);
	pvector[0]*=normalizingConstant;
	pvector[1]*=normalizingConstant;
	pvector[2]*=normalizingConstant;
}

//Fucking fix me!!!
static void _glhLookAtf2( float *matrix, float *eyePosition3D,
				  float *center3D, float *upVector3D )
{
   float forward[3], side[3], up[3];
   GYMatrix4 simdMat1, simdMat2, simdMat3;
   float* matrix2 = simdMat1;
   float* resultMatrix = simdMat2;
   //------------------
   forward[0] = center3D[0] - eyePosition3D[0];
   forward[1] = center3D[1] - eyePosition3D[1];
   forward[2] = center3D[2] - eyePosition3D[2];
   NormalizeVector(forward);
   //------------------
   //Side = forward x up
   ComputeNormalOfPlane(side, forward, upVector3D);
   NormalizeVector(side);
   //------------------
   //Recompute up as: up = side x forward
   ComputeNormalOfPlane(up, side, forward);
   //------------------
   matrix2[0] = side[0];
   matrix2[4] = side[1];
   matrix2[8] = side[2];
   matrix2[12] = 0.0;
   //------------------
   matrix2[1] = up[0];
   matrix2[5] = up[1];
   matrix2[9] = up[2];
   matrix2[13] = 0.0;
   //------------------
   matrix2[2] = -forward[0];
   matrix2[6] = -forward[1];
   matrix2[10] = -forward[2];
   matrix2[14] = 0.0;
   //------------------
   matrix2[3] = matrix2[7] = matrix2[11] = 0.0;
   matrix2[15] = 1.0;
   //------------------
   /*
   MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, matrix2);
   glhTranslatef2(resultMatrix,
				  -eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);*/


   for(unsigned i = 0; i < 4; ++i) for(unsigned j = 0; j < 4; ++j) simdMat3[i][j] = simdMat1[i][j];

   gyMatMultMat(&simdMat2, matrix, &simdMat3);
   gyMatPush();
   gyMatLoad(&simdMat2);
   gyMatTranslate(-eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);
   gyMatStore(matrix);
   gyMatPop();
   //------------------
  // memcpy(matrix, resultMatrix, 16*sizeof(float));
}

const int shadowWidth = 1280;
const int shadowHeight = 720;

void _glalShadowMultiPassInit(void) {
	static int initialized = 0;

	if(initialized) {
		glDeleteTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowMaps);
		glDeleteFramebuffers(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowFBOs);
	}

	glGenFramebuffers(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowFBOs);
	glGenTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowMaps);
	glGenTextures(GYRO_VIDEO_LIGHT_SOURCES_MAX, _shadowCubeMaps);

	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {

		glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBOs[i]);

		glBindTexture(GL_TEXTURE_2D, _shadowMaps[i]);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		 glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		 glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 1.0f);

		//initialize cubemaps
		 glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMaps[i]);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		 //probably should be intensity?
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		 //Does this REALLY have to have 6 faces!?
		 for (size_t i = 0; i < CUBE_MAP_FACES_MAX; ++i) {
			 //shadowDepth? lolz
			 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, shadowWidth, shadowHeight, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		 }
		 glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	 }
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glActiveTexture(GL_TEXTURE7);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMaps[0]);
	glUniform1i(curLightShader->lightsUniformLoc[0][SHADER_LIGHT_SHADOW_CUBE_MAP], 50);
	//glActiveTexture(GL_TEXTURE0);

	// create point light face view matrices
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_RIGHT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_LEFT]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0, 1.0); // +Y
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BOTTOM]);

	glLoadIdentity(); gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_pointShadowFaceMatrices[CUBE_MAP_BACK]);

	initialized = 1;
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

		if(lightSrc->spotCutoffAngle <= 180) {
			gyMatPush();
			gyMatLoadIdentity();

			//glScalef(-1.0f, -1.0f, 0.0f);

			gyMatRotateZ(0.785398163f);

			gluLookAt(-lightSrc->direction.x, -lightSrc->direction.y, -lightSrc->direction.z,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)temp);
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
	gluPerspective(90, (float)shadowWidth/(float)shadowHeight, 0.5f, 150.0f);//150.0f
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)test);

	memcpy(test, dest, sizeof(GYMatrix4));
}

void glalGetLightProjectionMatrix(GYMatrix4* dest, const GYLightSource*const lightSrc) {
	if(lightSrc->position.w == 0) {
		glalGetLightOrthoProjectionMatrix(dest, lightSrc);
	} else {
		if(lightSrc->spotCutoffAngle <= 180) {
			glalGetLightPerspectiveProjectionMatrix(dest, lightSrc);
			glalGetLightOrthoProjectionMatrix(dest, lightSrc);
		} else {

		}
	}
}

int gyVidBeginNextShadowPass(void) {
	GYMatrix4 lightProjMatrix;
	GYMatrix4 lightViewMatrix;
	GYMatrix4 lightProjViewMatrix;

	GYVector3 center3D = { 0.0f, 0.0f, 0.0f };
	GYVector3 upVector3D = { 0.0f, -1.0f, 0.0f };

	static const GYMatrix4 shadowTexCoordBiasMat = {
		{ 0.5f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.5f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.5f, 0.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f },
	};

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

		//SUBMIT PROJVIEW UNIFORM TO LIGHT SHADER
		for(unsigned i = 0; i < _lightSourceCount; ++i)
		glUniformMatrix4fv(_lightingShader.lightsUniformLoc[i][SHADER_LIGHT_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)&_lightSources[i]->projViewMat);
		return 0;
	}

	GYLightSource* curLight = _lightSources[_curShadowPass];

	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBOs[_curShadowPass]);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Determine whether we're processing a point shadow's cubemap
	if(curLight->spotCutoffAngle > 180.0f &&
			curLight->position.w != 0.0f) {

		if(_curPointShadowFace == CUBE_MAP_FACES_MAX)
			_curPointShadowFace == CUBE_MAP_LEFT;

		//Create Projection matrix
		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadIdentity();
		gluPerspective(90.0f, (float)shadowWidth/(float)shadowHeight, 0.5f, 150.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)lightProjMatrix);

		//Create View Matrix
		gyMatPush();
		gyMatLoadIdentity();
		gyMatLoad(_pointShadowFaceMatrices[_curPointShadowFace]);

		gyMatTranslate(-curLight->position.x, -curLight->position.y, -curLight->position.z);

		gyMatStore(lightViewMatrix);
		gyMatPop();

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_CUBE_MAP_POSITIVE_X + _curPointShadowFace,
							   _shadowCubeMaps[_curShadowPass],
							   0);

	} else {

		glalGetLightProjectionMatrix(lightProjMatrix, curLight);
		glalGetLightViewMatrix(lightViewMatrix, curLight);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D,
							   _shadowMaps[_curShadowPass],
							   0);
	}

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	glReadBuffer(GL_NONE);

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
	gyMatMultMat(&lightProjViewMatrix, &lightProjMatrix, &lightViewMatrix);

	//SUBMIT PROJVIEW UNIFORM TO SHADOW SHADER
	glUniformMatrix4fv(_shadowMapShader.uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)lightProjViewMatrix);

	//Store projection view matrix (plus bias) for next render pass
	gyMatMultMat(&curLight->projViewMat, &shadowTexCoordBiasMat, &lightProjViewMatrix);

	if(_curPointShadowFace != CUBE_MAP_FACES_MAX) ++_curPointShadowFace;
	if(_curPointShadowFace == CUBE_MAP_FACES_MAX) ++_curShadowPass;

	return 1;
}


//======= INTERNAL QUERY FUNCTIONS FOR DEFAULT SHADER =======

static inline int _protectedUniformQuery(const GYShaderProgram* prog, const char* const name) {
	int loc = glGetUniformLocation(prog->program, name);
	if(loc == -1) {
		_gyLog(GY_DEBUG_CRITICAL, "Query for shader uniform %s failed!", name);
	}
	return loc;
}

static inline int _protectedAttribQuery(const GYShaderProgram* prog, const char* const name) {
	int loc = glGetAttribLocation(prog->program, name);
	if(loc == -1) {
		_gyLog(GY_DEBUG_CRITICAL, "Query for shader attribute %s failed!", name);
	}
	return loc;
}


static void _queryLightingShader(_LightingShaderProgram*const shader) {
	char buff[100];

	//poll for attribute locations
	shader->attribLoc[SHADER_POS]		= _protectedAttribQuery(shader->program, "aPosition");
	shader->attribLoc[SHADER_COLOR]		= _protectedAttribQuery(shader->program, "aColor");
	shader->attribLoc[SHADER_TEX_COORD] = _protectedAttribQuery(shader->program, "aTexCoord");

	//poll for global uniform locations
	shader->uniformLoc[SHADER_NORMAL]					= _protectedUniformQuery(shader->program, "uNormal");
	shader->uniformLoc[SHADER_TANGENT]					= _protectedUniformQuery(shader->program, "uTangent");
	shader->uniformLoc[SHADER_PROJ_VIEW_MAT]			= _protectedUniformQuery(shader->program, "uProjViewMat");
	shader->uniformLoc[SHADER_AMBIENCE]					= _protectedUniformQuery(shader->program, "uAmbience");
	shader->uniformLoc[SHADER_LIGHTING_ENABLED]			= _protectedUniformQuery(shader->program, "uLightingEnabled");
	shader->uniformLoc[SHADER_NORMAL_MAP_ENABLED]		= _protectedUniformQuery(shader->program, "uNormalMapEnabled");
	shader->uniformLoc[SHADER_NORMAL_MAP]				= _protectedUniformQuery(shader->program, "uNormalMap");
	shader->uniformLoc[SHADER_SHADOW_CUBE_MAP_PROJ_MAT] = _protectedUniformQuery(shader->program, "uShadowCubeMapProjMat");

	//poll for GYMaterial uniform locations
	shader->uniformLoc[SHADER_MATERIAL_SHININESS]		= _protectedUniformQuery(shader->program, "uMaterial.shininess");
	shader->uniformLoc[SHADER_MATERIAL_AMBIENT_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.ambientColor");
	shader->uniformLoc[SHADER_MATERIAL_DIFFUSE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.diffuseColor");
	shader->uniformLoc[SHADER_MATERIAL_SPECULAR_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.specularColor");
	shader->uniformLoc[SHADER_MATERIAL_EMISSIVE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.emissiveColor");

	//poll for GYLightSource uniform locations
	for(unsigned i = 0; i < SHADER_LIGHTS_PER_PASS; ++i) {

		sprintf(buff, "uLights[%d].position", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_POS]							= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].ambientColor", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_AMBIENT_COLOR]					= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].diffuseColor", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_DIFFUSE_COLOR]					= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].specularColor", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_SPECULAR_COLOR]				= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].direction", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_DIRECTION]						= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].attenuationFactors", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_ATTENUATION_FACTORS]			= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].spotExponent", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_SPOT_EXPONENT]					= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLights[%d].spotCutoffAngle", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_SPOT_CUTOFF_ANGLE]				= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLightsEnabled[%d]", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_ENABLED]						= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uShadowMaps[%d]", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_MAP]					= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uShadowCubeMaps[%d]", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_CUBE_MAP]				= _protectedUniformQuery(shader->program, buff);

		sprintf(buff, "uLightProjViewMats[%d]", i);
		shader->lightsUniformLoc[i][SHADER_LIGHT_PROJ_VIEW_MAT]					= _protectedUniformQuery(shader->program, buff);

	}
}


// =========== PUBLIC INTERNAL FUNCTIONS ==========
void glalLoadDefaultShaders(void) {
	GYShader* vertShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/pixelLighting.vsh");
	GYShader* fragShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/pixelLighting.fsh");
	_lightingShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryLightingShader(&_lightingShader);

	vertShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/shadowMap.vsh");
	fragShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/shadowMap.fsh");
	_shadowMapShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryLightingShader(&_shadowMapShader);
}

void glalDefaultShadersLoadUniforms(void) {
	GYLightSource* sources[GYRO_VIDEO_LIGHT_SOURCES_MAX];
	int count = _lightSourceCount;

	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)projectionViewMatrix);
//	glUniform4fv(curLightShader->uniformLoc[SHADER_CAM_POS], 1, (float*)&viewPosition);
	gyVidMaterialBind(_currentMaterial);

	memcpy(sources, _lightSources, sizeof(GYLightSource*) * count);

	//Gid rid of them and readd
	gyVidLightSourceClear();
	for(int i = 0; i < count; ++i) {
		gyVidLightSourceAdd(sources[i]);
	}

	gyVidSetGlobalAmbience(&_globalAmbience);
	gyVidLightingSetEnable(_lightingEnabled);

	glUniform3fv(curLightShader->uniformLoc[SHADER_NORMAL], 1, normalVector);
	glUniform3fv(curLightShader->uniformLoc[SHADER_TANGENT], 1, tangentVector);

	//set normal map to use texture bank 1
	glUniform1i(curLightShader->uniformLoc[SHADER_NORMAL_MAP], 1);

	//next texture banks are used for shadow maps for each light
	for(unsigned i = 0; i < SHADER_LIGHTS_PER_PASS; ++i) {
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_MAP], 2+i);
		//glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_CUBE_MAP], 6+i);
	}

	GYMatrix4 projMatrix;
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	gluPerspective(90.0f, (float)shadowWidth/(float)shadowHeight, 0.5f, 150.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)projMatrix);
	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_SHADOW_CUBE_MAP_PROJ_MAT], 1, GL_FALSE, (float*)projMatrix);

	//Initialize internal data for shadow shader
	_glalShadowMultiPassInit();

}

// ======== SHADER ALLOCATION AND MANAGEMENT ========

int _gyVidShaderCreate(GYShader* shader) {
	char path[GYRO_PATH_MAX_SIZE];
	GLint compiled;
	const char* extension = NULL;

	//don't fuck it up by tokenizing
	strcpy(path, shader->path);
	strtok(path, ".");
	extension = strtok(NULL, ".");

	_gyLog(GY_DEBUG_VERBOSE, "Compiling shader: %s", shader->path);

	if(strcmp(extension, "vsh") == 0) {
		shader->type = GL_VERTEX_SHADER;
	} else if(strcmp(extension, "fsh") == 0) {
		shader->type = GL_FRAGMENT_SHADER;
	} else {
		_gyLog(GY_DEBUG_CRITICAL, "\tUnrecognized shader extension: %s", extension);
		return 0;
	}

	shader->shader = glCreateShader(shader->type);
	if(!shader->shader)  {
		_gyLog(GY_DEBUG_CRITICAL, "\tCould not generate shader", shader->path);
		return 0;
	}

	FILE* file = fopen(shader->path, "r");
	if(!file) {
		_gyLog(GY_DEBUG_CRITICAL, "\tCould not open shader", shader->path);
		return 0;
	}
	fseek(file , 0L , SEEK_END);
	long charCount = ftell(file);
	rewind(file);
	char* programSource = malloc(sizeof(char)*charCount+1);
	fread(programSource, charCount, 1 , file);
	programSource[charCount] = '\0';

	glShaderSource(shader->shader, 1, (const char*const*)&programSource, NULL);
	glCompileShader(shader->shader);

	glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		GLint infoLen = 0;

		glGetShaderiv(shader->shader, GL_INFO_LOG_LENGTH, &infoLen);

		if(infoLen > 1) {
			char* infoLog = malloc(sizeof(char)*infoLen);

			glGetShaderInfoLog(shader->shader, infoLen, NULL, infoLog);
			_gyLog(GY_DEBUG_CRITICAL, "\tError compiling shader: %s", infoLog);
			free(infoLog);
		} else {
			_gyLog(GY_DEBUG_CRITICAL, "\tUnknown compilation error");
		}

		glDeleteShader(shader->shader);
		return 0;
	}

	return 1;
}

int _gyVidShaderDestroy(GYShader* shader) {
	glDeleteShader(shader->shader);
	return 1;
}

int _gyVidShaderProgramCreate(GYShaderProgram* program) {
	GLint linked;

	_gyLog(GY_DEBUG_VERBOSE, "Linking shader program");

	program->program = glCreateProgram();
	if(!program->program) {
		_gyLog(GY_DEBUG_CRITICAL, "\tCould not generate program!");
		return 0;
	}

	glAttachShader(program->program, program->vertShader->shader);
	glAttachShader(program->program, program->fragShader->shader);

	glLinkProgram(program->program);

	glGetProgramiv(program->program, GL_LINK_STATUS, &linked);

	if(!linked) {
		GLint infoLen = 0;
		glGetProgramiv(program->program, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1) {
			char* infoLog = malloc(sizeof(char) * infoLen);

			glGetProgramInfoLog(program->program, infoLen, NULL, infoLog);
			_gyLog(GY_DEBUG_CRITICAL, "\tCould not link program: %s", infoLog);
			free(infoLog);

		} else {
			_gyLog(GY_DEBUG_CRITICAL, "\tCould not link program for unknown reasons!");
		}

		glDeleteProgram(program->program);
		return 0;
	}
	return 1;
}

int _gyVidShaderProgramDestroy(GYShaderProgram* program) {
	glDeleteProgram(program->program);
	return 1;
}

