#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_shader_api.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQRT_OF_2 1.414213562
#define SQRT_OF_3 1.73205080757

//OPENGL/OPENGL_ES COMMONS, BITCHES!

static GYVector3 _rectNormals[GY_RECT_PLANE_MAX][GY_RECT_TEX_ORIENT_MAX] = {
    {   //GY_RECT_PLANE_BOTTOM
        { 0.0f, 0.0f, 1.0f }, //GY_RECT_TEX_ORIENT_NORMAL
        { 0.0f, 0.0f, -1.0f }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ
        { 0.0f, 0.0f, -1.0f }, //GY_RECT_TEX_ORIENT_FLIP_VERT
        { 0.0f, 0.0f, 1.0f }, //GY_RECT_TEX_ORIENT_ROT_90
        { 0.0f, 0.0f, 1.0f }, //GY_RECT_TEX_ORIENT_ROT_180
        { 0.0f, 0.0f, 1.0f }, //GY_RECT_TEX_ORIENT_ROT_270
        { 0.0f, 0.0f, -1.0f }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90
        { 0.0f, 0.0f, -1.0f }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270
    },
    {   //GY_RECT_PLANE_FRONT
        { 0.0f, SQRT_OF_2, SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_NORMAL
        { 0.0f, -SQRT_OF_2, -SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ
        { 0.0f, -SQRT_OF_2, -SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_FLIP_VERT
        { -SQRT_OF_2, 0.0f, SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_ROT_90
        { 0.0f, SQRT_OF_2, SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_ROT_180
        { SQRT_OF_2, 0.0f, SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_ROT_270
        { -SQRT_OF_2, 0.0f, -SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90
        { SQRT_OF_2, 0.0f, -SQRT_OF_2 }, //GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270
    }
};

static const GYVector3 _rectTangents[GY_RECT_TEX_ORIENT_MAX] = {
        { 1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f }, //FH
        { 1.0, 0.0, 0.0f },  //FV
        /* 90 degrees*/
        { 0.0f, -1.0f, 0.0f }, //R90
        { -1.0f, 0.0f, 0.0f }, //R180
        { 0.0f, 1.0f, 0.0f }, //R270
        /* flip horiz, rot 90 */
        { 0.0f, -1.0f, 0.0f }, //FHR90
        { 0.0f, 1.0f, 0.0f }   //FHR270
};

static GYVector3 _planeNormals[GY_RECT_PLANE_MAX] = {
	{ 0.0f, 0.0f, 1.0f },
    { 0.0f, SQRT_OF_2, SQRT_OF_2},
    { -1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }
};


/*
typedef enum _GY_RECT_TEX_ORIENT { GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT,
    GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_ROT_270,
    GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270,
    GY_RECT_TEX_ORIENT_MAX //not representable with 3 bits
} GY_RECT_TEX_ORIENT;
*/

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
static GYTexCoord2		_stripTexCoords[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]     __attribute__((aligned(32)));
static GYVector4		_modelViewVertices[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]  __attribute__((aligned(32)));
GYColor4                _stripColors[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]           __attribute__((aligned(32)));


//Shared extern video state variables
const GYVector3*			glalTangentVector			= &_rectTangents[0];
const GYVector3*			glalNormalVector			= &_rectNormals[0][0];
//GYVector4					viewPosition			= { 0.0f, 0.0f, 0.0f, 0.0f };
float						glalViewZOrient				= 0.0f;
float						glalViewScale				= 0.0f;
const GYTexture*			glalCurTex					= 0;
const GYCamera*				glalCurCamera				= &_defaultCamera;
GLAL_TEXTURE_FLIP           glalTextureFlip             = GLAL_TEXTURE_FLIP_NONE;
GY_RECT_TEX_ORIENT          glalRectTexOrient			= GY_RECT_TEX_ORIENT_NORMAL;
GY_RECT_PLANE_TYPE          glalPlaneType				= GY_RECT_PLANE_BOTTOM;
unsigned                    glalCurStripSize            = 0;
unsigned    				glalVertexRenderCount		= 0;

//Private video state variables
static unsigned int			_projWidth				= 0;
static unsigned int			_projHeight				= 0;
static GYVector4			_clearColor				= { 0.0f, 0.0f, 0.0f, 0.0f };
static const GYTexCoord2*   _curTexCoordPtr          = NULL;
GYColor4				_curQuadListColor[12]	= { {0, 0, 0, 0}, { 0, 0, 0, 0}, {0,0,0,0}, {0, 0,0,0} };

void gyVidClearColor(const GYColor4 color) {
	_clearColor.x = (float)color.r*255.0f;
	_clearColor.y = (float)color.g*255.0f;
	_clearColor.z = (float)color.b*255.0f;
	_clearColor.w = (float)color.a*255.0f;
	glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
}

unsigned gyVidVertexRenderCount(void) { return glalVertexRenderCount; }

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
	*projWidth = _projWidth; *projHeight = _projHeight;
}

int glalVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {

    //gyVec3Normalize(&_planeNormals[GY_RECT_PLANE_FRONT]);

	//gyVidOrthoProjMat(projectionMatrix, pleft, pright, pbottom, ptop, pnear, pfar);
	//gyVidCamOrthoProj(&_defaultCamera, pleft, pright, pbottom, ptop, pnear, pfar);

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
        glalCurCamera = cam;
	} else {
        glalCurCamera = &_defaultCamera;
	}
    glUniformMatrix4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)glalCurCamera->projViewMat);
}

static inline void _gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, 0, vertexArray);
    //GLenum glErr = glGetError();
   /* if (glErr != GL_NO_ERROR)
    {
        gyPrintf("glError in file - vertAttrib");
    }*/
    glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_POS]);
  /*  glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        gyPrintf("glError in file enableVertAttribArray");
    }*/
	glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
    /*glErr = glGetError();
       if (glErr != GL_NO_ERROR)
       {
           gyPrintf("glError in file gldrawArrays");
       }*/
    glalVertexRenderCount += amount;

    _curTexCoordPtr = NULL;
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
	gyMatMultVec(vertexArray, _modelViewVertices, amount);
	_gyVidTriangleStrip(_modelViewVertices, startIndex, amount);
}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
    glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
    glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR]);
}

void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr) {
	//assert(curTex); //You should not be submitting this to the untextured shabut in der, dumbass!
    glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
    glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD]);
    _curTexCoordPtr = ptr;
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
    static int wasActive = 0;
    int active = 1;

    if(glalTextureMapUniforms[GLAL_TEXTURE_MAP] != 0) return; //mapped to another unit

    glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
    if(tex && tex->_data) {
        glEnable(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
	} else {
        active = 0;
        glDisable(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
	}
    glalCurTex = tex;
    glalTextureMapActive = active && glalTextureMappingEnabled;
    /*if(enabled != wasEnabled)*/ glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], glalTextureMapActive);
    wasActive = active;
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
    gyVidTexCoordArray(_stripTexCoords);
	gyVidColorArray(_stripColors);
    _gyVidTriangleStrip(_modelViewVertices, 0, glalCurStripSize);
    glalCurStripSize = 0;
    _curTexCoordPtr = NULL;
}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)

void glalVidNormalUpdateUniform(void) {
    glalNormalVector = !glalNormalMapActive? &_planeNormals[glalPlaneType] : &_rectNormals[glalPlaneType][glalRectTexOrient];
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL], 1, (float*)glalNormalVector);
}

void glalVidTangentUpdateUniform(void) {
    glalTangentVector = &_rectTangents[glalRectTexOrient];
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_TANGENT], 1, (float*)glalTangentVector);
}

void glalVidTextureFlipUpdateUniform(void) {
    glalTextureFlip = glalTextureFlipFromOrientAndPlane(glalRectTexOrient);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_FLIP], glalTextureFlip);
}

void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type) {
    glalPlaneType = type;
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
    glalRectTexOrient = orient;
}


void gyVidQuad(const GYVector4 *const verts, const int frame) {
    const GYTexCoord2*  coord;
    const GYVector3*    tangent;
    const GYVector3*    normal;
    unsigned i;

    int updateFlip = 0;

    tangent = &_rectTangents[glalRectTexOrient];
    if(tangent != glalTangentVector) { //FUCKING COMBO BREAKER, SUBMIT THE BITCH!!!!
        glalStripComboBreaker();
        glalVidTangentUpdateUniform();
        updateFlip = 1;
    }

    normal = !glalNormalMapActive? &_planeNormals[glalPlaneType] : &_rectNormals[glalPlaneType][glalRectTexOrient];
    if(normal != glalNormalVector) {
        glalStripComboBreaker();
        glalVidNormalUpdateUniform();
        //updateFlip = 1;
    }

    if(updateFlip && glalTextureFlipFromOrientAndPlane(glalRectTexOrient) != glalTextureFlip) {
        glalVidTextureFlipUpdateUniform();
    }

    if(glalCurStripSize) {
        gyMatMultVec(verts, &_modelViewVertices[glalCurStripSize+2], 4);
		//_stripTexCoords[curStripSize+1] = _stripTexCoords[curStripSize];
		//_stripColors[curStripSize+1] = _stripColors[curStripSize];
        _modelViewVertices[glalCurStripSize] = _modelViewVertices[glalCurStripSize-1];

		//_stripTexCoords[curStripSize+2] = coord[1];
		//_stripColors[curStripSize+2] = _curQuadListColor[0];
        _modelViewVertices[glalCurStripSize+1] = _modelViewVertices[glalCurStripSize+2];
        glalCurStripSize += 2;
	} else {
        gyMatMultVec(verts, &_modelViewVertices[glalCurStripSize], 4);
	}

    if(glalCurTex) {
        coord = (frame != -1)? gyVidTexCoordArrayFromFrame(&glalCurTex->_coord->texCoord[frame], glalRectTexOrient) :
                               _curTexCoordPtr;
		//gyVidTexCoordArray(coord);
        memcpy(&_stripTexCoords[glalCurStripSize], coord, sizeof(GYTexCoord2)*4);

	}

	//gyVidColorArray(_curQuadListColor);
	//memcpy(&_stripColors[curStripSize], _curQuadListColor, sizeof(GYColor4)*4);
    //for(i = 0; i < 4; ++i) {
   // memcpy(&_stripColors[glalCurStripSize], _curQuadListColor, sizeof(GYColor4)*4);
        _stripColors[glalCurStripSize] = _curQuadListColor[0];
       _stripColors[glalCurStripSize+1] = _curQuadListColor[1];
       _stripColors[glalCurStripSize+2] = _curQuadListColor[2];
       _stripColors[glalCurStripSize+3] = _curQuadListColor[3];
        //_gyLog(GY_DEBUG_VERBOSE, "ACCESS - %d", i);
        //_stripColors[glalCurStripSize+i] = temp;
        //_gyLog(GY_DEBUG_VERBOSE, "DONE - %d", glalCurStripSize+i);
//}

	//memcpy(&_stripVerts[curStripSize], verts, sizeof(GYVector4)*4);
	//GYVector4 (*stripVerts)[4] = &_stripVerts[curStripSize];
	//gyVidTriangleStrip(verts, 0, 4);
	//gyMatMultVec(verts, &_modelViewVertices[curStripSize], 4);
    glalCurStripSize += 4;

    assert(glalCurStripSize < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
}

void gyVidRect(const int frame) {
    gyVidQuad(_rectVerts[glalPlaneType], frame);
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



