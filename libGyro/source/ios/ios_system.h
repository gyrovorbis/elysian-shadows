#ifndef LIBGYRO_IOS_SYSTEM_INCLUDES_H
#define LIBGYRO_IOS_SYSTEM_INCLUDES_H

//total hack to check if we're objc (didn't work, but I dereested--FG)
#ifdef __OBJC__
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
	#import <UIKit/UIKit.h>
	#import <OpenGLES/EAGL.h>
	#import <QuartzCore/QuartzCore.h>
	#import <OpenGLES/EAGLDrawable.h>

#else
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
	//#include <OpenGLES/EAGL.h> C IS SHITTING ITS PANTS!!!
	//#include <OpenGLES/EAGLDrawable.h>
#endif

#endif //LIBGYRO_IOS_SYSTEM_INCLUDES_H
