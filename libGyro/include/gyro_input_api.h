#ifndef GYRO_INPUT_H
#define GYRO_INPUT_H

#include "gyro_platform.h"
#include "gyro_matrix_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_INPUT_CONTROLLER_ANALOG_MIN    0.3f
#define GYRO_INPUT_CONTROLLER_ANALOG_MAX    0.7f

struct GYInputInitParams;

typedef enum GY_BUTTON_FLAGS {
    GY_BUT_FLAGS_IS_PRESSED  = 0x01,
    GY_BUT_FLAGS_WAS_PRESSED = 0x02,
    GY_BUT_FLAGS_ALL         = 0x03
} GY_BUTTON_FLAGS;

typedef enum GY_BUTTON_STATE {
    GY_BUT_STATE_UP        = 0x00,
    GY_BUT_STATE_TAPPED    = 0x01,
    GY_BUT_STATE_RELEASED  = 0x02,
    GY_BUT_STATE_DOWN      = 0x03,
} GY_BUTTON_STATE;

//Two goddamn layers of translation? Christ.
typedef enum GY_KEY {
    GY_KEY_NONE, GY_KEY_A, GY_KEY_B, GY_KEY_C, GY_KEY_D, GY_KEY_E, GY_KEY_F, GY_KEY_G, GY_KEY_H, GY_KEY_I, GY_KEY_J, GY_KEY_K, GY_KEY_L, GY_KEY_M, GY_KEY_N, GY_KEY_O, GY_KEY_P,
    GY_KEY_Q, GY_KEY_R, GY_KEY_S, GY_KEY_T, GY_KEY_U, GY_KEY_V, GY_KEY_W, GY_KEY_X, GY_KEY_Y, GY_KEY_Z,
    GY_KEY_DPAD_U, GY_KEY_DPAD_D, GY_KEY_DPAD_L, GY_KEY_DPAD_R,
    GY_KEY_0, GY_KEY_1, GY_KEY_2, GY_KEY_3, GY_KEY_4, GY_KEY_5, GY_KEY_6, GY_KEY_7, GY_KEY_8, GY_KEY_9,
    GY_KEY_SPACE, GY_KEY_RETURN, GY_KEY_ESC,
    GY_KEY_BACKSLASH, GY_KEY_FORWARDSLASH,
    GY_KEY_LSHIFT, GY_KEY_RSHIFT, GY_KEY_LCTRL, GY_KEY_RCTRL, GY_KEY_RALT, GY_KEY_LALT, GY_KEY_BACKQUOTE,
    GY_KEY_COUNT
} GY_KEY;

typedef struct GYKeyboard {
    unsigned    keys[GY_KEY_COUNT];
    char        changed;
} GYKeyboard;

typedef enum GY_MOUSE_BUTTON {
    GY_MOUSE_BUT_NONE,
    GY_MOUSE_BUT_LEFT,
    GY_MOUSE_BUT_MIDDLE,
    GY_MOUSE_BUT_RIGHT,
    GY_MOUSE_BUT_BACK,
    GY_MOUSE_BUT_COUNT
} GY_MOUSE_BUTTON;

typedef struct GYMouse {
    unsigned        buttons[GY_MOUSE_BUT_COUNT];
    GYVector2       scrollWheel;
    GYVector2       screenPos;
    char            buttonsChanged;
    char            scrollWheelChanged;
    char            screenPosChanged;
} GYMouse;

typedef enum GY_CONTROLLER_ANALOG {
    GY_CONTROLLER_ANALOG_LEFT,
    GY_CONTROLLER_ANALOG_RIGHT,
    GY_CONTROLLER_ANALOG_COUNT
} GY_CONTROLLER_ANALOG;

typedef enum GY_CONTROLLER_TRIGGER {
    GY_CONTROLLER_TRIGGER_LEFT,
    GY_CONTROLLER_TRIGGER_RIGHT,
    GY_CONTROLLER_TRIGGER_COUNT
} GY_CONTROLLER_TRIGGER;

typedef enum GY_CONTROLLER_BUTTON {
    GY_CONTROLLER_BUT_DPAD_UP,
    GY_CONTROLLER_BUT_DPAD_DOWN,
    GY_CONTROLLER_BUT_DPAD_LEFT,
    GY_CONTROLLER_BUT_DPAD_RIGHT,
    GY_CONTROLLER_BUT_ANALOG1_UP,
    GY_CONTROLLER_BUT_ANALOG1_DOWN,
    GY_CONTROLLER_BUT_ANALOG1_LEFT,
    GY_CONTROLLER_BUT_ANALOG1_RIGHT,
    GY_CONTROLLER_BUT_ANALOG2_UP,
    GY_CONTROLLER_BUT_ANALOG2_DOWN,
    GY_CONTROLLER_BUT_ANALOG2_LEFT,
    GY_CONTROLLER_BUT_ANALOG2_RIGHT,
    GY_CONTROLLER_BUT_START,
    GY_CONTROLLER_BUT_SELECT,
    GY_CONTROLLER_BUT_HOME,
    GY_CONTROLLER_BUT_LSHOULDER,
    GY_CONTROLLER_BUT_RSHOULDER,
    GY_CONTROLLER_BUT_LANALOG,
    GY_CONTROLLER_BUT_RANALOG,
    GY_CONTROLLER_BUT_X,
    GY_CONTROLLER_BUT_SQUARE,
    GY_CONTROLLER_BUT_CIRCLE,
    GY_CONTROLLER_BUT_TRIANGLE,
    GY_CONTROLLER_BUT_COUNT
} GY_CONTROLLER_BUTTON;

typedef struct GYController {
    GYVector2        analogSticks[GY_CONTROLLER_ANALOG_COUNT];
    GYVector2        triggers    [GY_CONTROLLER_TRIGGER_COUNT];
    unsigned         buttons     [GY_CONTROLLER_BUT_COUNT];
} GYController;

int         gyInputInit(const struct GYInputInitParams*const params);
int         gyInputUninit(void);
int         gyInputPollEvents(void);

int         gyControllerPoll(const unsigned slot, struct GYController** const input);
int         gyKeyboardPoll(struct GYKeyboard** const keybd);
int         gyMousePoll(struct GYMouse**const mouse);

void        gyInputDeviceDump(void);

#ifdef GYRO_INPUT_PLATFORM_INCLUDE
#	include GYRO_INPUT_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
