#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_shader_api.h>
#include "gyro_video_glal.h"
#include "gyro_shaders_glal.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//OPENGL/OPENGL_ES COMMONS, BITCHES!
static const GYVector3 _planeNormals[GY_RECT_PLANE_MAX] = {
	{ 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 1.0f },
    { -1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }
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
static const GYVector4 _rectVerts[GY_RECT_PLANE_MAX][4] = {
	//FLAT
	{
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
	},
 #if 0
	//FRONT
	{
		{-0.5f, -0.5f, 1.0f, 1.0f},
		{0.5f, -0.5f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
	}
#else
	//FRONT
	{
		{-0.5f, 0.5f, 1.0f, 1.0f},
		{0.5f, 0.5f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.0f, 1.0f}
    },
   #endif
    //LEFT
    {
        {-0.5f, -0.5f, 1.0f, 1.0f},
        {-0.5f, 0.5f, 1.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f}
    },
    //RIGHT
    {
        {0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
    //FRONT_SLOPE_45
    {
        {-0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 1.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
    //LEFT_SLOPE_45
    {
        {0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
    //RIGHT_SLOPE_45
    {
        {0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
};

/*
 * typedef struct _GYViewVolume {
	//static enum {OUTSIDE, INTERSECT, INSIDE};
	GYPlane pl[6];
	GYVector3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;
} GYViewVolume;

typedef struct _GYCamera {
	GYViewVolume		viewVolume;
	GYMatrix4			projMat;
	GYMatrix4			viewMat;
	GYMatrix4			projViewMat;
	GY_PROJECTION_TYPE	projType;
} GYCamera;
 *
 */

static const GYCamera	_defaultCamera;
//static GYVector4		_stripVerts[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]         __attribute__((aligned(32)));
static GYColor4			_stripColors[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]        __attribute__((aligned(32)));
static GYTexCoord2		_stripTexCoords[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]     __attribute__((aligned(32)));
static GYVector4		_modelViewVertices[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]  __attribute__((aligned(32)));
unsigned				curStripSize = 0;

//Shared video state variables
const GYVector3*			tangentVector			= &_rectTangents[0];
const GYVector3*			normalVector			= &_planeNormals[0];
//GYVector4					viewPosition			= { 0.0f, 0.0f, 0.0f, 0.0f };
float						viewZOrient				= 0.0f;
float						viewScale				= 0.0f;
const GYTexture*			curTex					= 0;
const GYCamera*				curCamera				= &_defaultCamera;
const GYTexCoord2*          curTexCoordPtr          = NULL;

//Private video state variables
static unsigned int			_projWidth				= 0;
static unsigned int			_projHeight				= 0;
static GY_RECT_TEX_ORIENT	_rectTexOrient			= GY_RECT_TEX_ORIENT_NORMAL;
static GY_RECT_PLANE_TYPE	_planeType				= GY_RECT_PLANE_BOTTOM;
static GYColor4				_curQuadListColor[4]	= { {0, 0, 0, 0} };
unsigned					vertexRenderCount		= 0;
GYVector4					_clearColor				= { 0.0f, 0.0f, 0.0f, 0.0f };

void gyVidClearColor(const GYColor4 color) {
	_clearColor.x = (float)color.r*255.0f;
	_clearColor.y = (float)color.g*255.0f;
	_clearColor.z = (float)color.b*255.0f;
	_clearColor.w = (float)color.a*255.0f;
	glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
}

unsigned gyVidVertexRenderCount(void) { return vertexRenderCount; }

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
	*projWidth = _projWidth; *projHeight = _projHeight;
}

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {

	//gyVidOrthoProjMat(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);
	//gyVidCamOrthoProj(&_defaultCamera, pleft, pright, pbottom, ptop, pnear, pfar);

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

	glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
   // glClearDepthf(1.0f);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
    // Enable depth buffer
//    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);
	glViewport(0, 0, viewWidth, viewHeight);

    static int once = 0;
    if(!once) {
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glalLoadDefaultShaders();
        gyVidSetDefaultShaderProgram();
        glalDefaultShadersLoadUniforms();
        once = 1;
    }

    return 1;
}

// ==== LOW-LEVEL API =====

void gyVidCamUpdate(const GYCamera *const cam) {
	if(cam) {
		curCamera = cam;
	} else {
		curCamera = &_defaultCamera;
	}
	glUniformMatrix4fv(curLightShader->uniformLoc[SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)curCamera->projViewMat);
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
	_rectTexOrient = orient;
	//const GYVector3* tangent;
}

static inline void _gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
	glVertexAttribPointer(curLightShader->attribLoc[SHADER_POS], 4, GL_FLOAT, GL_FALSE, 0, vertexArray);
    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        gyPrintf("glError in file - vertAttrib");
    }
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_POS]);
    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        gyPrintf("glError in file enableVertAttribArray");
    }
	glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
    glErr = glGetError();
       if (glErr != GL_NO_ERROR)
       {
           gyPrintf("glError in file gldrawArrays");
       }
	vertexRenderCount += amount;

    curTexCoordPtr = NULL;
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
	gyMatMultVec(vertexArray, _modelViewVertices, amount);
	_gyVidTriangleStrip(_modelViewVertices, startIndex, amount);
}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
	glVertexAttribPointer(curLightShader->attribLoc[SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_COLOR]);
}

void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr) {
	//assert(curTex); //You should not be submitting this to the untextured shabut in der, dumbass!
	glVertexAttribPointer(curLightShader->attribLoc[SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
	glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_TEX_COORD]);
    curTexCoordPtr = ptr;
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

	glActiveTexture(GL_TEXTURE0);
    if(tex && tex->_data) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {
		enabled = 0;
		glDisable(GL_TEXTURE_2D);
	}
	curTex = tex;
	/*if(enabled != wasEnabled)*/ glUniform1i(curLightShader->uniformLoc[SHADER_TEXTURE_ENABLED], enabled);
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

void gyVidQuadListEnd(void) {
    if(curTex) gyVidTexCoordArray(_stripTexCoords);
	gyVidColorArray(_stripColors);
	_gyVidTriangleStrip(_modelViewVertices, 0, curStripSize);
	curStripSize = 0;
    curTexCoordPtr = NULL;
}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)

void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type) {
    //if(type != _planeType) {	//prevent unecessary updates
		glalStripComboBreaker();
        _planeType = type;
        normalVector = &_planeNormals[type];
		glUniform3fv(curLightShader->uniformLoc[SHADER_NORMAL], 1, (float*)normalVector);
    //}
}

void gyVidQuad(const GYVector4 *const verts, const int frame) {
	const GYTexCoord2* coord;

    const GYVector3* tangent;

	tangent = &_rectTangents[_rectTexOrient];
    if(tangent != tangentVector) { //FUCKING COMBO BREAKER, SUBMIT THE BITCH!!!!
		glalStripComboBreaker();
		tangentVector = tangent;
		glUniform3fv(curLightShader->uniformLoc[SHADER_TANGENT], 1, (float*)tangent);
    }


	if(curStripSize) {
		gyMatMultVec(verts, &_modelViewVertices[curStripSize+2], 4);
		//_stripTexCoords[curStripSize+1] = _stripTexCoords[curStripSize];
		//_stripColors[curStripSize+1] = _stripColors[curStripSize];
		_modelViewVertices[curStripSize] = _modelViewVertices[curStripSize-1];

		//_stripTexCoords[curStripSize+2] = coord[1];
		//_stripColors[curStripSize+2] = _curQuadListColor[0];
		_modelViewVertices[curStripSize+1] = _modelViewVertices[curStripSize+2];
		curStripSize += 2;
	} else {
		gyMatMultVec(verts, &_modelViewVertices[curStripSize], 4);
	}

    if(curTex) {
        coord = (frame != -1)? gyVidTexCoordArrayFromFrame(&curTex->_coord->texCoord[frame], _rectTexOrient) :
                               curTexCoordPtr;
		//gyVidTexCoordArray(coord);
		memcpy(&_stripTexCoords[curStripSize], coord, sizeof(GYTexCoord2)*4);
		_rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL; //reset after each rect
        tangent = &_rectTangents[_rectTexOrient];
        if(tangent != tangentVector) { //FUCKING COMBO BREAKER, SUBMIT THE BITCH!!!!
            glalStripComboBreaker();
            tangentVector = tangent;
            glUniform3fv(curLightShader->uniformLoc[SHADER_TANGENT], 1, (float*)tangent);
        }


	}

	//gyVidColorArray(_curQuadListColor);
	//memcpy(&_stripColors[curStripSize], _curQuadListColor, sizeof(GYColor4)*4);
	for(unsigned i = 0; i < 4; ++i) _stripColors[curStripSize+i] = _curQuadListColor[i];


	//memcpy(&_stripVerts[curStripSize], verts, sizeof(GYVector4)*4);
	//GYVector4 (*stripVerts)[4] = &_stripVerts[curStripSize];
	//gyVidTriangleStrip(verts, 0, 4);
	//gyMatMultVec(verts, &_modelViewVertices[curStripSize], 4);
	curStripSize += 4;

	assert(curStripSize < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
}

void gyVidRect(const int frame) {
    gyVidQuad(_rectVerts[_planeType], frame);
}


void gyVidCamFrustum(GYCamera* const cam, const float fov, const float aspect, const float znear, const float zfar) {
	GYViewVolume* volume = &cam->viewVolume;

	gyVidPerspectiveMat(cam->projMat, fov, aspect, znear, zfar);
	cam->projType = GY_PROJECTION_PERSPECTIVE;

	gyMatMultMat(cam->projViewMat, cam->projMat, cam->viewMat);

	volume->ratio = aspect;
	volume->nearD = znear;
	volume->farD = zfar;
	volume->angle = fov;

	volume->tang = (float)tan(GY_ANG2RAD * volume->angle * 0.5);
	volume->nh = volume->nearD * volume->tang;
	volume->nw = volume->nh * volume->ratio;
	volume->fh = volume->farD * volume->tang;
	volume->fw = volume->fh * volume->ratio;

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



