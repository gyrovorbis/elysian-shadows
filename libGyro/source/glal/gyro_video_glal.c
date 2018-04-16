#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <gyro_shader_api.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>
#include <glal/gyro_bloom_glal.h>
#include <glal/gyro_gl_utils.h>
#include <common/gyro_spine_common.h>
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
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
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
    #if 0
    {
        {-0.5f, -0.5f, SQRT_OF_HALF, 1.0f},
        {0.5f, -0.5f, SQRT_OF_HALF, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
    #else
    {
        {-0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 1.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    },
    #endif

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
GY_RECT_TEX_ORIENT          glalRectTexOrient			= GY_RECT_TEX_ORIENT_NORMAL;
GY_RECT_PLANE_TYPE          glalPlaneType				= GY_RECT_PLANE_BOTTOM;
//unsigned                    glalCurStripSize            = 0;
unsigned    				glalVertexRenderCount		= 0;
unsigned                    glalUniformChangeCount      = 0;
unsigned                    glalPrevVertexRenderCount   = 0;
unsigned                    glalPrevUniformChangeCount  = 0;
unsigned                    glalTextureChangeCount      = 0;
unsigned                    glalPrevTextureChangeCount  = 0;
GY_ALPHA_LEVEL              glalAlphaLevel              = GY_ALPHA_NONE;
GY_BLEND_MODE               glalBlendMode               = GY_BLEND_ONE_MINUS_SRC_ALPHA;
int                         glalDepthTest               = 0;
int                         glalVertexArray             = 0;
void                        (*glalFlushCallback)(void*) = NULL;
void*                       glalFlushObject             = NULL;
int                         glalVersionMajor            = 0;
int                         glalVersionMinor            = 0;

//Private video state variables
static unsigned int			_projWidth                  = 0;
static unsigned int			_projHeight                 = 0;
static GYVector4			_clearColor                 = { 0.0f, 0.0f, 0.0f, 0.0f };
static const GYTexCoord2*   _curTexCoordPtr             = NULL;
static const GYColor4*      _curColorArrayPtr           = NULL;
GYColor4                    _curQuadListColor[12]       = { {0, 0, 0, 0}, { 0, 0, 0, 0}, {0,0,0,0}, {0, 0,0,0} };
static int                  _vertexQueueSize            = 0;
static int                  _vertexQueueLastSize        = 0;
static GLintptr             _vertexQueueLastOffset      = 0;
GLuint                      _streamVao[2]               = { 0 };
GLuint                      _streamVertexVbo[2]         = { 0 };
GLuint                      _glalMainFbo                = 0;
GLuint                      _glalColorRT                = 0;
GLuint                      _glalGlowRT                 = 0;
GLuint                      _glalColorTex               = 0;
GLuint                      _glalGlowTex                = 0;
GLuint                      _glalDepthBuffer            = 0;


void glalBindMainMrtFbo(void) {


}

void gyVidSceneBegin(void) {
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1

    glBindFramebuffer(GL_FRAMEBUFFER, _glalMainFbo);
    glViewport(0, 0, (float)GLAL_RENDERBUFFER_WIDTH, (float)GLAL_RENDERBUFFER_HEIGHT);
#if 0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _glalColorTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _glalGlowTex, 0);
#endif
#if 1
    //uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //glDrawBuffers(2, buffers);
#endif
 /*
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        _gyLog(GY_DEBUG_ERROR, "FBO Initialization failed: %d", status);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }*/
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gyMatLoadIdentity();

    glalPrevVertexRenderCount   = glalVertexRenderCount;
    glalPrevUniformChangeCount  = glalUniformChangeCount;
    glalPrevTextureChangeCount  = glalTextureChangeCount;
    glalVertexRenderCount       = 0;
    glalUniformChangeCount      = 0;
    glalTextureChangeCount      = 0;
}

void gyVidSceneEnd(void) {
    gyVidVertexBatchFlush();
}




void gyVidClearColor(const GYColor4 color) {
    _clearColor.x = (float)color.r * 255.0f;
    _clearColor.y = (float)color.g * 255.0f;
    _clearColor.z = (float)color.b * 255.0f;
    _clearColor.w = (float)color.a * 255.0f;
    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
}

void gyVidVertexBatchBegin(void (*flushCallback)(void*), void* obj) {
    if(glalFlushCallback && glalFlushCallback != flushCallback)
        gyVidVertexBatchFlush();
    glalFlushCallback = flushCallback;
    glalFlushObject = obj;
}

void gyVidVertexBatchEnd(void) {
    glalFlushCallback   = NULL;
    glalFlushObject     = NULL;
}

void gyVidVertexBatchFlush(void) {
    static int recursing = 0;
    if(recursing) return;
    recursing = 1;
    if(glalFlushCallback) glalFlushCallback(glalFlushObject);
    recursing = 0;
}

void gyVidVertexArrayBind(int id) {
    if(id != glalVertexArray) {
        gyVidVertexBatchFlush();
#ifndef GL_ES_VERSION_2_0
        glBindVertexArray(id);
#endif
        glalVertexArray = id;
    }
}

unsigned gyVidDbgVertexRenderCount(void) { return glalPrevVertexRenderCount; }
unsigned gyVidDbgShaderUniformChangeCount(void) { return glalPrevUniformChangeCount; }
unsigned gyVidDbgTextureChangeCount(void) { return glalPrevTextureChangeCount; }

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
    *projWidth = _projWidth; *projHeight = _projHeight;
}

int glalVidGlewInit(void) {
    static int once = 0;

    if(!once) {
    //God fucking damnit, Windows....
#if GYRO_PLATFORM != GYRO_PLATFORM_ANDROID && GYRO_PLATFORM != GYRO_PLATFORM_RASPI
#   if defined(_WIN32) || defined(__linux__)
        _gyLog(GY_DEBUG_VERBOSE, "Initializing GLEW...");
        _gyPush();
        glewExperimental = GL_TRUE; //FUCKING GLEW CANT HANDLE GL CORE CONTEXTS WITHOUT THIS.
        GLenum err = glewInit();
        if (err != GLEW_OK) {
          /* Problem: glewInit failed, something is seriously wrong. */
            _gyLog(GY_DEBUG_ERROR, "Error: %s", glewGetErrorString(err));
        }

        if(GLEW_VERSION_3_3) {
            _gyLog(GY_DEBUG_VERBOSE, "OpenGL 3.3 Extensions detected");
        } else {
            _gyLog(GY_DEBUG_ERROR, "OpenGL 3.3 Extensions NOT detected! Prepare for crash!");
        }
        _gyPop(1);
        once = 1;

#   endif
#endif
    }
    return 1;
}



int glalVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {
    _projWidth = projWidth;
    _projHeight = projHeight;

    glalVidGlewInit();

    static int once = 0;
    if(!once) {    glFlush();
        GLint glint[2];
        _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Initializing OpenGL.");
        _gyPush();

        glGetIntegerv(GL_MAJOR_VERSION, &glint[0]);
        glGetIntegerv(GL_MINOR_VERSION, &glint[1]);
        if(!GY_GL_CHECK_ERROR()) {
            glalVersionMajor = glint[0];
            glalVersionMinor = glint[1];
        }
        glalGLDriverQuery();

        glalLoadDefaultShaders();
        glalVidInitializeBloom();
        gyVidSetDefaultShaderProgram();

        _gyLog(GY_DEBUG_VERBOSE, "Creating FBO(s)");
        _gyPush();
        GY_GL_CHECK_ERROR();
        glGenFramebuffers(1, &_glalMainFbo);
        glGenRenderbuffers(1, &_glalColorRT);
        glGenRenderbuffers(1, &_glalGlowRT);
        glGenRenderbuffers(1, &_glalDepthBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, _glalMainFbo);

#if 0
        glBindRenderbuffer(GL_RENDERBUFFER, _glalColorRT);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _glalColorRT);


        glBindRenderbuffer(GL_RENDERBUFFER, _glalGlowRT);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, _glalGlowRT);
#endif
        glBindRenderbuffer(GL_RENDERBUFFER, _glalDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _glalDepthBuffer);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE) {
            _gyLog(GY_DEBUG_ERROR, "FBO Initialization failed: %d", status);
        }

        glGenTextures(1, &_glalColorTex);
        glBindTexture(GL_TEXTURE_2D, _glalColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _glalColorTex, 0);
              status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE) {
            _gyLog(GY_DEBUG_ERROR, "FBO Initialization failed: %d", status);
        }

        glGenTextures(1, &_glalGlowTex);
        glBindTexture(GL_TEXTURE_2D, _glalGlowTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _glalGlowTex, 0);

        uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, buffers);

       status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE) {
            _gyLog(GY_DEBUG_ERROR, "FBO Initialization failed: %d", status);
        }
        glalVidBindDefaultFbo();
        _gyPop(1);

        _gyLog(GY_DEBUG_VERBOSE, "Creating UBOs");
        GY_GL_CHECK_ERROR();
        glGenBuffers(1, &glalLightingUBO);
        GY_GL_CHECK_ERROR();
        glBindBuffer(GL_UNIFORM_BUFFER, glalLightingUBO);
        GY_GL_CHECK_ERROR();
        glalLightingUBOData.count = 0;
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingUBOData), &glalLightingUBOData, GL_DYNAMIC_DRAW);
        GY_GL_CHECK_ERROR();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, glalLightingUBO);
        GY_GL_CHECK_ERROR();
        glUniformBlockBinding(glalLightingShader.program->program, glalLightingShader.lightingUBOIndex, 0);
        GY_GL_CHECK_ERROR();
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glalDefaultShadersLoadUniforms();
        //HAVE TO SET DEFAULT GL STATES OR THE STATE CACHES WILL BE DIRTY
        //K the fact that this reests spine needs to be reinvestigated!!!
        //glCullFace(GL_FRONT);
        //glEnable(GL_CULL_FACE);

        //gyVidDepthTest(0);
        //gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
        //gyVidAlphaLevel(GY_ALPHA_NONE);
    //    glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);    glFlush();
      //  glDepthFunc(GL_LESS);
     //   glClearDepth(1.0f);
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);

       // glGenFramebuffers(1, &fbo);
      //  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        _LightingShaderProgram* shaders[2] = {
            &glalLightingShader,
            &glalShadowMapShader
        };



        _gyLog(GY_DEBUG_VERBOSE, "Creating VAOs and VBOs.");

        glGenBuffers(1, &_streamVertexVbo[0]);
        glBindBuffer(GL_ARRAY_BUFFER, _streamVertexVbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE, _vertexQueue, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        for(unsigned i = 0; i < 2; ++i) {
#ifndef GL_ES_VERSION_2_0
            glGenVertexArrays(1, &_streamVao[i]);
#endif
            gyVidVertexArrayBind(_streamVao[i]);

            glBindBuffer(GL_ARRAY_BUFFER, _streamVertexVbo[0]);

            glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_POS]);
            glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_TEX_COORD]);

            glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
            glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));


            if(i == 0) { //light VAO
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_COLOR]);
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_NORMAL]);
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_TANGENT]);
                glEnableVertexAttribArray(shaders[i]->attribLoc[GLAL_SHADER_NORMAL_Z]);

                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)(6*sizeof(float)));
                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(7*sizeof(float)));
                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(10*sizeof(float)));
                glVertexAttribPointer(shaders[i]->attribLoc[GLAL_SHADER_NORMAL_Z], 1, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(13*sizeof(float)));
            }
        }
        once = 1;
        gyVidVertexArrayBind(0);
        _gyPop(1);
    }


    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
#if GYRO_PLATFORM != GYRO_PLATFORM_iOS
    glViewport(0, 0, viewWidth, viewHeight);
#endif

  //  glalVidBindDefaultFbo();
    return 1;
}

// ==== LOW-LEVEL API =====

void gyVidCamUpdate(const GYCamera *const cam) {
    if(cam) {
        glalCurCamera = cam;
    } else {
        glalCurCamera = &_defaultCamera;
    }
    gyVidVertexBatchFlush();
    glUniformMatrix4fv(glalCurLightShader->uniformLoc[GLAL_SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)glalCurCamera->projViewMat);
    ++glalUniformChangeCount;
}

static int _glPrimitiveType = GL_TRIANGLE_STRIP;

static inline void _flushStreamVertices(void* data) {
    (void)data;
#ifndef GL_ES_VERSION_2_0
    if(_vertexQueueSize-_vertexQueueLastSize <= 0) return;
    gyVidVertexArrayBind(_streamVao[gyVidShadowPass()]);

    glBindBuffer(GL_ARRAY_BUFFER, _streamVertexVbo[0]);

     GYVertex* ptr = (GYVertex*)glMapBufferRange(GL_ARRAY_BUFFER, _vertexQueueLastOffset, sizeof(GYVertex)*(_vertexQueueSize-_vertexQueueLastSize), /*GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT |*/ GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

    if(ptr) {
        memcpy(ptr, &_vertexQueue[_vertexQueueLastSize], sizeof(GYVertex)*(_vertexQueueSize-_vertexQueueLastSize));
        //glFlushMappedBufferRange(GL_ARRAY_BUFFER, _vertexQueueLastOffset, sizeof(GYVertex)*(_vertexQueueSize-_vertexQueueLastSize));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glDrawArrays(_glPrimitiveType, _vertexQueueLastSize, (_vertexQueueSize-_vertexQueueLastSize));
    }
    _vertexQueueLastSize    = _vertexQueueSize;
    _vertexQueueLastOffset  = _vertexQueueSize*sizeof(GYVertex);
#endif
}


#if 0
static inline void _gyVidTriangleStrip(const GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    (void)vertexArray;
    (void)startIndex;
    (void)amount;
    //glBindVertexArray(_defaultVao[gyVidShadowPass()]);
  //  glBindBuffer(GL_ARRAY_BUFFER, 0);
   // glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);
    glalVertexRenderCount += amount;
   // glBindVertexArray(0);
    _curTexCoordPtr = NULL;
}
#endif

void gyVidTriangleStrip(const GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    (void)vertexArray;
    (void)startIndex;
    (void)amount;
    //gyMatMultVec(vertexArray, _modelViewVertices, amount);
   // _gyVidTriangleStrip(_modelViewVertices, startIndex, amount);
}

void gyVidColorArray(const struct GYColor4 *const ptr) {
    if(gyVidShadowPass()) return;
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR]);
    _curColorArrayPtr = ptr;
}

void gyVidTexCoordArray(const GYTexCoord2 *const ptr) {
    //assert(curTex); //You should not be submitting this to the untextured shabut in der, dumbass!
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD]);
    _curTexCoordPtr = ptr;
}

void gyVidTangentArray(const GYVector3 *const ptr) {
    if(gyVidShadowPass()) return;
    //glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, 0, ptr);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT]);
    glalTangentVector = ptr;
}

void gyVidNormalArray(const GYVector3 *const ptr) {
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
            glBindTexture(GL_TEXTURE_2D, (GLuint)tex->_data);
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
        case GY_BLEND_ADDITIVE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
        glalBlendMode = mode;
    }
}

void gyVidDepthTest(const int enable) {
#if 1
    if(gyVidShadowPass()) return;
    static int wasShadow = 0;
    int passChange = wasShadow != gyVidShadowPass();
    if(enable != glalDepthTest || passChange) {
        gyVidVertexBatchFlush();
        if(!enable) glDisable(GL_DEPTH_TEST);
        else glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glalDepthTest = enable;
        wasShadow = gyVidShadowPass();
    }
#endif
}

void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev) {
    if(gyVidShadowPass()) return;
    //static int wasShadow = 0;
   // int passChange = (wasShadow != gyVidShadowPass());

    //Does this even do anything now that we're shader based?

    if(lev != glalAlphaLevel) {
        gyVidVertexBatchFlush();
        switch(lev) {
        case GY_ALPHA_NONE:
            glDisable(GL_BLEND);
            break;
        default:
        case GY_ALPHA_PUNCHTHRU:
            glEnable(GL_BLEND);
            break;
        }
        glalAlphaLevel = lev;
      //  wasShadow = gyVidShadowPass();
    }
}


void gyVidQuadListBegin(const GYColor4 *const color) {
    _curQuadListColor[0] = *color;
    _curQuadListColor[1] = *color;
    _curQuadListColor[2] = *color;
    _curQuadListColor[3] = *color;
    _curColorArrayPtr = _curQuadListColor;
    gyVidVertexBatchBegin(&_flushStreamVertices, NULL);
}

void gyVidQuadListEnd(void) {
   // _flushStreamVertices(NULL);
  //  gyVidVertexBatchEnd();
    //gyVidTexCoordArray(_stripTexCoords);
    //gyVidNormalArray(_stripNormals);
    //gyVidTangentArray(_stripTangents);
   // gyVidColorArray(_stripColors);
  //  _gyVidTriangleStrip(_modelViewVertices, 0, glalCurStripSize);
    _curTexCoordPtr = NULL;
    _curColorArrayPtr = NULL;
}

// ==== HIGH-LEVEL API ====
// (glorified wrappers around low-level API)

void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type) {
    glalPlaneType = type;
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
    glalRectTexOrient = orient;
}


void gyVidQuad(const GYVector4 *const verts, const int frame) {
    GYVector4           worldVerts[4];
    const GYTexCoord2*  coord;
    const GYVector3*    tangent;
    const GYVector3*    normal;

   // if(!gyVidShadowPass()) {
        tangent = &glalRectTangents[glalRectTexOrient];
        normal = glalPlaneNormalVector(glalNormalMapActive, glalPlaneType, glalRectTexOrient);

        gyMatMultVec(verts, worldVerts, 4);
        //Connect current and previous polys with degenerate triangles to continue batching operation
        if(_vertexQueueSize) {

            for(unsigned i = 0; i < 4; ++i) {
                _vertexQueue[_vertexQueueSize+i+2].pos      = worldVerts[i];
                _vertexQueue[_vertexQueueSize+i+2].normal   = *normal;
                _vertexQueue[_vertexQueueSize+i+2].tangent  = *tangent;
                _vertexQueue[_vertexQueueSize+i+2].normalZ  = glalOrientNormalMapZFactor(glalRectTexOrient);
            }

            _vertexQueue[_vertexQueueSize].pos      = _vertexQueue[_vertexQueueSize-1].pos;
            _vertexQueue[_vertexQueueSize+1].pos    = _vertexQueue[_vertexQueueSize+2].pos;

            _vertexQueueSize += 2;
        } else {

            for(unsigned i = 0; i < 4; ++i) {
                _vertexQueue[_vertexQueueSize+i].pos        = worldVerts[i];
                _vertexQueue[_vertexQueueSize+i].normal     = *normal;
                _vertexQueue[_vertexQueueSize+i].tangent    = *tangent;
            }
        }

        _vertexQueue[_vertexQueueSize].color      = _curColorArrayPtr[0];
        _vertexQueue[_vertexQueueSize+1].color    = _curColorArrayPtr[1];
        _vertexQueue[_vertexQueueSize+2].color    = _curColorArrayPtr[2];
        _vertexQueue[_vertexQueueSize+3].color    = _curColorArrayPtr[3];

   // }

    if(glalCurTex) {
        coord = (frame != -1)? gyVidTexCoordArrayFromFrame(&glalCurTex->_coord->texCoord[frame], glalRectTexOrient) :
                               _curTexCoordPtr;

        for(unsigned i = 0; i < 4; ++i) {
            _vertexQueue[_vertexQueueSize+i].texCoord = coord[i];
        }

     //   memcpy(&_stripTexCoords[glalCurStripSize], coord, sizeof(GYTexCoord2)*4);

    }

    _vertexQueueSize += 4;

    if((_vertexQueueSize + 6) >= GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE) {
        gyVidVertexBatchFlush();

        _vertexQueueSize = 0;
        _vertexQueueLastSize = 0;
        _vertexQueueLastOffset = 0;
        gyVidVertexArrayBind(_streamVao[gyVidShadowPass()]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE, NULL, GL_STREAM_DRAW);
    }


    //assert(_vertexQueue < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
}

void gyVidRect(const int frame) {
    gyVidQuad(glalRectVerts[glalPlaneType], frame);
}


//FUCKING GAY!!!! TRANSFORM IN PLACE!!!
//We aren't taking normalMapZ into account here!!!
static float worldVertices[GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE];
//Spine interop
void gyVidSkeleton(const spSkeleton*const skeleton) {
    spSlot*                     slot                = NULL;
    spAttachment*               attachment          = NULL;
    GYTexture*                  texture             = NULL;
    spRegionAttachment*         regionAttachment    = NULL;
    spAtlasRegion*              atlasRegion         = NULL;
    spAtlasPage*                atlasPage           = NULL;
    spMeshAttachment*           mesh                = NULL;
    spSkinnedMeshAttachment*    skinnedMesh         = NULL;

    //float                       width, height;
    //GYColor4                    color;
    //int                         i;
    //int                         index;
    const float                 zEpsilon = 0.0f;
    float                       zOffset;

    gyVidNormalMapBind(NULL);
    gyVidSpecularMapBind(NULL);
    gyVidMaterialBind(NULL);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    gyVidDepthTest(0);

    gyVidVertexBatchFlush();
    gyVidVertexBatchBegin(&_flushStreamVertices, NULL);
    GYVertex*                   startVertex = &_vertexQueue[_vertexQueueSize];
    GYVertex*                   vertex = startVertex;

    for (int i = 0; i < skeleton->slotsCount; ++i) {
    //for (int i = skeleton->slotsCount-1; i >= 0; --i) {
        startVertex = &_vertexQueue[_vertexQueueSize];
        vertex = startVertex;

        slot = skeleton->drawOrder[i];
        attachment = slot->attachment;
        if (!attachment) continue;
        texture = NULL;
        zOffset = zEpsilon * (float)i;

        //handle custom attachment types by deffering to application layer callbacks.
        if(attachment->type > SP_ATTACHMENT_SKINNED_MESH) {

            //GYMatrix4 mat;
            gyVidVertexBatchFlush();
            gyVidVertexBatchEnd();
            //gyMatStore(&mat);
            //gyMatPop();
            gyMatPush();
            gyMatLoadIdentity();

            gyVidSkeletonAttachment(attachment);

            gyMatPop();
            //gyMatPush();
            //gyMatLoad(&mat);

            gyVidVertexBatchBegin(&_flushStreamVertices, NULL);
            startVertex = &_vertexQueue[_vertexQueueSize];
            vertex = startVertex;
            gyVidNormalMapBind(NULL);
            gyVidSpecularMapBind(NULL);
            gyVidMaterialBind(NULL);
            gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
            gyVidAlphaLevel(GY_ALPHA_FULL);
            gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
            gyVidDepthTest(0);
            continue;
        }

        if (attachment->type == SP_ATTACHMENT_REGION) {
            regionAttachment    = (spRegionAttachment*)attachment;
            atlasRegion         = regionAttachment? regionAttachment->rendererObject : NULL;
            atlasPage           = atlasRegion? atlasRegion->page : NULL;
            texture             = atlasPage? ((GYSpineRenderObject*)atlasPage->rendererObject)->textureMap : NULL;

            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);

            GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };

            if(texture) {
//                width = texture->width;
//                height = texture->height;
            }

            vertex->pos.x = worldVertices[SP_VERTEX_X2];
            vertex->pos.y = worldVertices[SP_VERTEX_Y2];
            vertex->pos.z = worldVertices[SP_VERTEX_Z2];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X2];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y2];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X1];
            vertex->pos.y = worldVertices[SP_VERTEX_Y1];
            vertex->pos.z = worldVertices[SP_VERTEX_Z1];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X1];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y1];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;



            vertex->pos.x = worldVertices[SP_VERTEX_X3];
            vertex->pos.y = worldVertices[SP_VERTEX_Y3];
            vertex->pos.z = worldVertices[SP_VERTEX_Z3];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X3];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y3];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;


            vertex->pos.x = worldVertices[SP_VERTEX_X3];
            vertex->pos.y = worldVertices[SP_VERTEX_Y3];
            vertex->pos.z = worldVertices[SP_VERTEX_Z3];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X3];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y3];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X4];
            vertex->pos.y = worldVertices[SP_VERTEX_Y4];
            vertex->pos.z = worldVertices[SP_VERTEX_Z4];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X4];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y4];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;

            vertex->pos.x = worldVertices[SP_VERTEX_X1];
            vertex->pos.y = worldVertices[SP_VERTEX_Y1];
            vertex->pos.z = worldVertices[SP_VERTEX_Z1];
            vertex->pos.w = 1.0f;
            vertex->texCoord.u = regionAttachment->uvs[SP_VERTEX_X1];// * width;
            vertex->texCoord.v = regionAttachment->uvs[SP_VERTEX_Y1];// * height;
            vertex->color = *(GYColor4*)&color;
            vertex->normalZ = 1.0f;
            ++vertex;

        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            mesh = (spMeshAttachment*)attachment;

            mesh                = (spMeshAttachment*)attachment;
            atlasRegion         = mesh? mesh->rendererObject : NULL;
            atlasPage           = atlasRegion? atlasRegion->page : NULL;
            texture             = atlasPage? ((GYSpineRenderObject*)atlasPage->rendererObject)->textureMap : NULL;


            //if (mesh->verticesCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
            texture = regionAttachment?
                        (GYTexture*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject :
                        NULL;
            spMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

           GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };

            if(texture) {
//                width = texture->width;
//                height = texture->height;
            }

            for (int j = 0; j < mesh->trianglesCount; ++j) { //loop unroll?
                int index = mesh->triangles[j] << 1;
                vertex->pos.x = worldVertices[index];
                vertex->pos.y = worldVertices[index + 1];
                vertex->pos.z = 80.0f + zOffset;
                vertex->pos.w = 1.0f;
                vertex->texCoord.u = mesh->uvs[index];// * width;
                vertex->texCoord.v = mesh->uvs[index + 1];// * height;
                vertex->color = *(GYColor4*)&color;
                vertex->normalZ = 1.0f;
                ++vertex;
            }


        } else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH) {
            skinnedMesh = (spSkinnedMeshAttachment*)attachment;
           // if (mesh->uvsCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
            atlasRegion         = skinnedMesh? skinnedMesh->rendererObject : NULL;
            atlasPage           = atlasRegion? atlasRegion->page : NULL;
            texture             = atlasPage? ((GYSpineRenderObject*)atlasPage->rendererObject)->textureMap : NULL;

            spSkinnedMeshAttachment_computeWorldVertices(skinnedMesh, slot, worldVertices);

            GYColor4 color = {
                (unsigned char)(skeleton->r * slot->r * 255),
                (unsigned char)(skeleton->g * slot->g * 255),
                (unsigned char)(skeleton->b * slot->b * 255),
                (unsigned char)(skeleton->a * slot->a * 255)
            };

            if(texture) {
//                width = texture->width;
//                height = texture->height;
            }
            for (int j = 0; j < skinnedMesh->trianglesCount; ++j) { //loop unroll?
                int index = skinnedMesh->triangles[j] << 1;
                vertex->pos.x = worldVertices[index];
                vertex->pos.y = worldVertices[index + 1];
                vertex->pos.z = 80.0f + zOffset;
                vertex->pos.w = 1.0f;
                vertex->texCoord.u = skinnedMesh->uvs[index];// * width;
                vertex->texCoord.v = skinnedMesh->uvs[index + 1];// * height;
                vertex->color = *(GYColor4*)&color;
                vertex->normalZ = 1.0f;
                ++vertex;
            }
        }

        if(texture) {

           // if(slot->data->additiveBlending != lastAdditive) {
            if(slot->data->blendMode == SP_BLEND_MODE_ADDITIVE) {
                gyVidBlendMode(GY_BLEND_ONE);
            } else {
                gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
            }

            gyVidTexBind(texture);

            /*
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
*/
            int count = vertex-startVertex;
            _vertexQueueSize += count;
            assert(count < GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE);
            //glDrawArrays(attachment->type == SP_ATTACHMENT_REGION? GL_QUADS : GL_TRIANGLES, 0, count);
            glalVertexRenderCount += count;

            _glPrimitiveType = attachment->type == SP_ATTACHMENT_REGION? GL_TRIANGLE_STRIP : GL_TRIANGLES;

            gyVidVertexBatchFlush();

            if((_vertexQueueSize + 6) >= GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE) {
                gyVidVertexBatchFlush();

                _vertexQueueSize = 0;
                _vertexQueueLastSize = 0;
                _vertexQueueLastOffset = 0;
                gyVidVertexArrayBind(_streamVao[gyVidShadowPass()]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*GYRO_VIDEO_GLAL_MAX_TRI_STRIP_SIZE, NULL, GL_STREAM_DRAW);
            }

        }

    }

    gyVidVertexBatchEnd();
    _glPrimitiveType = GL_TRIANGLE_STRIP;

   // target.draw(*vertexArray, states);
}
