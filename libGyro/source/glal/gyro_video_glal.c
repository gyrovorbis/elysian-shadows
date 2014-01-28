#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_audio_api.h>
#include <gyro_system_api.h>
#include <gyro_shader_api.h>
#include "gyro_video_glal.h"
#include "gyro_shaders_glal.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//OPENGL/OPENGL_ES COMMONS, BITCHES!
static const GYVector3 _rectNormals[GY_RECT_NORMAL_MAX] = {
	{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f }
};

static const GYVector3 _rectTangents[GY_RECT_TEX_ORIENT_MAX] = {
		{ 1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		/* 90 degrees*/
		{ 0.0f, 1.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		/* flip horiz, rot 90 */
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
};

static const GYVector4 _rectVerts[GY_RECT_NORMAL_MAX][4] = {
	//FLAT
	{
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
	},
	//DEPTH
	{
		{-0.5f, -0.5f, 1.0f, 1.0f},
		{0.5f, -0.5f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
	}
};

//Shared video state variables
const GYVector3*			tangentVector			= &_rectTangents[0];
const GYVector3*			normalVector			= &_rectNormals[0];
GYMatrix4					projectionViewMatrix	= { { 0.0f } };
GYVector4					viewPosition			= { 0.0f, 0.0f, 0.0f, 0.0f };
float						viewZOrient				= 0.0f;
float						viewScale				= 0.0f;
GYMatrix4					viewMatrix				= { { 0.0f } };
GYMatrix4					projectionMatrix		= { { 0.0f } };
const GYTexture*			curTex					= 0;

//Private video state variables
static GYColor4				_rectColor[4];
static unsigned int			_projWidth				= 0;
static unsigned int			_projHeight				= 0;
static GY_RECT_TEX_ORIENT	_rectTexOrient			= GY_RECT_TEX_ORIENT_NORMAL;
static GY_RECT_NORMAL_TYPE	_normalType				= GY_RECT_NORMAL_FLAT;
static GYColor4				_curQuadListColor[4]	= { {0} };

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
	*projWidth = _projWidth; *projHeight = _projHeight;
}

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {
	const float pfar = 1000.0f;
	const float pnear = -999.0f;
    const float pleft = 0.0f;
	const float pright = projWidth;
    const float ptop = 0.0f;
    const float pbottom = projHeight;

	gyVidOrthoProjMat(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);

//God fucking damnit, Windows....
#ifdef _WIN32
    _gyLog(GY_DEBUG_VERBOSE, "Initializing GLEW...");
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      /* Problem: glewInit failed, something is seriously wrong. */
        _gyLog(GY_DEBUG_CRITICAL, "\tError: %s", glewGetErrorString(err));
    }

    if(GLEW_VERSION_2_1) {
        _gyLog(GY_DEBUG_VERBOSE, "\tOpenGL 2.1 Extensions detected");
    } else {
        _gyLog(GY_DEBUG_CRITICAL, "\tOpenGL 2.1 Extensions NOT detected! Prepare for crash!");
    }

#endif

    _projWidth = projWidth;
    _projHeight = projHeight;

    glClearColor(0, 0, 0, 0);
	glClearDepth (1.0f);
    glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS)​;
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glLoadIdentity();

	glalLoadDefaultShaders();
	gyVidSetDefaultShaderProgram();
	glalDefaultShadersLoadUniforms();

    return 1;
}

// ==== LOW-LEVEL API =====

void gyVidCameraCustomViewMatrix(const GYMatrix4 mat) {
//	GYMatrix4 bullshit;
	gyMatMultMat(projectionViewMatrix, projectionMatrix, mat);
	memcpy(viewMatrix, mat, sizeof(GYMatrix4));
//	memcpy(bullshit, projectionViewMatrix, sizeof(GYMatrix4));
	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)projectionViewMatrix);
}

void gyVidCameraSetViewCenter(const GYVector3*const pos, const float scale, const float zOrient) {
	unsigned scrWidth, scrHeight;
	GYMatrix4 result;
	GYVector4 pos4 = {
		pos->x, pos->y, pos->z, 1.0f
	};

	//Save shit for other subsystems
	memcpy(&viewPosition, &pos4, sizeof(GYVector4));
	viewScale = scale;
	viewZOrient = zOrient;

	gyVidResolution(&scrWidth, &scrHeight);

	//Calculate view matrix
	gyMatPush();
	gyMatLoadIdentity();
	gyMatTranslate((float)scrWidth*0.5f, (float)scrHeight*0.5f, 0.0f);
	gyMatScale(scale, scale, 1.0f);
	gyMatRotateZ(zOrient);
	gyMatTranslate(-pos->x, -pos->y, 0.0f);
	gyMatStore(&result);
	gyMatPop();

	gyVidCameraCustomViewMatrix(result);
}

void gyVidProjectionCustomMatrix(const GYMatrix4 mat) {
	//GYMatrix4 result;
	gyMatMultMat(projectionViewMatrix, mat, viewMatrix);
	//memcpy(projectionViewMatrix, result, sizeof(GYMatrix4));
	memcpy(projectionMatrix, mat, sizeof(GYMatrix4));
	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)projectionViewMatrix);
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
	_rectTexOrient = orient;
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
	GYVector4 modelViewVertices[amount];

	gyMatMultVec(vertexArray, modelViewVertices, amount);

	glVertexAttribPointer(curLightShader->attribLoc[SHADER_POS], 4, GL_FLOAT, GL_FALSE, 0, modelViewVertices);
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_POS]);
	glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
	glVertexAttribPointer(curLightShader->attribLoc[SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_COLOR]);
}

void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr) {
	assert(curTex); //You should not be submitting this to the untextured shader, dumbass!
	glVertexAttribPointer(curLightShader->attribLoc[SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_TEX_COORD]);
}

/*
void gyVidNormalArray(const GYVector3* const ptr) {
	glVertexAttribPointer(lightingShader.attribLoc[SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(lightingShader.attribLoc[SHADER_NORMAL]);
}

void gyVidTangentArray(const GYVector3 *const ptr) {
	glVertexAttribPointer(lightingShader.attribLoc[SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(lightingShader.attribLoc[SHADER_TANGENT]);
}*/

void gyVidTexBind(const GYTexture *const tex) {
	static int wasEnabled = 0;
	int enabled = 1;

	if(tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {
		enabled = 0;
		glDisable(GL_TEXTURE_2D);
	}
	curTex = tex;
	if(enabled != wasEnabled) glUniform1i(curLightShader->uniformLoc[SHADER_TEXTURE_ENABLED], enabled);
	wasEnabled = enabled;
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
	_curQuadListColor[0] = *color;
	_curQuadListColor[1] = *color;
	_curQuadListColor[2] = *color;
	_curQuadListColor[3] = *color;
}

//Don't do shit. Lolz.
void gyVidQuadListEnd(void) {

}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)

void gyVidRectNormalType(const GY_RECT_NORMAL_TYPE type) {
	if(type != _normalType) {	//prevent unecessary updates
		_normalType = type;
		normalVector = &_rectNormals[type];
		glUniform3fv(curLightShader->uniformLoc[SHADER_NORMAL], 1, (float*)normalVector);
	}
}

void gyVidQuad(const GYVector4 *const verts, const unsigned int frame) {
	const GYTexCoord2* coord;
	const GYVector3* tangent;

	if(curTex) {
		coord = gyVidTexCoordArrayFromFrame(&curTex->_coord->texCoord[frame], _rectTexOrient);
		gyVidTexCoordArray(coord);
		_rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL; //reset after each rect
	}

	gyVidColorArray(_curQuadListColor);

	tangent = &_rectTangents[_rectTexOrient];
	if(tangent != tangentVector) {
		tangentVector = tangent;
		glUniform3fv(curLightShader->uniformLoc[SHADER_TANGENT], 1, (float*)tangent);
	}

	gyVidTriangleStrip(verts, 0, 4);
}

void gyVidRect(const unsigned int frame) {
	gyVidQuad(_rectVerts[_normalType], frame);
}

void gyVidProjectionDefaultMatrix(void) {
	GYMatrix4 mat;
	const float pfar = 1000.0f;
	const float pnear = -999.0f;
	const float pleft = 0.0f;
	float pright;
	const float ptop = 0.0f;
	float pbottom;

	unsigned width, height;

	gyVidResolution(&width, &height);

	pright = width; pbottom = height;

	gyVidOrthoProjMat(mat, pleft, pright, pbottom, ptop, pnear, pfar);
	gyVidProjectionCustomMatrix(mat);
}

void gyVidViewFrustrumMat(GYMatrix4 mat, const float fov, const float aspect, const float znear, const float zfar) {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
	gluPerspective(fov, aspect, znear, zfar);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)mat);
	glalTranspose(mat);
	glLoadIdentity();
}

void gyVidLookAtMat(GYMatrix4 mat, const float ex, const float ey, const float ez,
									const float dx, const float dy, const float dz,
									const float ux, const float uy, const float uz) {
	glMatrixMode(GL_MODELVIEW);

	//Cache cube shadow map lookat matrices
	glLoadIdentity();
	gluLookAt(ex, ey, ez, dx, dy, dz, ux, uy, uz);
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)mat);
	glalTranspose(mat);
	glLoadIdentity();
}


/*
 *Every possible way to orient a texture's UV coords (3 bits)
typedef enum _GY_RECT_TEX_ORIENT { GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT,
	GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_ROT_270,
	GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270,
	GY_RECT_TEX_ORIENT_MAX //not representable with 3 bits
} GY_RECT_TEX_ORIENT;

*/




/*
//Every possible way to orient a texture's UV coords (3 bits)
typedef enum _GY_RECT_TEX_ORIENT { GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT,
	GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_ROT_270,
	GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270,
	GY_RECT_TEX_ORIENT_MAX //not representable with 3 bits
} GY_RECT_TEX_ORIENT;



const GYVector4 _rectTangents[1][GY_RECT_TEX_ORIENT_MAX] = {


} */



