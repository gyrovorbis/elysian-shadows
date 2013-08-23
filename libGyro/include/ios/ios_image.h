#ifndef LIBGYRO_IOS_IMAGE_H
#define LIBGYRO_IOS_IMAGE_H

#include <OpenGLES/ES1/gl.h>

GLuint create_texture_png(const char *p, GLenum wrap, GLenum mag, GLenum min, unsigned *const success);

#endif
