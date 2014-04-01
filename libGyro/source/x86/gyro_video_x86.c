#include <gyro_video_api.h>
#include <gyro_system_api.h>
#include "../glal/gyro_video_glal.h"
#include <glal/gyro_glal.h>

//Global SDL shit--shared with input (which has to resize window)
GYVidInitParams vidParams;
int vidBpp;

int gyVidInit(const GYVidInitParams *const params) {
	int flags = 0;
    unsigned int viewWidth=0, viewHeight=0, projWidth=0, projHeight=0;
    int videoInit;
    GLFWwindow *window;

	memcpy(&vidParams, params, sizeof(GYVidInitParams));

	switch(params->res) {
	case GY_VID_320x240:
		viewWidth = projWidth = 320;
		viewHeight = projHeight = 240;
		break;
	case GY_VID_640x480:
		viewWidth = projWidth = 640;
		viewHeight = projHeight = 480;
		break;
	case GY_VID_1280x720:
		viewWidth = projWidth = 1280;
		viewHeight = projHeight = 720;
		break;
	}

    glfwWindowHint(GLFW_RED_BITS, 5);
    glfwWindowHint(GLFW_GREEN_BITS, 5);
    glfwWindowHint(GLFW_BLUE_BITS, 5);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_RESIZABLE, (params->prop & GY_WIN_RESIZABLE) ? 1 : 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(viewWidth, viewHeight, params->title, (params->prop & GY_WIN_FULLSCREEN) ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (window == NULL) {
        _gyLog(GY_DEBUG_CRITICAL, "Video mode set failed\n");
    }

    glfwMakeContextCurrent(window);

    glewExperimental = 1;
    glewInit();

    /* vsync */
    glfwSwapInterval(1);

	gyVidOglInit(viewWidth, viewHeight, projWidth, projHeight);
	return 1;
}

int gyVidUninit(void) {
	return 1;
}

void gyVidSceneBegin(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	polyCount = 0;
}

void gyVidSceneEnd(void) {
    glfwSwapBuffers(glfwGetCurrentContext());
}
