#include <gyro_video_api.h>
#include <gyro_system_api.h>
#include "../glal/gyro_video_glal.h"
#include "internal_android.h"
#include <glal/gyro_glal.h>

GYVidInitParams vidParams;
int vidBpp;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;

int gyVidInit(const GYVidInitParams *const params) {
	int videoInit;
	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_NONE
	};
	EGLint attrib_list[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint num_configs;
	EGLConfig config;
	int error;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, 0, 0);
	eglChooseConfig(display, attribs, &config, 1, &num_configs);
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(gy_android_app->window, 0, 0, format);
	surface = eglCreateWindowSurface(display, config, gy_android_app->window, NULL);
	context = eglCreateContext(display, config, NULL, attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		_gyLog(GY_DEBUG_FATAL, "Unable to eglMakeCurrent\n");
		return 0;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    gyVidOglInit(w, h, w, h);
    return 1;
}

int gyVidUninit(void) {
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
	display = EGL_NO_DISPLAY;
	context = EGL_NO_CONTEXT;
	surface = EGL_NO_SURFACE;
	return 1;
}

void gyVidSceneBegin(void) {
    //glClearColor(0.5f, 0.75f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    polyCount = 0;
}

void gyVidSceneEnd(void) {
    eglSwapBuffers(display, surface);
}
