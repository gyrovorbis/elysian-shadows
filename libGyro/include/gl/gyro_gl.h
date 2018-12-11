#ifndef GYRO_GL_H
#define GYRO_GL_H

#include <libGyro/gyro_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

//this is no good
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#   ifdef __OBJC__
    #import <OpenGLES/ES3/gl.h>
    #import <OpenGLES/ES3/glext.h>
    #import <UIKit/UIKit.h>
    #import <OpenGLES/EAGL.h>
    #import <QuartzCore/QuartzCore.h>
    #import <OpenGLES/EAGLDrawable.h>

#   else


    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#   endif



#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>

#elif GYRO_PLATFORM == GYRO_PLATFORM_RASPI
#   include <EGL/egl.h>
#   include <GLES3/gl3.h>
#   include <GLES3/gl3ext.h>

#elif GYRO_PLATFORM == GYRO_PLATFORM_WEB

#ifdef __WIN32
#       include <windows.h>
#endif

#       include <GL/glew.h>
#define QT_NO_OPENGL_ES_2

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	ifdef __APPLE__
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
//#       include <gl.h>
#       include <gl3.h>
#       include <gl3ext.h>
#define __gl_h_

#	else
//#       include <windows.h>
//#if !defined(__gl_h_) && !defined(__GL_H__) && !defined(__X_GL_H)
#ifdef __WIN32
#       include <windows.h>
#endif

#       include <GL/glew.h>
#define QT_NO_OPENGL_ES_2
//#       include <GL/gl.h>
//#       include <GL/glu.h>
//#endif
#      // define QOPENGL_H //THIS WILL REEST GLEW OTHERWISE!!!
#	endif
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_GL_H
