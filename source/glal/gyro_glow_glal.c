#include <gyro_system_api.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_glow_glal.h>
#include <stdio.h>
#if 0

typedef enum __GLOW_SHADER_ATTRIB {
    GLOW_SHADER_POS,
    GLOW_SHADER_TEX_COORD,
    GLOW_SHADER_ATTRIB_MAX
} _GLOW_SHADER_ATTRIB;

typedef enum __GLOW_SHADER_UNIFORM {
    GLOW_SHADER_TEXTURE_MAP,
    GLOW_SHADER_CONV_DIR,
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

static int _glalVidInitializeGlow(void) {

    _gyLog(GY_DEBUG_VERBOSE, "Initializing glow postprocessing effect.");
    _gyPush();

    glGenFramebuffers(1, &_glowBuffFbo);
    glGenFramebuffers(1, &_horizontalConvFbo);
    glGenTextures(1, &_glowBuffTex);
    glGenTextures(1, &_horizontalConvTex);
    glGenTextures(1, &_finalConvTex);

    if(_glowBuffTex == 0 || _horizontalConvVex == 0 || _glowBuffFbo == 0 || _horizontalConvFbo == 0 || _finalConvTex == 0) {
        _gyLog(GY_DEBUG_ERROR, "Error creating FBOs and texture targets.");
        _gyPop(1);
        return 0;
    }

    GLuint textures[3] = {
        _glowBuffTex,
        _horizontalConvBuff,
        _finalConvBuff
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
    }

    _gyPop(2);

    return _glowShader.program != NULL;
}


static int _glalVidUninitializeGlow(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing glow postprocessing effect.");
    _gyPush();

    int success = gyShaderProgramDestroy(_glowShader);
    glDeleteFramebuffers(1, &_glowBuffFbo);
    glDeleteFramebuffers(1, &_horizontalConvFbo);
    glDeleteTextures(1, &_backBuffTex);
    glDeleteTextures(1, &_horizontalConvTex);

    _gyPop(1);
    return success;
}


  typedef struct GYVertex {
    GYVector4	pos;
    GYTexCoord2	texCoord;
    GYColor4	color;
    GYVector3   normal;
    GYVector3   tangent;
} GYVertex;


void gyVidApplyGlow(void) {
    GYVertex vertices[4] = {
        {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, -1.0f, 0.0f }
        },
        {
            { 0.5f, -0.5f, 0.0f, 1.0f },
            { 1.0f, 0.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, -1.0f, 0.0f }
        },
        {
            { -0.5f, 0.5f, 0.0f, 1.0f },
            { 0.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, -1.0f, 0.0f }
        },
        {
            { 0.5f, 0.5f, 0.0f, 1.0f },
            { 1.0f, 1.0f },
            { 255, 255, 255, 255 },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, -1.0f, 0.0f }
        }
    };
    unsigned int scrWidth, scrHeight;

    //1 - Copy backbuffer to texture.
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
    gyVidSetCurrentShaderProgram(_glowShader);
    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_TEXTURE_MAP], glalTextureMapUniforms[GLAL_TEXTURE_MAP]);
    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_CONV_DIR], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _horizontalConvFbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _horizontalConvTex,
                           0);
    glViewport(0, 0, (float)GYRO_GLAL_GLOW_TEXTURE_WIDTH, (float)GYRO_GLAL_GLOW_TEXTURE_HEIGHT);
    glBindTexture(GL_TEXTURE_2D, _glowBuffTex); //not going to work, where the fuck are texture coords coming from!?
    gyMatPush();
    gyMatLoadIdentity();
    gyMatScale(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 1.0f);
    GYColor4 color = { 255, 255, 255, 255 };
    gyVidQuadListBegin(&color);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    gyVidRect(0);


    //3 - Perform vertical convolution on horizontal FBO
    glUniform1i(_glowShader.uniformLoc[GLOW_SHADER_CONV_DIR], 1);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _finalConvTex,
                           0);
     glBindTexture(GL_TEXTURE_2D, _horizBuffTex); //not going to work, where the fuck are texture coords coming from!?
     gyVidRect(0);
     gyVidQuadListEnd();
     gyMatPop();

    //4 - RENDER FULL-SCREEN QUAD
    gyVidSetDefaultShaderProgram();
    glalVidBindDefaultFbo();
    glViewport(0.0f, 0.0f, (float)scrWidth, (float)scrHeight);
    gyMatPush();
    gyMatLoadIdentity();
    gyMatScale(GYRO_GLAL_GLOW_TEXTURE_WIDTH, GYRO_GLAL_GLOW_TEXTURE_HEIGHT, 1.0f);
    color.a = 100;
    gyVidAlphaLevel(GY_ALPHA_FULL);
    glBindTexture(GL_TEXTURE_2D, _finalConvTex);
    gyVidQuadListBegin(&color);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    gyVidRect(0);
    gyVidQuadListEnd();
    gyMatPop();

}

#endif
