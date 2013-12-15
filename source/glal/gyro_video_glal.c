#include "gyro_platform.h"
#include "gyro_video.h"
#include "gyro_texture.h"
#include "gyro_matrix.h"
#include "gyro_matrix_stack.h"
#include "gyro_system.h"
#include "gyro_shader.h"
#include "glal/gyro_glal.h"
#include "glal/gyro_video_glal.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//OPENGL/OPENGL_ES COMMONS, BITCHES!

//Internal video state variables
static GYColor4 _rectColor[4];
static const GYTexture *_curTex = 0;
static unsigned int _projWidth = 0;
static unsigned int _projHeight = 0;
static GY_RECT_TEX_ORIENT _rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL;
static GYMatrix4 _projectionMatrix = { { 0.0f } };
static GYMatrix4 _modelViewMatrix = { { 0.0f } };

typedef enum __FF_SHADER_ATTRIB {
	FF_SHADER_POS,
	FF_SHADER_COLOR,
	FF_SHADER_TEX_COORD,
	FF_SHADER_NORMAL,
	FF_SHADER_ATTRIB_MAX
} _FF_SHADER_ATTRIB;

typedef enum __FF_SHADER_UNIFORM {
	FF_SHADER_PROJ_MAT,
	FF_SHADER_MODEL_VIEW_MAT,
	FF_SHADER_UNIFORM_MAX
} _FF_SHADER_UNIFORM;

//"Inherits" from GYShaderProgram
typedef struct __FixedFunctionShaderProgram {
	GYShaderProgram* program;
	GLint attribLoc[FF_SHADER_ATTRIB_MAX];
	GLint uniformLoc[FF_SHADER_UNIFORM_MAX];
} _FixedFunctionShaderProgram;

static _FixedFunctionShaderProgram _ffShader;
static _FixedFunctionShaderProgram _ffUntexShader;

static int							_fixedFunctionMode	= 1;
static _FixedFunctionShaderProgram*	_curFFShaderProg	= &_ffShader;
static GYShaderProgram*				_curShaderProg		= NULL;

/* Sets the global shader program. A null argument means we're switching
 * back to the fixed-function shaders managed internally.
 */
void gyVidSetCurrentShaderProgram(struct _GYShaderProgram*const program) {
	if(program) {
		_fixedFunctionMode = 0;
		_curShaderProg = program;
		glUseProgram(program->program);
	} else {
		_fixedFunctionMode = 1;
		glUseProgram(_curFFShaderProg->program->program);
	}
}

/* Maintains pointer to current fixed-function shader program we're
 * using. Only sets it to the current if we're in fixed-function mode
 * and if the shader program actually changed. We must also update
 * the modelViewMatrix when this happens
 */
void _setCurrentFFShader(_FixedFunctionShaderProgram*const program) {
	if(program != _curFFShaderProg) {
		_curFFShaderProg = program;
		if(_fixedFunctionMode) {
			glUseProgram(_curFFShaderProg->program->program);
			//glUniformMatrix4fv(_curFFShaderProg->uniformLoc[FF_SHADER_MODEL_VIEW_MAT], 1, GL_FALSE, (float*)&_modelViewMatrix);
		}
	}
}

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

void _glalOnMatApply(void) {
	//gyMatStore(&_modelViewMatrix);
	//glUniformMatrix4fv(_curFFShaderProg->uniformLoc[FF_SHADER_MODEL_VIEW_MAT], 1, GL_FALSE, (float*)&_modelViewMatrix);
}

static int _queryFixedFunctionShader(_FixedFunctionShaderProgram*const shader, const unsigned textured) {

	//poll uniform location(s)
	shader->uniformLoc[FF_SHADER_PROJ_MAT] = glGetUniformLocation(shader->program->program, "uProjMat");
	//shader->uniformLoc[FF_SHADER_MODEL_VIEW_MAT] = glGetUniformLocation(shader->program->program, "uModelViewMat");

	//poll attribute location(s)
	shader->attribLoc[FF_SHADER_POS] = glGetAttribLocation(shader->program->program, "aPosition");
	shader->attribLoc[FF_SHADER_COLOR] = glGetAttribLocation(shader->program->program, "aColor");
	//shader->_ffShader.attribLoc[FF_SHADER_NORMAL] = glGetAttribLocation(shader->program, "aNormal");
	if(textured) {
		shader->attribLoc[FF_SHADER_TEX_COORD] = glGetAttribLocation(shader->program->program, "aTexCoord");
	}

	/*_gyLog(GY_DEBUG_VERBOSE, "UNIFORM = %d\n", shader->uniformLoc[FF_SHADER_MODEL_VIEW_MAT]);

	for(unsigned i = 0; i < FF_SHADER_ATTRIB_MAX; ++i) {
		_gyLog(GY_DEBUG_VERBOSE, "ATTRIB[%d] = %d\n", i, shader->attribLoc[i]);
	}*/

	return 1;
}

static void _loadFixedFunctionShaders(void) {
	GYShader* vertShader = gyShaderCreate("shaders/fixedFunctionTextured.vsh");
	GYShader* fragShader = gyShaderCreate("shaders/fixedFunctionTextured.fsh");
	_ffShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryFixedFunctionShader(&_ffShader, 1);

	vertShader = gyShaderCreate("shaders/fixedFunctionUntextured.vsh");
	fragShader = gyShaderCreate("shaders/fixedFunctionUntextured.fsh");
	_ffUntexShader.program = gyShaderProgramCreate(vertShader, fragShader);
	_queryFixedFunctionShader(&_ffUntexShader, 0);

}

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
	*projWidth = _projWidth; *projHeight = _projHeight;
}

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {
	const float far = 1000.0f;
	const float near = -1000.0f;
	const float left = 0.0f;
	const float right = projWidth;
	const float top = 0.0f;
	const float bottom = projHeight;

	GYMatrix4 orthoProjMat = {
		{ 2.0f/(right-left), 0.0f, 0.0f, -(right+left)/(right-left) },
		{ 0.0f, 2.0f/(top-bottom), 0.0f, -(top+bottom)/(top-bottom) },
		{ 0.0f, 0.0f, -2.0f/(far-near), -(far+near)/(far-near) },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	memcpy(_projectionMatrix, orthoProjMat, sizeof(GYMatrix4));

    _projWidth = projWidth;
    _projHeight = projHeight;

    glClearColor(0, 0, 0, 0);
    glClearDepth (1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//glOrtho(0, projWidth, projHeight, 0, -999, 1000);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*glTexParameteri( GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S, 
                    GL_CLAMP ); */
	glLoadIdentity();
	//gyLoadIdentity();

	_loadFixedFunctionShaders();

	GYShaderProgram *prevProgram = _curShaderProg;
	_FixedFunctionShaderProgram* prevFFProgram = _curFFShaderProg;

	gyVidSetCurrentShaderProgram(NULL);
	_setCurrentFFShader(&_ffShader);
	glUniformMatrix4fv(_curFFShaderProg->uniformLoc[FF_SHADER_PROJ_MAT], 1, GL_TRUE, (float*)&_projectionMatrix);
	_setCurrentFFShader(&_ffUntexShader);
	glUniformMatrix4fv(_curFFShaderProg->uniformLoc[FF_SHADER_PROJ_MAT], 1, GL_TRUE, (float*)&_projectionMatrix);

	gyVidSetCurrentShaderProgram(prevProgram);
	_setCurrentFFShader(prevFFProgram);

    return 1;
}

// ==== LOW-LEVEL API =====

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
	_rectTexOrient = orient;
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
	GYVector4 modelViewVertices[amount];

	gyMatMultVec(vertexArray, modelViewVertices, amount);

	glVertexAttribPointer(_curFFShaderProg->attribLoc[FF_SHADER_POS], 4, GL_FLOAT, GL_FALSE, 0, modelViewVertices);
	glEnableVertexAttribArray(_curFFShaderProg->attribLoc[FF_SHADER_POS]);
	glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
	glVertexAttribPointer(_curFFShaderProg->attribLoc[FF_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(_curFFShaderProg->attribLoc[FF_SHADER_COLOR]);
}

void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr) {
	assert(_curTex); //You should not be submitting this to the untextured shader, dumbass!
	glVertexAttribPointer(_curFFShaderProg->attribLoc[FF_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(_curFFShaderProg->attribLoc[FF_SHADER_TEX_COORD]);
}

void gyVidTexBind(const GYTexture *const tex) {
    //GYMatrix4 texMatrix;
	_FixedFunctionShaderProgram *newShader = NULL;
	
	if(tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
		newShader = &_ffShader;
	}
	else {
		glDisable(GL_TEXTURE_2D);
		newShader = &_ffUntexShader;
	}

	_setCurrentFFShader(newShader);
	_curTex = tex;
}

void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev) {
	//Does this even do anything now that we're shader based?
    switch(lev) {
	case GY_ALPHA_NONE:
		glDisable(GL_BLEND);
		break;
	default:
		glEnable(GL_BLEND);
            break;
    }
}


void gyVidQuadListBegin(const GYColor4 *const color) {
	//Pad for to apply to each vertex
	_rectColor[0] = *color;
	_rectColor[1] = *color;
	_rectColor[2] = *color;
	_rectColor[3] = *color;
	gyVidColorArray(_rectColor);
}

//Don't do shit. Lolz.
void gyVidQuadListEnd(void) {

}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)

void gyVidQuad(const GYVector4 *const verts, const unsigned int frame) {
	const GYTexCoord2* coord;

	if(_curTex) {
		coord = gyVidTexCoordArrayFromFrame(&_curTex->_coord->texCoord[frame], _rectTexOrient);
		gyVidTexCoordArray(coord);
		_rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL; //reset after each rect
	}

	gyVidTriangleStrip(verts, 0, 4);
}

void gyVidRect(const unsigned int frame) {

	static const GYVector4 localVertArray[4] = {
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
	};

	gyVidQuad(localVertArray, frame);
}

