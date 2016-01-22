#include <gyro_system_api.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_glow_glal.h>
#include <stdio.h>


typedef enum __GLOW_SHADER_ATTRIB {
    GLOW_SHADER_POS = GLAL_SHADER_POS,
    GLOW_SHADER_TEX_COORD = GLAL_SHADER_TEX_COORD,
    GLOW_SHADER_ATTRIB_MAX
} _GLOW_SHADER_ATTRIB;

typedef enum __GLOW_SHADER_UNIFORM {
    GLOW_SHADER_TEXTURE_MAP,
    GLOW_SHADER_CONV_DIR,
    GLOW_SHADER_PROJ_VIEW_MAT,
    GLOW_SHADER_UNIFORM_MAX
} _GLOW_SHADER_UNIFORM;

typedef struct __GlowShaderProgram {
    GYShaderProgram*            program;
    GLint						attribLoc[GLOW_SHADER_ATTRIB_MAX];
    GLint						uniformLoc[GLOW_SHADER_UNIFORM_MAX];
} _GlowShaderProgram;


//TERMINLOLOGY
//

static _GlowShaderProgram   _glowShader;
static GLuint               _glowBuffFbo;
static GLuint               _glowBuffTex;
static GLuint               _horizontalConvFbo;
static GLuint               _horizontalConvTex;
static GLuint               _finalConvTex;
static GLuint               _glowVao;
static GLuint               _glowVbo;
static GYMatrix4            _projViewMat;


int glalVidInitializeGlow(void) {
    const GYVertex vertices[4] = {
        {
            { -1.0f, -1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        },
        {
            { 1.0f, -1.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        },
        {
            { 1.0f, 1.0f, 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f }
        }

    };

    _gyLog(GY_DEBUG_VERBOSE, "Initializing glow postprocessing effect.");
    _gyPush();

    gyVidOrthoProjMat(_projViewMat, 0.0f, (float)GYRO_GLAL_GLOW_TEXTURE_WIDTH, (float)GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 0.0f, -999.0f, 1000.0f);

    glGenFramebuffers(1, &_glowBuffFbo);
    glGenFramebuffers(1, &_horizontalConvFbo);
    glGenTextures(1, &_glowBuffTex);
    glGenTextures(1, &_horizontalConvTex);
    glGenTextures(1, &_finalConvTex);

    if(_glowBuffTex == 0 || _horizontalConvTex == 0 || _glowBuffFbo == 0 || _horizontalConvFbo == 0 || _finalConvTex == 0) {
        _gyLog(GY_DEBUG_ERROR, "Error creating FBOs and texture targets.");
        _gyPop(1);
        return 0;
    }

    GLuint textures[3] = {
        _glowBuffTex,
        _horizontalConvTex,
        _finalConvTex
    };

    for(unsigned i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GYRO_GLAL_GLOW_TEXTURE_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GYRO_GLAL_GLOW_TEXTURE_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GYRO_GLAL_GLOW_TEXTURE_FILTER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GYRO_GLAL_GLOW_TEXTURE_FILTER);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    _gyLog(GY_DEBUG_VERBOSE, "Loading Glow shaders.");
    _gyPush();

    //Compile and link shaders
    GYShader* vertShader = gyShaderCreate(GYRO_GLAL_GLOW_VERTEX_SHADER);
    GYShader* fragShader = gyShaderCreate(GYRO_GLAL_GLOW_FRAGMENT_SHADER);
    _glowShader.program = gyShaderProgramCreate(vertShader, fragShader);

    //Query for shader attrib and uniform locations
    if(_glowShader.program) {
        _glowShader.attribLoc[GLOW_SHADER_POS]            = _protectedAttribQuery(_glowShader.program, "aPosition", 0);
        _glowShader.attribLoc[GLOW_SHADER_TEX_COORD]      = _protectedAttribQuery(_glowShader.program, "aTexCoord", 0);
        _glowShader.uniformLoc[GLOW_SHADER_TEXTURE_MAP]   = _protectedUniformQuery(_glowShader.program, "uTexture", 0);
        _glowShader.uniformLoc[GLOW_SHADER_CONV_DIR]      = _protectedUniformQuery(_glowShader.program, "uConvDir", 0);
        _glowShader.uniformLoc[GLOW_SHADER_PROJ_VIEW_MAT] = _protectedUniformQuery(_glowShader.program, "uProjViewMat", 0);
    }

    glGenBuffers(1, &_glowVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _glowVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*4, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_glowVao);
    gyVidVertexArrayBind(_glowVao);
    glBindBuffer(GL_ARRAY_BUFFER, _glowVbo);

    glEnableVertexAttribArray(_glowShader.attribLoc[GLOW_SHADER_POS]);
    glEnableVertexAttribArray(_glowShader.attribLoc[GLOW_SHADER_TEX_COORD]);

    glVertexAttribPointer(_glowShader.attribLoc[GLOW_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
    glVertexAttribPointer(_glowShader.attribLoc[GLOW_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));
    gyVidVertexArrayBind(0);

    _gyPop(2);

    return _glowShader.program != NULL;
}


int glalVidUninitializeGlow(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing glow postprocessing effect.");
    _gyPush();

    int success = gyShaderProgramDestroy(_glowShader.program);
    glDeleteFramebuffers(1, &_glowBuffFbo);
    glDeleteFramebuffers(1, &_horizontalConvFbo);
    glDeleteTextures(1, &_glowBuffTex);
    glDeleteTextures(1, &_horizontalConvTex);

    _gyPop(1);
    return success;
}


void gyVidApplyGlow(void) {

    GYTexCoord2 texCoords[4] = {
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };



    unsigned int scrWidth, scrHeight;

    //1 - Copy backbuffer to texture.
    gyVidVertexBatchEnd();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, glalVidDefaultFbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _glowBuffFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _glowBuffTex,
                           0);
    gyVidResolution(&scrWidth, &scrHeight);
    glBlitFramebuffer(0, 0, scrWidth, scrHeight, 0, 0, GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);


    //2 - Perform horizontal convolution on glow FBO.
    gyVidSetCurrentShaderProgram(_glowShader.program);
    //glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_TEXTURE_MAP], glalTextureMapUniforms[GLAL_TEXTURE_MAP]);
    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_CONV_DIR], 0);
    //glUniformMatrix4fv(_glowShader.uniformLoc[GLOW_SHADER_PROJ_VIEW_MAT], 1, GL_FALSE, (float*)_projViewMat);


    glBindFramebuffer(GL_FRAMEBUFFER, _horizontalConvFbo);
            glBindTexture(GL_TEXTURE_2D, _horizontalConvTex); //not going to work, where the fuck are texture coords coming from!?
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _horizontalConvTex,
                           0);
    glViewport(0, 0, (float)GYRO_GLAL_GLOW_TEXTURE_WIDTH, (float)GYRO_GLAL_GLOW_TEXTURE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
    GLenum status;
     if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
         _gyLog(GY_DEBUG_WARNING, "Binding GLAL depth texture to frame buffer has reested - %d!", status);
         return;
     }

   // glalVidOglInit(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT);
    //glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
    glBindTexture(GL_TEXTURE_2D, _glowBuffTex); //not going to work, where the fuck are texture coords coming from!?
     gyVidVertexArrayBind(_glowVao);
     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_CONV_DIR], 1);
    // glBindFramebuffer(GL_FRAMEBUFFER, _horizontalConvFbo);
      //       glBindTexture(GL_TEXTURE_2D, _horizontalConvTex); //not going to work, where the fuck are texture coords coming from!?
     glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                            GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D,
                            _finalConvTex,
                            0);
     //glViewport(0, 0, (float)GYRO_GLAL_GLOW_TEXTURE_WIDTH, (float)GYRO_GLAL_GLOW_TEXTURE_HEIGHT);
     // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     //GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
     //glDrawBuffers(1, attachments);
       glClear(GL_COLOR_BUFFER_BIT);
      if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
          _gyLog(GY_DEBUG_WARNING, "Binding GLAL depth texture to frame buffer has reested - %d!", status);
          return;
      }

    // glalVidOglInit(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT);
     //glActiveTexture(glalTextureMapTexUnits[GLAL_TEXTURE_MAP]);
     glBindTexture(GL_TEXTURE_2D, _horizontalConvTex); //not going to work, where the fuck are texture coords coming from!?
      gyVidVertexArrayBind(_glowVao);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



#if 0
    //3 - Perform vertical convolution on horizontal FBO
    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_CONV_DIR], 1);
        glBindTexture(GL_TEXTURE_2D, _finalConvTex); //not going to work, where the fuck are texture coords coming from!?
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _finalConvTex,
                           0);
        glDrawBuffers(1, attachments);
     //glBindTexture(GL_TEXTURE_2D, _horizontalConvTex); //not going to work, where the fuck are texture coords coming from!?
     //gyVidTexCoordArray(texCoords);
     gyVidQuadListBegin(&color);
     gyVidRect(-1);
     gyVidQuadListEnd();
     gyMatPop();
     gyVidVertexBatchEnd();
#endif
    //4 - RENDER FULL-SCREEN QUAD
         GYColor4 color = { 255, 255, 255, 255 };
    gyVidSetDefaultShaderProgram();
    gyVidVertexArrayBind(0);
    glalVidBindDefaultFbo();
   // glalVidOglInit(scrWidth, scrHeight, scrWidth, scrHeight);
    glViewport(0.0f, 0.0f, (float)scrWidth, (float)scrHeight);

    color.a = 100;
    gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidBlendMode(GY_BLEND_ONE);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
#if 0
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(GYRO_GLAL_GLOW_TEXTURE_WIDTH*0.5f, GYRO_GLAL_GLOW_TEXTURE_HEIGHT*0.5f, 1.0f);
    gyMatScale(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _glowBuffTex);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color);
    gyVidRect(-1);
    gyVidQuadListEnd();
    gyMatPop();

    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(GYRO_GLAL_GLOW_TEXTURE_WIDTH*2.0f, GYRO_GLAL_GLOW_TEXTURE_HEIGHT*0.5f, 0.0f);
    gyMatScale(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _horizontalConvTex);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color);
    gyVidRect(-1);
    gyVidQuadListEnd();
    gyMatPop();
#endif
#if 0
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(GYRO_GLAL_GLOW_TEXTURE_WIDTH*2.0f, GYRO_GLAL_GLOW_TEXTURE_HEIGHT*2.0f, 0.0f);
    gyMatScale(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _finalConvTex);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color);
    gyVidRect(-1);
    gyVidQuadListEnd();
    gyMatPop();
#else
    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslate(scrWidth*0.5f, scrHeight*0.5f, 0.0f);
    gyMatScale(scrWidth, scrHeight, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _finalConvTex);
    gyVidTexCoordArray(texCoords);
    gyVidQuadListBegin(&color);
    gyVidRect(-1);
    gyVidQuadListEnd();
    gyMatPop();

    gyVidVertexBatchEnd();
#endif
    gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
}


