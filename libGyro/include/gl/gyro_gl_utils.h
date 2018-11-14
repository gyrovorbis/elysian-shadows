#ifndef GYRO_GL_UTILS_H
#define GYRO_GL_UTILS_H

#ifdef __cplusplus
#extern "C" {
#endif

#include <gl/gyro_gl.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_defines.h>
#include <stdio.h>

#define GY_GL_CHECK_ERROR() \
    (glalCheckError(__FILE__, __LINE__, __func__))


void glalGLDriverQuery(void);

static inline const char* glalErrorStr(GLenum err) {
    static char buff[15];

    switch(err) {
        GYRO_SWITCH_CASE_STRING(GL_NO_ERROR);
        GYRO_SWITCH_CASE_STRING(GL_INVALID_ENUM);
        GYRO_SWITCH_CASE_STRING(GL_INVALID_VALUE);
        GYRO_SWITCH_CASE_STRING(GL_INVALID_OPERATION);
        GYRO_SWITCH_CASE_STRING(GL_OUT_OF_MEMORY);
        GYRO_SWITCH_CASE_STRING(GL_INVALID_FRAMEBUFFER_OPERATION);
        default:
            sprintf(buff, "Unknown: %d", err);
            return buff;
    }

}

static inline int glalCheckError(const char* file, int line, const char* function) {
    int wasError = 0;
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        wasError = 1;
        _gyLog(GY_DEBUG_ERROR, "OpenGL Error: %s [%s:%d (%s)]", glalErrorStr(err), file, line, function);
    }
    return wasError;
}


#ifdef __cplusplus
}
#endif

#endif // GYRO_GL_UTILS_H
