#include <libGyro/gyro_input_api.h>
#include <libGyro/gyro_system_api.h>
#include <sdl/gyro_input_sdl.inl>

int gyControllerPoll(const unsigned slot, struct GYController** const input) {
  return 0;
}

int gyInputPollEvents(void) {
  return 0;
}


extern "C" int  _sdlControllerInit(const struct GYInputInitParams*const  params);
extern "C" int  _sdlControllerUninit(void);
extern "C" void _sdlControllerDump(void);

GYInputInitParams    _initParams;
static bool         _keysChangedThisFrame[GY_KEY_COUNT]                 = { false };
static bool         _mouseButtonsChangedThisFrame[GY_MOUSE_BUT_COUNT]   = { false };
static bool         _mousePosChangedThisFrame                           = false;
static bool         _mouseScrollWheelChangedThisFrame                   = false;

// static inline GY_KEY _TranslateKeyFromQt(const int keyCode) {
//   return GY_KEY_NONE;
// }
//
// static inline GY_MOUSE_BUTTON _TranslateMouseButtonFromQt(Qt::MouseButton button) {
//   return GY_MOUSE_BUT_NONE;
// }

static inline void _setKeyDown(GYKeyboard *const keybd, const GY_KEY key) {
    unsigned newState = keybd->keys[key] | 0x1;
    keybd->keys[key] = (GY_BUTTON_STATE)newState;
    _keysChangedThisFrame[key] = true;
}

static inline void _setKeyUp(GYKeyboard *const keybd, const GY_KEY key) {
    //keybd->keys[key] = (GY_BUTTON_STATE)(((keybd->keys[key] << 0x1)) & 0x11);
    unsigned newState = (keybd->keys[key] & (~0x1));
    keybd->keys[key] = (GY_BUTTON_STATE)newState;
    _keysChangedThisFrame[key] = true;
}

static inline void _setMouseButtonDown(GYMouse *const mouse, const GY_MOUSE_BUTTON button) {
    unsigned newState = mouse->buttons[button] | 0x1;
    mouse->buttons[button] = (GY_BUTTON_STATE)newState;
    _mouseButtonsChangedThisFrame[button] = true;
}

static inline void _setMouseButtonUp(GYMouse *const mouse, const GY_MOUSE_BUTTON button) {
    unsigned newState = (mouse->buttons[button] & (~0x1));
    mouse->buttons[button] = (GY_BUTTON_STATE)newState;
    _mouseButtonsChangedThisFrame[button] = true;
}

extern "C" int gyInputInit(const GYInputInitParams*const params) {
    return 0;
}

extern "C" int gyInputUninit(void) {
    return 0;
}

extern "C" int gyKeyboardPoll(const unsigned slot, GYKeyboard**const keybd) {
    return 0;
}

extern "C" int gyMousePoll(const unsigned slot, GYMouse**const mouse) {
    return 0;
}

extern "C" void gyInputDeviceDump(void) {
    _gyLog(GY_DEBUG_VERBOSE, "LibGyro Device Dump");

    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "Keyboard: ATTACHED [Web]");
    _gyLog(GY_DEBUG_VERBOSE, "Mouse: ATTACHED [Web]");

    _sdlControllerDump();
    _gyPop(1);


}
