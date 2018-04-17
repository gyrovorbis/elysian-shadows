#include <gyro_system_api.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_bloom_glal.h>
#include <stdio.h>


typedef enum __BLOOM_SHADER_ATTRIB {
    BLOOM_SHADER_POS         = GLAL_SHADER_POS,
    BLOOM_SHADER_TEX_COORD   = GLAL_SHADER_TEX_COORD,
    BLOOM_SHADER_ATTRIB_MAX
} _BLOOM_SHADER_ATTRIB;

typedef enum __BLOOM_SHADER_UNIFORM {
    BLOOM_SHADER_TEXTURE_MAP,
    BLOOM_SHADER_CONV_DIR,
    BLOOM_SHADER_UNIFORM_MAX
} _BLOOM_SHADER_UNIFORM;

typedef struct _BloomShaderProgram {
    GYShaderProgram*            program;
    GLint						attribLoc[BLOOM_SHADER_ATTRIB_MAX];
    GLint						uniformLoc[BLOOM_SHADER_UNIFORM_MAX];
} _BloomShaderProgram;

extern GLuint               _glalMainFbo;
extern GLuint               _glalColorTex;
extern GLuint               _glalGlowTex;


//TERMINLOLOGY
//

static _BloomShaderProgram   _bloomShader;
static GLuint               _bloomBuffFbo;
static GLuint               _bloomBuffTex;
static GLuint               _horizontalConvFbo;
static GLuint               _horizontalConvTex;
static GLuint               _finalConvTex;
static GLuint               _bloomVao;
static GLuint               _bloomVbo;


int glalVidInitializeBloom(void) {
    //Vertex data for a full-screen quad.
    const GYVertex vertices[4] = {
        {
            { -1.0f, -1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f },
            1.0f
        },
        {
            { 1.0f, -1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f },
            1.0f
        },
        {
            { -1.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f },
            1.0f
        },
        {
            { 1.0f, 1.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f },
            1.0f
        }

    };

    _gyLog(GY_DEBUG_VERBOSE, "Initializing Bloom postprocessing effect.");
    _gyPush();

    glGenFramebuffers(1, &_bloomBuffFbo);
    glGenFramebuffers(1, &_horizontalConvFbo);
    glGenTextures(1, &_bloomBuffTex);
    glGenTextures(1, &_horizontalConvTex);
    glGenTextures(1, &_finalConvTex);

    if(_bloomBuffTex == 0 || _horizontalConvTex == 0 || _bloomBuffFbo == 0 || _horizontalConvFbo == 0 || _finalConvTex == 0) {
        _gyLog(GY_DEBUG_ERROR, "Error creating FBOs and texture targets.");
        _gyPop(1);
        return 0;
    }

    GLuint textures[3] = {
        _bloomBuffTex,
        _horizontalConvTex,
        _finalConvTex
    };

    for(unsigned i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GYRO_GLAL_BLOOM_TEXTURE_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GYRO_GLAL_BLOOM_TEXTURE_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GYRO_GLAL_BLOOM_TEXTURE_FILTER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GYRO_GLAL_BLOOM_TEXTURE_FILTER);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GYRO_GLAL_BLOOM_TEXTURE_WIDTH, GYRO_GLAL_BLOOM_TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    _gyLog(GY_DEBUG_VERBOSE, "Loading Bloom shaders.");
    _gyPush();

    //Compile and link shaders
    GYShader* vertShader    = gyShaderCreate(GYRO_GLAL_BLOOM_VERTEX_SHADER);
    GYShader* fragShader    = gyShaderCreate(GYRO_GLAL_BLOOM_FRAGMENT_SHADER);
    _bloomShader.program     = gyShaderProgramCreate(vertShader, fragShader);

    //Query for shader attrib and uniform locations
    if(_bloomShader.program) {
        _bloomShader.attribLoc[BLOOM_SHADER_POS]            = _protectedAttribQuery(_bloomShader.program, "aPosition", 0);
        _bloomShader.attribLoc[BLOOM_SHADER_TEX_COORD]      = _protectedAttribQuery(_bloomShader.program, "aTexCoord", 0);
        _bloomShader.uniformLoc[BLOOM_SHADER_TEXTURE_MAP]   = _protectedUniformQuery(_bloomShader.program, "uTexture", 0);
        _bloomShader.uniformLoc[BLOOM_SHADER_CONV_DIR]      = _protectedUniformQuery(_bloomShader.program, "uConvDir", 0);
    }

    glGenBuffers(1, &_bloomVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _bloomVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*4, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_bloomVao);
    gyVidVertexArrayBind(_bloomVao);
    glBindBuffer(GL_ARRAY_BUFFER, _bloomVbo);

    glEnableVertexAttribArray(_bloomShader.attribLoc[BLOOM_SHADER_POS]);
    glEnableVertexAttribArray(_bloomShader.attribLoc[BLOOM_SHADER_TEX_COORD]);

    glVertexAttribPointer(_bloomShader.attribLoc[BLOOM_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
    glVertexAttribPointer(_bloomShader.attribLoc[BLOOM_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));
    gyVidVertexArrayBind(0);
    
    //=====ATTACH BLOOM ATTACHMENT TO DEFAULT FRAMEBUFFER=======
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, glalVidDefaultFbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _bloomBuffFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _bloomBuffTex,
                           0);
    //gyVidResolution(&scrWidth, &scrHeight);

    _gyPop(2);

    return _bloomShader.program != NULL;
}


int glalVidUninitializeBLOOM(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing Bloom postprocessing effect.");
    _gyPush();

    int success = gyShaderProgramDestroy(_bloomShader.program);
    glDeleteFramebuffers(1, &_bloomBuffFbo);
    glDeleteFramebuffers(1, &_horizontalConvFbo);
    glDeleteTextures(1, &_bloomBuffTex);
    glDeleteTextures(1, &_horizontalConvTex);
    glDeleteTextures(1, &_finalConvTex);

    _gyPop(1);
    return success;
}


void gyVidApplyBloom(void) {

    GYTexCoord2 texCoords[4] = {
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    unsigned int scrWidth, scrHeight;
    gyVidResolution(&scrWidth, &scrHeight);

    //1 - Copy backbuffer to texture.
    gyVidVertexBatchFlush();


#if 1
#if 0
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _glalMainFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _bloomBuffFbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _bloomBuffTex,
                           0);
    gyVidResolution(&scrWidth, &scrHeight);
    glBlitFramebuffer(0, 0, GLAL_RENDERBUFFER_WIDTH, GLAL_RENDERBUFFER_HEIGHT, 0, 0, GYRO_GLAL_BLOOM_TEXTURE_WIDTH, GYRO_GLAL_BLOOM_TEXTURE_HEIGHT, GL_COLOR_BUFFER_BIT, GYRO_GLAL_BLOOM_BLIT_FILTER);
#else

#endif

    //===== 2 - Perform horizontal convolution on BLOOM buffer texture =====
    gyVidSetCurrentShaderProgram(_bloomShader.program);
    glUniform1i(_bloomShader.uniformLoc[BLOOM_SHADER_CONV_DIR], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _horizontalConvFbo);
    glBindTexture(GL_TEXTURE_2D, _horizontalConvTex);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _horizontalConvTex,
                           0);
    glViewport(0, 0, (float)GYRO_GLAL_BLOOM_TEXTURE_WIDTH, (float)GYRO_GLAL_BLOOM_TEXTURE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    //GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    //glDrawBuffers(1, attachments);
    GLenum status;
     if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
         _gyLog(GY_DEBUG_ERROR, "Binding GLAL horizontal bloom texture to frame buffer has reested - %d!", status);
         return;
     }

    //Render fullscreen quad
    glBindTexture(GL_TEXTURE_2D, _glalGlowTex);
    gyVidVertexArrayBind(_bloomVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        gyVidVertexBatchFlush();


    //===== 3 - Perform vertical convolution on horizontal convolution texture =====
    glUniform1i(_bloomShader.uniformLoc[BLOOM_SHADER_CONV_DIR], 1);
     glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                            GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D,
                            _finalConvTex,
                            0);
    glClear(GL_COLOR_BUFFER_BIT);
    if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
        _gyLog(GY_DEBUG_WARNING, "Binding GLAL vertical bloom texture to frame buffer has reested - %d!", status);
        return;
    }

    //Render fullscreen quad
    glBindTexture(GL_TEXTURE_2D, _horizontalConvTex);
    gyVidVertexArrayBind(_bloomVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    gyVidVertexBatchFlush();
    //===== 4 - Additively blend blurred bloom texture over backbuffer as a fullscreen quad.  =====
#endif
    gyVidSetDefaultShaderProgram();
   gyVidVertexArrayBind(0);
    glalVidBindDefaultFbo();
    glViewport(0.0f, 0.0f, (float)scrWidth, (float)scrHeight);

#if 0
    gyVidAlphaLevel(GY_ALPHA_NONE);

#else
    gyVidDepthTest(0);
   // gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
    //gyVidAlphaLevel(GY_ALPHA_FULL);
#endif
        GYColor4 col = { 0, 0, 0, 255 };
    gyVidTexBind(NULL);

            glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], 0);
   // gyVidNormalMapBind(NULL);
    //gyVidSpecularMapBind(NULL);
    //Render fullscreen quad.
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);
    gyMatScale(scrWidth, scrHeight, 1.0f);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&col);
    gyVidRect(-1);
    //gyVidQuadListEnd();
    //gyVidVertexBatchEnd();
    gyMatPop();

    gyVidVertexBatchFlush();

  //  gyVidAlphaLevel(GY_ALPHA_FULL);
                glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], 1);
  //  gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);

        GYColor4 color = { 255, 255, 255, 255 };
    static GYTexture tex;
    tex._data = _glalColorTex;
    gyVidTexBind(NULL);
    gyVidTexBind(&tex);
            glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], 1);
   // gyVidNormalMapBind(NULL);
    //gyVidSpecularMapBind(NULL);
    //Render fullscreen quad.
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);
    gyMatScale(scrWidth, scrHeight, 1.0f);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color);
    gyVidRect(-1);
    //gyVidQuadListEnd();
    //gyVidVertexBatchEnd();
    gyMatPop();
        gyVidVertexBatchFlush();

#if 1
    gyVidAlphaLevel(GY_ALPHA_FULL);
        GYColor4 color2 = { 255, 255, 255, 255 };
gyVidBlendMode(GY_BLEND_ONE);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);    

    tex._data = _finalConvTex;
    gyVidTexBind(NULL);
    gyVidTexBind(&tex);
    glUniform1i(glalCurLightShader->uniformLoc[GLAL_SHADER_TEXTURE_ENABLED], 1);
    //Render fullscreen quad.
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);
    gyMatScale(scrWidth, scrHeight, 1.0f);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color2);
    gyVidRect(-1);
   // gyVidVertexBatchEnd();
    gyMatPop();

#endif

    gyVidVertexBatchFlush();
    gyVidAlphaLevel(GY_ALPHA_NONE);

}


