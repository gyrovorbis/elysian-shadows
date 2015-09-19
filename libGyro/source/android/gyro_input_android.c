#include <stdlib.h>
#include <sys/types.h>

#include <android/keycodes.h>

#include <gyro_input_api.h>
#include <gyro_system_api.h>
#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_shader_api.h>
#include <glal/gyro_gl.h>

#include "internal_android.h"

int gyInputInit(const struct GYInputInitParams*const params) {}

static inline void _SetKeyDown(GYInput *const input, const GY_ANDROID_KEY key) {
	if(!input->keyDown[key]) input->keyTapped[key] = 1;
    input->keyDown[key] = 1;
}

static inline void _SetKeyUp(GYInput *const input, const GY_ANDROID_KEY key) {
    input->keyDown[key] = 0;
    input->keyTapped[key] = 0;
}

static inline GY_ANDROID_KEY _TranslateFromAndroid(const int keyCode) {
    switch(keyCode) {
    /* GY_KEY_ESC has no mapping */
    case AKEYCODE_DPAD_UP:
        return GY_KEY_DPAD_U;
    case AKEYCODE_DPAD_DOWN:
        return GY_KEY_DPAD_D;
    case AKEYCODE_DPAD_LEFT:
        return GY_KEY_DPAD_L;
    case AKEYCODE_DPAD_RIGHT:
        return GY_KEY_DPAD_R;
    case AKEYCODE_A:
        return GY_KEY_A;
    case AKEYCODE_B:
        return GY_KEY_B;
    case AKEYCODE_C:
        return GY_KEY_C;
    case AKEYCODE_D:
        return GY_KEY_D;
    case AKEYCODE_E:
        return GY_KEY_E;
    case AKEYCODE_F:
        return GY_KEY_F;
    case AKEYCODE_G:
        return GY_KEY_G;
    case AKEYCODE_H:
        return GY_KEY_H;
    case AKEYCODE_I:
        return GY_KEY_I;
    case AKEYCODE_J:
        return GY_KEY_J;
    case AKEYCODE_K:
        return GY_KEY_K;
    case AKEYCODE_L:
        return GY_KEY_L;
    case AKEYCODE_M:
        return GY_KEY_M;
    case AKEYCODE_N:
        return GY_KEY_N;
    case AKEYCODE_O:
        return GY_KEY_O;
    case AKEYCODE_P:
        return GY_KEY_P;
    case AKEYCODE_Q:
        return GY_KEY_Q;
    case AKEYCODE_R:
        return GY_KEY_R;
    case AKEYCODE_S:
        return GY_KEY_S;
    case AKEYCODE_T:
        return GY_KEY_T;
    case AKEYCODE_U:
        return GY_KEY_U;
    case AKEYCODE_V:
        return GY_KEY_V;
    case AKEYCODE_W:
        return GY_KEY_W;
    case AKEYCODE_X:
        return GY_KEY_X;
    case AKEYCODE_Y:
        return GY_KEY_Y;
    case AKEYCODE_Z:
        return GY_KEY_Z;
	case AKEYCODE_0:
		return GY_KEY_0;
	case AKEYCODE_1:
		return GY_KEY_1;
	case AKEYCODE_2:
		return GY_KEY_2;
	case AKEYCODE_3:
		return GY_KEY_3;
	case AKEYCODE_4:
		return GY_KEY_4;
	case AKEYCODE_5:
		return GY_KEY_5;
	case AKEYCODE_6:
		return GY_KEY_6;
	case AKEYCODE_7:
		return GY_KEY_7;
	case AKEYCODE_8:
		return GY_KEY_8;
	case AKEYCODE_9:
		return GY_KEY_9;
	case AKEYCODE_SPACE:
		return GY_KEY_SPACE;
	case AKEYCODE_ENTER:
		return GY_KEY_RETURN;
	case AKEYCODE_SHIFT_LEFT:
		return GY_KEY_LSHIFT;
	case AKEYCODE_SHIFT_RIGHT:
		return GY_KEY_RSHIFT;
    //case AKEYCODE_CTRL:
    //	return GY_KEY_LCTRL;
//	case AKEYCODE_CTRL_RIGHT:
//		return GY_KEY_RCTRL;
	case AKEYCODE_ALT_LEFT:
		return GY_KEY_LALT;
	case AKEYCODE_ALT_RIGHT:
		return GY_KEY_RALT;
	case AKEYCODE_GRAVE:
		return GY_KEY_BACKQUOTE;
	case AKEYCODE_BACKSLASH:
		return GY_KEY_BACKSLASH;
	case AKEYCODE_SLASH:
		return GY_KEY_FORWARDSLASH;
    default:
        return GY_KEY_NONE;
    }
}

static GYInput *_Input = NULL;

static int32_t __handle_input_event(struct android_app *app, AInputEvent *event)
{
	size_t pointer_count, i;
	int device_id, source, pointer_id;
	int action, type, code;
	GY_ANDROID_KEY retval;
	device_id = AInputEvent_getDeviceId(event);
	source = AInputEvent_getSource(event);
	type = AInputEvent_getType(event);
	if (type == AINPUT_EVENT_TYPE_MOTION) {
	} else if (type == AINPUT_EVENT_TYPE_KEY) {
		action = AKeyEvent_getAction(event);
		code = AKeyEvent_getKeyCode(event);
		retval = _TranslateFromAndroid(code);
		if (action == AKEY_EVENT_ACTION_UP) {
			if (retval > GY_KEY_NONE && retval < GY_KEY_LAST) {
				_SetKeyUp(_Input, retval);
			}
		} else if (action == AKEY_EVENT_ACTION_DOWN) {
			if (retval > GY_KEY_NONE && retval < GY_KEY_LAST) {
				_SetKeyDown(_Input, retval);
			}
		}
	}
	return 0;
}
#if 0
void  gyInputPoll(struct GYInput** const input)
{
	gy_android_app->onInputEvent = __handle_input_event;
    return 1;
}
#endif

int gyInputUninit(void) {
    return 1;
}

void gyMousePoll(GYMouse*const mouse) {}

void gyInputPoll(GYInput **const input) {
#if 0
	int ident;
	int events;
	struct android_poll_source *source;
    unsigned int i;

    //keyTap is only true if it was pressed last frame, so prime it
    for (i = 0; i < GY_KEY_LAST; ++i)
        input->keyTapped[i] = 0;

    _Input = input;

	while ((ident = ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) {
		if (source != NULL) {
			source->process(gy_android_app, source);
		}
		if (gy_android_app->destroyRequested != 0) {
			exit(0);
			break;
		}
	}

    _Input = NULL;
#endif
}

