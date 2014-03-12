#include <gyro_input_api.h>
#include <gyro_system_api.h>
#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_shader_api.h>
#include <x86/gyro_glfw_x86.inl>
#include "../glal/gyro_video_glal.h"

extern GYVidInitParams vidParams;
extern int vidBpp;

static inline void _SetKeyDown(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 1;
    input->keyTapped[key] = 1;
}

static inline void _SetKeyUp(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 0;
    input->keyTapped[key] = 0;
}

static inline GY_X86_KEY _TranslateFromGLFW(const int keyCode) {
    switch(keyCode) {
    case GLFW_KEY_ESCAPE:
        //exit(1);
        return GY_KEY_ESC;
    case GLFW_KEY_UP:
        return GY_KEY_DPAD_U;
    case GLFW_KEY_DOWN:
        return GY_KEY_DPAD_D;
    case GLFW_KEY_LEFT:
        return GY_KEY_DPAD_L;
    case GLFW_KEY_RIGHT:
        return GY_KEY_DPAD_R;
    case GLFW_KEY_A:
        return GY_KEY_A;
    case GLFW_KEY_B:
        return GY_KEY_B;
    case GLFW_KEY_C:
        return GY_KEY_C;
    case GLFW_KEY_D:
        return GY_KEY_D;
    case GLFW_KEY_E:
        return GY_KEY_E;
    case GLFW_KEY_F:
        return GY_KEY_F;
    case GLFW_KEY_G:
        return GY_KEY_G;
    case GLFW_KEY_H:
        return GY_KEY_H;
    case GLFW_KEY_I:
        return GY_KEY_I;
    case GLFW_KEY_J:
        return GY_KEY_J;
    case GLFW_KEY_K:
        return GY_KEY_K;
    case GLFW_KEY_L:
        return GY_KEY_L;
    case GLFW_KEY_M:
        return GY_KEY_M;
    case GLFW_KEY_N:
        return GY_KEY_N;
    case GLFW_KEY_O:
        return GY_KEY_O;
    case GLFW_KEY_P:
        return GY_KEY_P;
    case GLFW_KEY_Q:
        return GY_KEY_Q;
    case GLFW_KEY_R:
        return GY_KEY_R;
    case GLFW_KEY_S:
        return GY_KEY_S;
    case GLFW_KEY_T:
        return GY_KEY_T;
    case GLFW_KEY_U:
        return GY_KEY_U;
    case GLFW_KEY_V:
        return GY_KEY_V;
    case GLFW_KEY_W:
        return GY_KEY_W;
    case GLFW_KEY_X:
        return GY_KEY_X;
    case GLFW_KEY_Y:
        return GY_KEY_Y;
    case GLFW_KEY_Z:
        return GY_KEY_Z;
	case GLFW_KEY_0:
		return GY_KEY_0;
	case GLFW_KEY_1:
		return GY_KEY_1;
	case GLFW_KEY_2:
		return GY_KEY_2;
	case GLFW_KEY_3:
		return GY_KEY_3;
	case GLFW_KEY_4:
		return GY_KEY_4;
	case GLFW_KEY_5:
		return GY_KEY_5;
	case GLFW_KEY_6:
		return GY_KEY_6;
	case GLFW_KEY_7:
		return GY_KEY_7;
	case GLFW_KEY_8:
		return GY_KEY_8;
	case GLFW_KEY_9:
		return GY_KEY_9;
	case GLFW_KEY_SPACE:
		return GY_KEY_SPACE;
	case GLFW_KEY_ENTER:
		return GY_KEY_RETURN;
	case GLFW_KEY_LEFT_SHIFT:
		return GY_KEY_LSHIFT;
	case GLFW_KEY_RIGHT_SHIFT:
		return GY_KEY_RSHIFT;
	case GLFW_KEY_LEFT_CONTROL:
		return GY_KEY_LCTRL;
	case GLFW_KEY_RIGHT_CONTROL:
		return GY_KEY_RCTRL;
	case GLFW_KEY_LEFT_ALT:
		return GY_KEY_LALT;
	case GLFW_KEY_RIGHT_ALT:
		return GY_KEY_RALT;
	case GLFW_KEY_GRAVE_ACCENT:
		return GY_KEY_BACKQUOTE;
	case GLFW_KEY_BACKSLASH:
		return GY_KEY_BACKSLASH;
	case GLFW_KEY_SLASH:
		return GY_KEY_FORWARDSLASH;  
    default:
        return GY_KEY_NONE;
    }
}

static GYInput *_Input = NULL;

static void _KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    GY_X86_KEY retval;
    if (_Input == NULL) {
        return;
    }
    retval = _TranslateFromGLFW(key);
    switch (action) {
        case GLFW_PRESS:
            if (retval > GY_KEY_NONE && retval < GY_KEY_LAST) {
                _SetKeyDown(_Input, retval);
            }
            break;
        case GLFW_RELEASE:
            if (retval > GY_KEY_NONE && retval < GY_KEY_LAST) {
                _SetKeyUp(_Input, retval);
            }
            break;
    }
}

static void _FramebufferResizeCallback(GLFWwindow *window, int width, int height) {
    unsigned int projWidth, projHeight;
    if (!(vidParams.prop & GY_WIN_STRETCH)) {
        if (vidParams.minWidth != -1 && width < vidParams.minWidth) {
            projWidth = vidParams.minWidth;
        } else if (vidParams.maxWidth != -1 && width > vidParams.maxWidth) {
            projWidth = vidParams.maxWidth;
        } else {
            projWidth = width;
        }
        if (vidParams.minHeight > -1 && height < vidParams.minHeight) {
            projHeight = vidParams.minHeight;
        } else if (vidParams.maxHeight > -1 && height > vidParams.maxHeight) {
            projHeight = vidParams.maxHeight;
        } else {
            projHeight = height;
        }
    } else {
        gyVidResolution(&projWidth, &projHeight);
    }
    gyShadersReloadAll();
    gyVidOglInit(width, height, projWidth, projHeight);
    gyVidTexReloadAll();
}

int gyInputInit(void) {
    GLFWwindow *context = glfwGetCurrentContext();
    if (context == NULL) {
        _gyLog(GY_DEBUG_FATAL, "\tCan't initialize the input subsystem without an active context!\n");
        return 0;
    }
    glfwSetKeyCallback(context, &_KeyCallback);
    glfwSetFramebufferSizeCallback(context, &_FramebufferResizeCallback);
    return 1;
}

int gyInputUninit(void) {
    return 1;
}

void gyInputPoll(GYInput *const input) {
    unsigned int i;

    //keyTap is only true if it was pressed last frame, so prime it
    for (i = 0; i < GY_KEY_LAST; ++i)
        input->keyTapped[i] = 0;

    _Input = input;

    glfwPollEvents();
    if (glfwWindowShouldClose(glfwGetCurrentContext())) {
        glfwTerminate();
        exit(0);
    }

    _Input = NULL;
}

