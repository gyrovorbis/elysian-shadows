#include <gyro_file_api.h>
#include <gyro_video_api.h>
#include <gyro_shader_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>
#include <stdio.h>

#include "gyro_video_glal.h"
#include "gyro_shaders_glal.h"

const GYMatrix4 shadowTexCoordBiasMat = {
	{ 0.5f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.5f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.5f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f },
};

// ========= DEFAULT UNIFORM STATE VALUES =======
const static GYMaterial _defaultMaterial = {
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
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

GYMatrix4 pointShadowFaceMatrices[GY_CUBE_MAP_FACE_MAX];


// ====== UNIFORM STATE DATA =======
static const GYMaterial*	_currentMaterial								= &_defaultMaterial;
static GYVector4			_globalAmbience									= { 0.0f, 0.0f, 0.0f, 1.0f };
static const GYTexture*		_normalMap										= NULL;
static const GYTexture*		_specularMap									= NULL;
GYVector3					camWorldPos										= { 0.0f, 0.0f, 0.0f };

// ==== INTERNAL STATE ===
GYShaderProgram*			_curShaderProg		= NULL;
_LightingShaderProgram		_lightingShader;
_LightingShaderProgram		_shadowMapShader;
_LightingShaderProgram*		curLightShader		= &_lightingShader;

// ====== PUBLIC LIGHTING API =======

void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program) {
	_curShaderProg = program;
	glUseProgram(program->program);
}

void gyVidSetDefaultShaderProgram(void) {
	gyVidSetCurrentShaderProgram(_lightingShader.program);
}

void gyVidSetCamWorldPos(const GYVector3*const pos) {
	if(pos->x == camWorldPos.x && pos->y == camWorldPos.y && pos->z == camWorldPos.z) return;
	camWorldPos = *pos;
	glUniform3fv(curLightShader->uniformLoc[SHADER_CAM_WORLD_POS], 1, (float*)&camWorldPos);
}

void gyVidNormalMapBind(const GYTexture * const tex) {
	static int wasEnabled = 0;
	int enabled = 1;

	glActiveTexture(GL_TEXTURE1);
	if(tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {
		enabled = 0;
		glDisable(GL_TEXTURE_2D);
	}
	if(enabled != wasEnabled) glUniform1i(curLightShader->uniformLoc[SHADER_NORMAL_MAP_ENABLED], enabled);
	_normalMap = tex;
	glActiveTexture(GL_TEXTURE0);

	wasEnabled = enabled;
}

void gyVidSpecularMapBind(const GYTexture * const tex) {
	static int wasEnabled = 0;
	int enabled = 1;

	glActiveTexture(GL_TEXTURE2);
	if(tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {
		enabled = 0;
		glDisable(GL_TEXTURE_2D);
	}
	if(enabled != wasEnabled) glUniform1i(curLightShader->uniformLoc[SHADER_SPECULAR_MAP_ENABLED], enabled);
	_specularMap = tex;
	glActiveTexture(GL_TEXTURE0);

	wasEnabled = enabled;
}


void glalLightingInit(void) {

	//Cache cube shadow map lookat matrices
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_RIGHT], 0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_LEFT], 0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_BOTTOM], 0.0, 0.0, 0.0,  0.0, -1.0, 0.0,  0.0,0.0, -1.0); // +Y
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_TOP], 0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0,0.0, 1.0); // -Y
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_BACK], 0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
	gyVidLookAtMat(pointShadowFaceMatrices[GY_CUBE_MAP_FRONT], 0.0, 0.0, 0.0,  0.0, 0.0, -1.0,  0.0,1.0, 0.0); // -Z
}

void gyVidMaterialBind(const GYMaterial*const material) {

	if(!material) gyVidMaterialBind(&_defaultMaterial);
	else if(_currentMaterial != material) {
		_currentMaterial = material;

		if(_currentMaterial) {
			glUniform1f(curLightShader->uniformLoc[SHADER_MATERIAL_SHININESS], material->shininess);
			glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_AMBIENT_COLOR], 1, (float*)&material->ambientColor);
			glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_DIFFUSE_COLOR], 1, (float*)&material->diffuseColor);
			glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_SPECULAR_COLOR], 1, (float*)&material->specularColor);
			glUniform4fv(curLightShader->uniformLoc[SHADER_MATERIAL_EMISSIVE_COLOR], 1, (float*)&material->emissiveColor);

		}
	}
}

void gyVidSetGlobalAmbience(const GYVector4*const color) {
	_globalAmbience = *color;
	glUniform4fv(curLightShader->uniformLoc[SHADER_AMBIENCE], 1, (float*)color);
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


static void _queryLightingShader(_LightingShaderProgram*const shader, const unsigned light) {
	char buff[100];

	//SHARED SHADER ATTRIBS/UNIFORMS
	shader->attribLoc[SHADER_POS]						= _protectedAttribQuery(shader->program, "aPosition");
	shader->uniformLoc[SHADER_PROJ_VIEW_MAT]			= _protectedUniformQuery(shader->program, "uProjViewMat");

	shader->attribLoc[SHADER_TEX_COORD]					= _protectedAttribQuery(shader->program, "aTexCoord");
	shader->uniformLoc[SHADER_TEXTURE_ENABLED]			= _protectedUniformQuery(shader->program, "uTextureEnabled");

	shader->attribLoc[SHADER_COLOR]						= _protectedAttribQuery(shader->program, "aColor");

	if(!light) return; //the rest is lighting only

	//poll for global uniform locations
	shader->uniformLoc[SHADER_CAM_WORLD_POS]			= _protectedUniformQuery(shader->program, "uCamWorldPos");
	shader->uniformLoc[SHADER_NORMAL]					= _protectedUniformQuery(shader->program, "uNormal");
	shader->uniformLoc[SHADER_TANGENT]					= _protectedUniformQuery(shader->program, "uTangent");
	shader->uniformLoc[SHADER_AMBIENCE]					= _protectedUniformQuery(shader->program, "uAmbience");
	shader->uniformLoc[SHADER_LIGHTING_ENABLED]			= _protectedUniformQuery(shader->program, "uLightingEnabled");
	shader->uniformLoc[SHADER_NORMAL_MAP_ENABLED]		= _protectedUniformQuery(shader->program, "uNormalMapEnabled");
	shader->uniformLoc[SHADER_NORMAL_MAP]				= _protectedUniformQuery(shader->program, "uNormalMap");
	shader->uniformLoc[SHADER_SPECULAR_MAP_ENABLED]		= _protectedUniformQuery(shader->program, "uSpecularMapEnabled");
	shader->uniformLoc[SHADER_SPECULAR_MAP]				= _protectedUniformQuery(shader->program, "uSpecularMap");
	shader->uniformLoc[SHADER_SHADOW_CUBE_MAP_PROJ_MAT] = _protectedUniformQuery(shader->program, "uShadowCubeMapProjMat");

	//poll for GYMaterial uniform locations
	shader->uniformLoc[SHADER_MATERIAL_SHININESS]		= _protectedUniformQuery(shader->program, "uMaterial.shininess");
	shader->uniformLoc[SHADER_MATERIAL_AMBIENT_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.ambientColor");
	shader->uniformLoc[SHADER_MATERIAL_DIFFUSE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.diffuseColor");
	shader->uniformLoc[SHADER_MATERIAL_SPECULAR_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.specularColor");
	shader->uniformLoc[SHADER_MATERIAL_EMISSIVE_COLOR]	= _protectedUniformQuery(shader->program, "uMaterial.emissiveColor");

	//poll for GYLightSource uniform locations
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {

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
	GYShader* vertShader = gyShaderCreate("/assets/shaders/pixelLighting.vsh");
	GYShader* fragShader = gyShaderCreate("/assets/shaders/pixelLighting.fsh");
	//GYShader* vertShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/pixelLighting.vsh");
	//GYShader* fragShader = gyShaderCreate("/Users/Falco/Documents/ESDev/libGyro/source/glal/shaders/pixelLighting.fsh");
	_lightingShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryLightingShader(&_lightingShader, 1);

	vertShader = gyShaderCreate("/assets/shaders/shadowMap.vsh");
	fragShader = gyShaderCreate("/assets/shaders/shadowMap.fsh");
	_shadowMapShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryLightingShader(&_shadowMapShader, 0);
}

void glalDefaultShadersLoadUniforms(void) {
	GYLightSource* sources[GYRO_VIDEO_LIGHT_SOURCES_MAX];
	int count = lightSourceCount;
	const GYMaterial* material = _currentMaterial;
	_currentMaterial = NULL;

	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)curCamera->projViewMat);
//	glUniform4fv(curLightShader->uniformLoc[SHADER_CAM_POS], 1, (float*)&viewPosition);
	gyVidMaterialBind(material);

	memcpy(sources, lightSources, sizeof(GYLightSource*) * count);

	//Gid rid of them and readd
	gyVidLightSourceClear();
	for(int i = 0; i < count; ++i) {
		gyVidLightSourceAdd(sources[i]);
	}

	gyVidSetGlobalAmbience(&_globalAmbience);
	gyVidLightingSetEnable(lightingEnabled);

	glUniform3fv(curLightShader->uniformLoc[SHADER_CAM_WORLD_POS], 1, (float*)&camWorldPos);
	glUniform3fv(curLightShader->uniformLoc[SHADER_NORMAL], 1, (float*)normalVector);
	glUniform3fv(curLightShader->uniformLoc[SHADER_TANGENT], 1, (float*)tangentVector);

	//set normal map to use texture bank 1
	glUniform1i(curLightShader->uniformLoc[SHADER_NORMAL_MAP], 1);
	glUniform1i(curLightShader->uniformLoc[SHADER_SPECULAR_MAP], 2);

	//next texture banks are used for shadow maps for each light
	for(unsigned i = 0; i < GYRO_VIDEO_LIGHT_SOURCES_MAX; ++i) {
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_MAP], 3+i);
		glUniform1i(curLightShader->lightsUniformLoc[i][SHADER_LIGHT_SHADOW_CUBE_MAP], 7+i);
	}

	//Do these not need to be physically resubmitted?
	glUniform1i(curLightShader->uniformLoc[SHADER_TEXTURE_ENABLED], (curTex)? 1 : 0);
	glUniform1i(curLightShader->uniformLoc[SHADER_NORMAL_MAP_ENABLED], (_normalMap)? 1 : 0);
	glUniform1i(curLightShader->uniformLoc[SHADER_SPECULAR_MAP_ENABLED], (_specularMap)? 1 : 0);

	GYMatrix4 projMat;
	GYMatrix4 finalMat;
	glalGetPointLightProjectionMatrix(projMat, NULL);
	gyMatMultMat(finalMat, shadowTexCoordBiasMat, projMat);
	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_SHADOW_CUBE_MAP_PROJ_MAT], 1, GL_FALSE, (float*)projMat); //NO TEX COORD OFFSET? WHAT THE FUCK!?

	//Initialize internal data for shadow shader
	glalLightingInit();

}

// ======== SHADER ALLOCATION AND MANAGEMENT ========

int _gyVidShaderCreate(GYShader* shader) {
	char path[GYRO_PATH_MAX_SIZE];
	GLint compiled;
	const char* extension = NULL;
	const char* token = NULL;
    GYFile *file;
    size_t charCount = 0;
    char *programSource = NULL;

	//don't fuck it up by tokenizing
	strcpy(path, shader->path);
	strtok(path, ".");

	while((token = strtok(NULL, "."))) {
		extension = token;
	}

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

	if(!gyFileOpen(shader->path, "r", &file)) {
		_gyLog(GY_DEBUG_CRITICAL, "\tCould not open shader", shader->path);
		return 0;
	}
    gyFileLength(file, &charCount);
	programSource = malloc(sizeof(char) * charCount + 1);
    gyFileRead(file, programSource, charCount, NULL);
	programSource[charCount] = '\0';
    gyFileClose(&file);

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

