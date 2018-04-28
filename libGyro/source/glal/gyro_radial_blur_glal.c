#include <glal/gyro_radial_blur_glal.h>
#include <gyro_system_api.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_gl_utils.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>
#include <stdio.h>

typedef enum __RADIAL_BLUR_SHADER_ATTRIB {
    RADIAL_BLUR_SHADER_POS         = GLAL_SHADER_POS,
   // RADIAL_BLUR_SHADER_TEX_COORD   = GLAL_SHADER_TEX_COORD,
    RADIAL_BLUR_SHADER_ATTRIB_MAX
} _RADIAL_BLUR_SHADER_ATTRIB;

typedef enum _RADIAL_BLUR_SHADER_UNIFORM {
    RADIAL_BLUR_SHADER_TEXTURE_MAP,
    RADIAL_BLUR_SHADER_UNIFORM_MAX
} _RADIAL_BLUR_SHADER_UNIFORM;

typedef struct _RadialBlurShaderProgram {
    GYShaderProgram*            program;
    GLint						attribLoc[RADIAL_BLUR_SHADER_ATTRIB_MAX];
    GLint						uniformLoc[RADIAL_BLUR_SHADER_UNIFORM_MAX];
} _RadialBlurShaderProgram;


static _RadialBlurShaderProgram     _radialBlurShader;
static GLuint                       _radialBlurVao;
static GLuint                       _radialBlurVbo;
extern GLuint                       _glalColorTex;

int glalVidInitializeRadialBlur(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Initializing Radial Blur Postprocessing Effect.");
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "Loading Shaders.");
    _gyPush();

    //Compile and link shaders
    GYShader* vertShader    = gyShaderCreate(GYRO_GLAL_RADIAL_BLUR_VERTEX_SHADER);
    GYShader* fragShader    = gyShaderCreate(GYRO_GLAL_RADIAL_BLUR_FRAGMENT_SHADER);
    _radialBlurShader.program     = gyShaderProgramCreate(vertShader, fragShader);
    GY_GL_CHECK_ERROR();

    //Query for shader attrib and uniform locations
    if(_radialBlurShader.program) {
        _radialBlurShader.attribLoc[RADIAL_BLUR_SHADER_POS]            = _protectedAttribQuery(_radialBlurShader.program, "aPosition", 0);
        _radialBlurShader.uniformLoc[RADIAL_BLUR_SHADER_TEXTURE_MAP]   = _protectedUniformQuery(_radialBlurShader.program, "uTexture", 0);
    }

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


    glGenBuffers(1, &_radialBlurVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _radialBlurVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*4, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_radialBlurVao);
    gyVidVertexArrayBind(_radialBlurVao);
    glBindBuffer(GL_ARRAY_BUFFER, _radialBlurVbo);

    glEnableVertexAttribArray(_radialBlurShader.attribLoc[RADIAL_BLUR_SHADER_POS]);

    glVertexAttribPointer(_radialBlurShader.attribLoc[RADIAL_BLUR_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
    gyVidVertexArrayBind(0);
    _gyPop(2);

    return _radialBlurShader.program != NULL;
}

int glalVidUnitializeRadialBlur(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing Radial Blur postprocessing effect.");
    _gyPush();

    int success = gyShaderProgramDestroy(_radialBlurShader.program);
    glDeleteVertexArrays(1, &_radialBlurVao);
    glDeleteBuffers(1, &_radialBlurVbo);

    _gyPop(1);
    return success;

}


void glalVidApplyRadialBlur(void) {

    gyVidVertexBatchFlush();

    gyVidSetCurrentShaderProgram(_radialBlurShader.program);
    glUniform1i(_radialBlurShader.uniformLoc[RADIAL_BLUR_SHADER_TEXTURE_MAP], 0);

    glBindTexture(GL_TEXTURE_2D, _glalColorTex);
    gyVidVertexArrayBind(_radialBlurVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gyVidVertexBatchFlush();
    gyVidSetDefaultShaderProgram();
    gyVidVertexArrayBind(0);

}
