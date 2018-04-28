#include <glal/gyro_chromatic_aberration_glal.h>
#include <gyro_system_api.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_gl_utils.h>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_shaders_glal.h>
#include <stdio.h>

typedef enum __CHROMATIC_ABERRATION_SHADER_ATTRIB {
    CHROMATIC_ABERRATION_SHADER_POS         = GLAL_SHADER_POS,
   // CHROMATIC_ABERRATION_SHADER_TEX_COORD   = GLAL_SHADER_TEX_COORD,
    CHROMATIC_ABERRATION_SHADER_ATTRIB_MAX
} _CHROMATIC_ABERRATION_SHADER_ATTRIB;

typedef enum _CHROMATIC_ABERRATION_SHADER_UNIFORM {
    CHROMATIC_ABERRATION_SHADER_TEXTURE_MAP,
    CHROMATIC_ABERRATION_SHADER_UNIFORM_MAX
} _CHROMATIC_ABERRATION_SHADER_UNIFORM;

typedef struct _ChromaticAberrationShaderProgram {
    GYShaderProgram*            program;
    GLint						attribLoc[CHROMATIC_ABERRATION_SHADER_ATTRIB_MAX];
    GLint						uniformLoc[CHROMATIC_ABERRATION_SHADER_UNIFORM_MAX];
} _ChromaticAberrationShaderProgram;


static _ChromaticAberrationShaderProgram    _chromaticAberrationShader;
static GLuint                               _chromaticAberrationVao;
static GLuint                               _chromaticAberrationVbo;
extern GLuint                               _glalColorTex;

int glalVidInitializeChromaticAberration(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Initializing 'Chromatic Abhoration' Postprocessing Effect.");
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "Loading Shaders.");
    _gyPush();

    //Compile and link shaders
    GYShader* vertShader    = gyShaderCreate(GYRO_GLAL_CHROMATIC_ABERRATION_VERTEX_SHADER);
    GYShader* fragShader    = gyShaderCreate(GYRO_GLAL_CHROMATIC_ABERRATION_FRAGMENT_SHADER);
    _chromaticAberrationShader.program     = gyShaderProgramCreate(vertShader, fragShader);
    GY_GL_CHECK_ERROR();

    //Query for shader attrib and uniform locations
    if(_chromaticAberrationShader.program) {
        _chromaticAberrationShader.attribLoc[CHROMATIC_ABERRATION_SHADER_POS]            = _protectedAttribQuery(_chromaticAberrationShader.program, "aPosition", 0);
        _chromaticAberrationShader.uniformLoc[CHROMATIC_ABERRATION_SHADER_TEXTURE_MAP]   = _protectedUniformQuery(_chromaticAberrationShader.program, "uTexture", 0);
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


    glGenBuffers(1, &_chromaticAberrationVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _chromaticAberrationVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*4, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_chromaticAberrationVao);
    gyVidVertexArrayBind(_chromaticAberrationVao);
    glBindBuffer(GL_ARRAY_BUFFER, _chromaticAberrationVbo);

    glEnableVertexAttribArray(_chromaticAberrationShader.attribLoc[CHROMATIC_ABERRATION_SHADER_POS]);

    glVertexAttribPointer(_chromaticAberrationShader.attribLoc[CHROMATIC_ABERRATION_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
    gyVidVertexArrayBind(0);
    _gyPop(2);

    return _chromaticAberrationShader.program != NULL;
}

int glalVidUnitializeChromaticAberration(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Uninitializing 'Chromatic Abhoration' postprocessing effect.");
    _gyPush();

    int success = gyShaderProgramDestroy(_chromaticAberrationShader.program);
    glDeleteVertexArrays(1, &_chromaticAberrationVao);
    glDeleteBuffers(1, &_chromaticAberrationVbo);

    _gyPop(1);
    return success;

}


void glalVidApplyChromaticAberration(void) {

    gyVidVertexBatchFlush();

    gyVidSetCurrentShaderProgram(_chromaticAberrationShader.program);
    glUniform1i(_chromaticAberrationShader.uniformLoc[CHROMATIC_ABERRATION_SHADER_TEXTURE_MAP], 0);

    glBindTexture(GL_TEXTURE_2D, _glalColorTex);
    gyVidVertexArrayBind(_chromaticAberrationVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gyVidVertexBatchFlush();
    gyVidSetDefaultShaderProgram();
    gyVidVertexArrayBind(0);

}
