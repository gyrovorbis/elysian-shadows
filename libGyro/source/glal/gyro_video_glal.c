#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_shader_api.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>
#include <spine/spine.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQRT_OF_2 1.414213562
#define SQRT_OF_3 1.73205080757

//OPENGL/OPENGL_ES COMMONS, BITCHES!

const GYVector3 glalPlaneNormalMapNormals[GY_RECT_PLANE_MAX][GY_RECT_TEX_ORIENT_MAX] = {
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

const GYVector3 glalRectTangents[GY_RECT_TEX_ORIENT_MAX] = {
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

const GYVector3 glalPlaneNormals[GY_RECT_PLANE_MAX] = {
	{ 0.0f, 0.0f, 1.0f },
    { 0.0f, SQRT_OF_2, SQRT_OF_2},
    { -1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }
};


const GYVector4 glalRectVerts[GY_RECT_PLANE_MAX][4] = {
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


static const GYCamera       _defaultCamera;
static GYVertex             _vertexQueue[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE]        __attribute__((aligned(32)));


//Shared extern video state variables
const GYVector3*			glalTangentVector			= &glalRectTangents[0];
const GYVector3*			glalNormalVector			= &glalPlaneNormalMapNormals[0][0];
//GYVector4					viewPosition			= { 0.0f, 0.0f, 0.0f, 0.0f };
float						glalViewZOrient				= 0.0f;
float						glalViewScale				= 0.0f;
const GYTexture*			glalCurTex					= 0;
const GYCamera*				glalCurCamera				= &_defaultCamera;
float                       glalNormalMapZFactor        = 1.0f;
GY_RECT_TEX_ORIENT          glalRectTexOrient			= GY_RECT_TEX_ORIENT_NORMAL;
GY_RECT_PLANE_TYPE          glalPlaneType				= GY_RECT_PLANE_BOTTOM;
unsigned                    glalCurStripSize            = 0;
unsigned    				glalVertexRenderCount		= 0;
unsigned                    glalUniformChangeCount      = 0;
unsigned                    glalPrevVertexRenderCount   = 0;
unsigned                    glalPrevUniformChangeCount  = 0;
unsigned                    glalTextureChangeCount      = 0;
unsigned                    glalPrevTextureChangeCount  = 0;
GY_ALPHA_LEVEL              glalAlphaLevel              = GY_ALPHA_NONE;
GY_BLEND_MODE               glalBlendMode               = GY_BLEND_ONE_MINUS_SRC_ALPHA;
int                         glalDepthTest               = 0;
void                        (*glalFlushCallback)(void*) = NULL;
void*                       glalFlushObject             = NULL;

//Private video state variables
static unsigned int			_projWidth                  = 0;
static unsigned int			_projHeight                 = 0;
static GYVector4			_clearColor                 = { 0.0f, 0.0f, 0.0f, 1.0f };
static const GYTexCoord2*   _curTexCoordPtr             = NULL;
static const GYColor4*      _curColorArrayPtr           = NULL;
GYColor4                    _curQuadListColor[12]       = { {0, 0, 0, 0}, { 0, 0, 0, 0}, {0,0,0,0}, {0, 0,0,0} };
//static unsigned int         _vertexQueueSize          = 0;
GLuint                      _streamVao[2]               = { 0 };
GLuint                      _streamVertexVbo[2]         = { 0 };

void gyVidClearColor(const GYColor4 color) {
    _clearColor.x = (float)color.r*255.0f;
    _clearColor.y = (float)color.g*255.0f;
    _clearColor.z = (float)color.b*255.0f;
    _clearColor.w = (float)color.a*255.0f;
    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
}

void gyVidBindDefaultVertexArray(void) {
    //glBindVertexArray(_defaultVao[0]);
}

void gyVidVertexBatchBegin(void (*flushCallback)(void*), void* obj) {
    if(glalFlushCallback && glalFlushCallback != flushCallback)
        gyVidVertexBatchFlush();
    glalFlushCallback = flushCallback;
    glalFlushObject = obj;
}

void gyVidVertexBatchEnd(void) {
    glalFlushCallback = NULL;
    glalFlushObject = NULL;
}

void gyVidVertexBatchFlush(void) {
    if(glalFlushCallback) glalFlushCallback(glalFlushObject);
}

unsigned gyVidDbgVertexRenderCount(void) { return glalPrevVertexRenderCount; }
unsigned gyVidDbgShaderUniformChangeCount(void) { return glalPrevUniformChangeCount; }
unsigned gyVidDbgTextureChangeCount(void) { return glalPrevTextureChangeCount; }

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
    *projWidth = _projWidth; *projHeight = _projHeight;
}

int glalVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {
    _projWidth = projWidth;
    _projHeight = projHeight;

	glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
	glViewport(0, 0, viewWidth, viewHeight);

    static int once = 0;
    if(!once) {

        //God fucking damnit, Windows....
        #ifdef _WIN32
            _gyLog(GY_DEBUG_VERBOSE, "Initializing GLEW...");
            GLenum err = glewInit();
            if (err != GLEW_OK) {
              /* Problem: glewInit failed, something is seriously wrong. */
                _gyLog(GY_DEBUG_CRITICAL, "\tError: %s", glewGetErrorString(err));
            }

            if(GLEW_VERSION_3_3) {
                _gyLog(GY_DEBUG_VERBOSE, "\tOpenGL 2.1 Extensions detected");
            } else {
                _gyLog(GY_DEBUG_CRITICAL, "\tOpenGL 2.1 Extensions NOT detected! Prepare for crash!");
            }

        #endif

        glalLoadDefaultShaders();
        gyVidSetDefaultShaderProgram();
        glalDefaultShadersLoadUniforms();
        glDisable(GL_CULL_FACE);
        //gyVidDepthTest(0);
        //gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
        //gyVidAlphaLevel(GY_ALPHA_NONE);
    //    glEnable(GL_DEPTH_TEST);
    //    glDisable(GL_BLEND);
     //   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //  glDepthFunc(GL_LESS);
     //   glClearDepth(1.0f);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLuint fbo;
       // glGenFramebuffers(1, &fbo);
      //  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        _LightingShaderProgram* shaders[2] = {
            &glalLightingShader,
            &glalShadowMapShader
        };

        for(unsigned i = 0; i < 2; ++i) {
            glGenVertexArrays(1, &_streamVao[i]);
            glBindVertexArray(_streamVao[i]);

            glGenBuffers(1, &_streamVertexVbo[i]);
            glBindBuffer(GL_ARRAY_BUFFER, _streamVertexVbo[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE, _vertexQueue, GL_STREAM_DRAW);

            glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_POS]);
            glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_TEX_COORD]);

            glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), 0);
            glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));


            if(i == 0) { //light VAO
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_COLOR]);
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_NORMAL]);
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_TANGENT]);

                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)(6*sizeof(float)));
                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(7*sizeof(float)));
                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(10*sizeof(float)));

            }
        }
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
    ++glalUniformChangeCount;
}

static inline void _flushStreamVertices(void* data) {

    glBindVertexArray(_streamVao[gyVidShadowPass()]);

}

static inline void _gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    //glBindVertexArray(_defaultVao[gyVidShadowPass()]);
  //  glBindBuffer(GL_ARRAY_BUFFER, 0);
   // glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
    glalVertexRenderCount += amount;
   // glBindVertexArray(0);
    _curTexCoordPtr = NULL;
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    //gyMatMultVec(vertexArray, _modelViewVertices, amount);
   // _gyVidTriangleStrip(_modelViewVertices, startIndex, amount);
}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
    if(gyVidShadowPass()) return;
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR]);
    _curColorArrayPtr = ptr;
}

void gyVidTexCoordArray(const struct _GYTexCoord2 *const ptr) {
    //assert(curTex); //You should not be submitting this to the untextured shabut in der, dumbass!
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD]);
    _curTexCoordPtr = ptr;
}

void gyVidTangentArray(const struct _GYVector3 *const ptr) {
    if(gyVidShadowPass()) return;
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT]);
    glalTangentVector = ptr;
}

void gyVidNormalArray(const struct _GYVector3 *const ptr) {
    if(gyVidShadowPass()) return;
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL]);
    glalNormalVector = ptr;
}

void gyVidTexBind(const GYTexture *const tex) {
    static int wasActive = 0;
    static int wasShadow = 0;
    int active = 1;
    int passChange = wasShadow != gyVidShadowPass();

    if(glalTextureMapUniforms[GLAL_TEXTURE_MAP] != 0) return; //mapped to another unit

    if(tex != glalCurTex || passChange) {
        gyVidVertexBatchFlush();
        glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
        if(tex && tex->_data) {
            if(!wasActive || passChange) {
               // glEnable(GL_TEXTURE_2D);
            }
            glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
        } else {
            active = 0;
            if(wasActive || passChange) {
               // glDisable(GL_TEXTURE_2D);
            }
        }
        glalCurTex = tex;
        glalTextureMapActive = active && glalTextureMappingEnabled;
        if(glalTextureMapActive != wasActive || passChange) {
            glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], glalTextureMapActive);
            ++glalUniformChangeCount;
        }

        ++glalTextureChangeCount;
        wasActive = glalTextureMapActive;
    }
    wasShadow = gyVidShadowPass();
}

void gyVidBlendMode(const GY_BLEND_MODE mode) {
    if(gyVidShadowPass()) return;
    if(mode != glalBlendMode) {
        gyVidVertexBatchFlush();
        switch(mode) {
        case GY_BLEND_ONE_MINUS_SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case GY_BLEND_ONE:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        }
        glalBlendMode = mode;
    }
}

void gyVidDepthTest(const int enable) {
    if(gyVidShadowPass()) return;
    static int wasShadow = 0;
    int passChange = wasShadow != gyVidShadowPass();
    if(enable != glalDepthTest || passChange) {
        gyVidVertexBatchFlush();
        if(!enable) glDisable(GL_DEPTH_TEST);
        else glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        glalDepthTest = enable;
        wasShadow = gyVidShadowPass();
    }
}

void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev) {
    if(gyVidShadowPass()) return;

    //Does this even do anything now that we're shader based?
    if(lev != glalAlphaLevel) {
        gyVidVertexBatchFlush();
        switch(lev) {
        case GY_ALPHA_NONE:
        case GY_ALPHA_PUNCHTHRU:
            glDisable(GL_BLEND);
            break;
        default:
            glEnable(GL_BLEND);
            break;
        }
        glalAlphaLevel = lev;
    }
}


void gyVidQuadListBegin(const GYColor4 *const color) {
    _curQuadListColor[0] = *color;
    _curQuadListColor[1] = *color;
    _curQuadListColor[2] = *color;
    _curQuadListColor[3] = *color;
    _curColorArrayPtr = _curQuadListColor;
}

void gyVidQuadListEnd(void) {
    //gyVidTexCoordArray(_stripTexCoords);
    //gyVidNormalArray(_stripNormals);
    //gyVidTangentArray(_stripTangents);
   // gyVidColorArray(_stripColors);
  //  _gyVidTriangleStrip(_modelViewVertices, 0, glalCurStripSize);
    glalCurStripSize = 0;
    _curTexCoordPtr = NULL;
    _curColorArrayPtr = NULL;
}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)


/* IT'S YOUR GODDAMN JOB TO MAKE SURE THESE VECTORS ACTUALLY CHANGED BEFORE CALLING UPDATE ON THEM!
 * DON'T SUBMIT REDUNDANT DATA TO SHADER UNIFORMS, DIPSHIT! */
/*
void glalVidNormalUpdateUniform(const GYVector3* const normal) {
    if(gyVidShadowPass()) return;
    glalNormalVector = normal;
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL], 1, (float*)glalNormalVector);
    //++glalUniformChangeCount;
}

void glalVidTangentUpdateUniform(const GYVector3* const tangent) {
    if(gyVidShadowPass()) return;
    glalTangentVector = tangent;
    glUniform3fv(glalCurLightShader->uniformLoc[GLAL_SHADER_TANGENT], 1, (float*)glalTangentVector);
    //++glalUniformChangeCount;
}*/

void glalVidNormalMapZFactorUpdateUniform(const float factor) {
    if(gyVidShadowPass()) return;
    if(factor != glalNormalMapZFactor) {
        gyVidVertexBatchFlush();
        glalNormalMapZFactor = factor;
        glUniform1f(glalCurLightShader->uniformLoc[GLAL_SHADER_NORMAL_MAP_ZFACTOR], glalNormalMapZFactor);
        ++glalUniformChangeCount;
    }
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
    float               zFactor;

    if(!gyVidShadowPass()) {
        tangent = &glalRectTangents[glalRectTexOrient];

        normal = glalPlaneNormalVector(glalNormalMapActive, glalPlaneType, glalRectTexOrient);

        //update flip flag for normal map texture flipping to correct normals in fragment shader
        zFactor = glalOrientNormalMapZFactor(glalRectTexOrient);
        if(zFactor != glalNormalMapZFactor) {
            glalStripComboBreaker();
            glalVidNormalMapZFactorUpdateUniform(zFactor);
        }

        //Connect current and previous polys with degenerate triangles to continue batching operation
        if(glalCurStripSize) {
        //    gyMatMultVec(verts, &_modelViewVertices[glalCurStripSize+2], 4);
       //     _modelViewVertices[glalCurStripSize] = _modelViewVertices[glalCurStripSize-1];

        //    _modelViewVertices[glalCurStripSize+1] = _modelViewVertices[glalCurStripSize+2];
            glalCurStripSize += 2;
        } else {
      //      gyMatMultVec(verts, &_modelViewVertices[glalCurStripSize], 4);
        }

      //  _stripColors[glalCurStripSize]      = _curColorArrayPtr[0];
     //   _stripColors[glalCurStripSize+1]    = _curColorArrayPtr[1];
     //   _stripColors[glalCurStripSize+2]    = _curColorArrayPtr[2];
    //    _stripColors[glalCurStripSize+3]    = _curColorArrayPtr[3];

      //  _stripNormals[glalCurStripSize]         = *normal;
     //   _stripNormals[glalCurStripSize+2]      = *normal;
      //  _stripNormals[glalCurStripSize+3]      = *normal;
      //  _stripNormals[glalCurStripSize+4]      = *normal;

       // _stripTangents[glalCurStripSize]         = *tangent;
      //  _stripTangents[glalCurStripSize+2]      = *tangent;
      //  _stripTangents[glalCurStripSize+3]      = *tangent;
     //   _stripTangents[glalCurStripSize+4]      = *tangent;
    }

    if(glalCurTex) {
        coord = (frame != -1)? gyVidTexCoordArrayFromFrame(&glalCurTex->_coord->texCoord[frame], glalRectTexOrient) :
                               _curTexCoordPtr;

     //   memcpy(&_stripTexCoords[glalCurStripSize], coord, sizeof(GYTexCoord2)*4);

	}

    glalCurStripSize += 4;

    assert(glalCurStripSize < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
}

void gyVidRect(const int frame) {
    gyVidQuad(glalRectVerts[glalPlaneType], frame);
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

//FUCKING GAY!!!! TRANSFORM IN PLACE!!!
static float worldVertices[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE];
//Spine interop
void gyVidSkeleton(const spSkeleton*const skeleton) {
    if(gyVidShadowPass()) return;
    spSlot*                     slot;
    spAttachment*               attachment;
    GYTexture*                  texture;
    spRegionAttachment*         regionAttachment;
    spMeshAttachment*           mesh;
    spSkinnedMeshAttachment*    skinnedMesh;
    float                       width, height;
    //GYColor4                    color;
    GYVertex*                   startVertex = &_vertexQueue[0];
    GYVertex*                   vertex = startVertex;
    //int                         i;
    //int                         index;
    const float                 zEpsilon = 0.0f;
    float                       zOffset;

    gyVidQuadListEnd();

    gyVidNormalMapBind(NULL);
    gyVidSpecularMapBind(NULL);
    gyVidMaterialBind(NULL);
    gyVidAlphaLevel(GY_ALPHA_FULL);
     gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
     gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
     gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
     glalVidNormalMapZFactorUpdateUniform(1.0f);


    gyVidDepthTest(1);

    for (int i = 0; i < skeleton->slotsCount; ++i) {
    //for (int i = skeleton->slotsCount-1; i >= 0; --i) {
        startVertex = &_vertexQueue[0];
        vertex = startVertex;

        slot = skeleton->drawOrder[i];
        attachment = slot->attachment;
        if (!attachment) continue;
        texture = NULL;
        zOffset = zEpsilon * (float)i;

        if (attachment->type == SP_ATTACHMENT_REGION) {
            regionAttachment = (spRegionAttachment*)attachment;
            texture = (GYTexture*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);
/*
            GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };*/

                        GYColor4 color = { 0, 0, 255, 255 };

            width = texture->width;
            height = texture->height;

            vertex->pos.x = worldVertices[SP_VERTEX_X1];
            vertex->pos.y = worldVertices[SP_VERTEX_Y1];
            vertex->pos.z = 80.0f + zOffset;
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X1];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y1];// * height;
            vertex->color = *(GYColor4*)&color;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X2];
            vertex->pos.y = worldVertices[SP_VERTEX_Y2];
            vertex->pos.z = 80.0f + zOffset;
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X2];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y2];// * height;
            vertex->color = *(GYColor4*)&color;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X3];
            vertex->pos.y = worldVertices[SP_VERTEX_Y3];
            vertex->pos.z = 80.0f + zOffset;
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X3];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y3];// * height;
            vertex->color = *(GYColor4*)&color;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X4];
            vertex->pos.y = worldVertices[SP_VERTEX_Y4];
            vertex->pos.z = 80.0f + zOffset;
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X4];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y4];// * height;
            vertex->color = *(GYColor4*)&color;
            ++vertex;

        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            mesh = (spMeshAttachment*)attachment;
            //if (mesh->verticesCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
            texture = (GYTexture*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;
            spMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

        /*    GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };*/

            GYColor4 color = { 255, 0, 0, 255 };

            width = texture->width;
            height = texture->height;
            for (int j = 0; j < mesh->trianglesCount; ++j) { //loop unroll?
                int index = mesh->triangles[j] << 1;
                vertex->pos.x = worldVertices[index];
                vertex->pos.y = worldVertices[index + 1];
                vertex->pos.z = 80.0f + zOffset;
                vertex->pos.w = 1.0f;
                vertex->texCoord.u = mesh->uvs[index];// * width;
                vertex->texCoord.v = mesh->uvs[index + 1];// * height;
                vertex->color = *(GYColor4*)&color;
                ++vertex;
            }

        } else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH) {
            skinnedMesh = (spSkinnedMeshAttachment*)attachment;
           // if (mesh->uvsCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
            texture = (GYTexture*)((spAtlasRegion*)skinnedMesh->rendererObject)->page->rendererObject;
            spSkinnedMeshAttachment_computeWorldVertices(skinnedMesh, slot, worldVertices);
/*
            GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };
  */
                        GYColor4 color = { 0, 255, 0, 255 };

            width = texture->width;
            height = texture->height;
            for (int j = 0; j < skinnedMesh->trianglesCount; ++j) { //loop unroll?
                int index = skinnedMesh->triangles[j] << 1;
                vertex->pos.x = worldVertices[index];
                vertex->pos.y = worldVertices[index + 1];
                vertex->pos.z = 80.0f + zOffset;
                vertex->pos.w = 1.0f;
                vertex->texCoord.u = skinnedMesh->uvs[index];// * width;
                vertex->texCoord.v = skinnedMesh->uvs[index + 1];// * height;
                vertex->color = *(GYColor4*)&color;
                ++vertex;
            }
        }


        if(texture) {

            gyVidAlphaLevel(GY_ALPHA_FULL);
           // if(slot->data->additiveBlending != lastAdditive) {
            if(slot->data->additiveBlending) {
                gyVidBlendMode(GY_BLEND_ONE);
            } else {
                gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
            }

            gyVidTexBind(texture);

            float* rawPtr = (float*)startVertex;
            glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_POS]);
            glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR]);
            glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD]);
            glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL]);
            glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT]);
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)(rawPtr));
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(rawPtr+4));
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)(rawPtr+6));
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(rawPtr+7));
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(rawPtr+10));

            int count = (int)vertex-(int)startVertex;
            assert(count < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
            glDrawArrays(attachment->type == SP_ATTACHMENT_REGION? GL_QUADS : GL_TRIANGLES, 0, count);
            glalVertexRenderCount += count;
        }

    }

    gyVidDepthTest(0);

   // target.draw(*vertexArray, states);
}

