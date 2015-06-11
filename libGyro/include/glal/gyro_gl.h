#ifndef GYRO_GL_H
#define GYRO_GL_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

//this is no good
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
#	include "ios/ios_system.h"
#	include "ios/gyro_gles_ios.h"
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	ifdef __APPLE__
//#       include <gl.h>
#       include <gl3.h>
#       include <gl3ext.h>
#define __gl_h_

#	else
//#       include <windows.h>
//#if !defined(__gl_h_) && !defined(__GL_H__) && !defined(__X_GL_H)
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

