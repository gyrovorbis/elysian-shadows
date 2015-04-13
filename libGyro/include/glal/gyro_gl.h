#ifndef GYRO_GL_H
#define GYRO_GL_H

#include "gyro_platform.h"

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
#       include <gl3.h>
#       include <gl3ext.h>
#define __gl_h_

#	else
#       include <windows.h>
#       include <GL/glew.h>
//#       include <GL/gl.h>
//#       include <GL/glu.h>
#	endif
#endif

#endif // GYRO_GL_H

