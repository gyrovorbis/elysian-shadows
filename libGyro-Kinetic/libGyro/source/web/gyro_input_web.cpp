#include <libGyro/gyro_input_api.h>
#include <libGyro/gyro_system_api.h>
#include <sdl/gyro_input_sdl.inl>
#include <SDL2/SDL.h>


// int gyInputPollEvents(void) {
//   SDL_Event e;
//   while (SDL_PollEvent(&e)) {
//     _gyLog(GY_DEBUG_VERBOSE, "gyInputPollEvents%s\n", e.type);
//     // switch (e.type) {
//     // }
//   }
//   return 0;
// }


extern "C" int  _sdlControllerInit(const struct GYInputInitParams*const  params);
extern "C" int  _sdlControllerUninit(void);
extern "C" void _sdlControllerDump(void);

GYInputInitParams    _initParams;
static bool         _keysChangedThisFrame[GY_KEY_COUNT]                 = { false };
static bool         _mouseButtonsChangedThisFrame[GY_MOUSE_BUT_COUNT]   = { false };
static bool         _mousePosChangedThisFrame                           = false;
static bool         _mouseScrollWheelChangedThisFrame                   = false;

// static inline GY_KEY _TranslateKeyFromSDL(const int keyCode) {
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

static GYKeyboard*  _inputFilter = nullptr;

extern "C" int gyInputInit(const GYInputInitParams*const params) {
  _inputFilter = new GYKeyboard();
    return 0;
}

extern "C" int gyInputUninit(void) {
    return 0;
}



static inline GY_KEY _TranslateKeyFromSDL(const int keyCode) {
   //  switch(keyCode) {
   //  case Qt::Key_Escape:
   //      //exit(1);
   //      return GY_KEY_ESC;
   //  case Qt::Key_Up:
   //      return GY_KEY_DPAD_U;
   //  case Qt::Key_Down:
   //      return GY_KEY_DPAD_D;
   //  case Qt::Key_Left:
   //      return GY_KEY_DPAD_L;
   //  case Qt::Key_Right:
   //      return GY_KEY_DPAD_R;
   //  case Qt::Key_A:
   //      return GY_KEY_A;
   //  case Qt::Key_B:
   //      return GY_KEY_B;
   //  case Qt::Key_C:
   //      return GY_KEY_C;
   //  case Qt::Key_D:
   //      return GY_KEY_D;
   //  case Qt::Key_E:
   //      return GY_KEY_E;
   //  case Qt::Key_F:
   //      return GY_KEY_F;
   //  case Qt::Key_G:
   //      return GY_KEY_G;
   //  case Qt::Key_H:
   //      return GY_KEY_H;
   //  case Qt::Key_I:
   //      return GY_KEY_I;
   //  case Qt::Key_J:
   //      return GY_KEY_J;
   //  case Qt::Key_K:
   //      return GY_KEY_K;
   //  case Qt::Key_L:
   //      return GY_KEY_L;
   //  case Qt::Key_M:
   //      return GY_KEY_M;
   //  case Qt::Key_N:
   //      return GY_KEY_N;
   //  case Qt::Key_O:
   //      return GY_KEY_O;
   //  case Qt::Key_P:
   //      return GY_KEY_P;
   //  case Qt::Key_Q:
   //      return GY_KEY_Q;
   //  case Qt::Key_R:
   //      return GY_KEY_R;
   //  case Qt::Key_S:
   //      return GY_KEY_S;
   //  case Qt::Key_T:
   //      return GY_KEY_T;
   //  case Qt::Key_U:
   //      return GY_KEY_U;
   //  case Qt::Key_V:
   //      return GY_KEY_V;
   //  case Qt::Key_W:
   //      return GY_KEY_W;
   //  case Qt::Key_X:
   //      return GY_KEY_X;
   //  case Qt::Key_Y:
   //      return GY_KEY_Y;
   //  case Qt::Key_Z:
   //      return GY_KEY_Z;
   //  case Qt::Key_0:
   //      return GY_KEY_0;
   //  case Qt::Key_1:
   //      return GY_KEY_1;
   //  case Qt::Key_2:
   //      return GY_KEY_2;
   //  case Qt::Key_3:
   //      return GY_KEY_3;
   //  case Qt::Key_4:
   //      return GY_KEY_4;
   //  case Qt::Key_5:
   //      return GY_KEY_5;
   //  case Qt::Key_6:
   //      return GY_KEY_6;
   //  case Qt::Key_7:
   //      return GY_KEY_7;
   //  case Qt::Key_8:
   //      return GY_KEY_8;
   //  case Qt::Key_9:
   //      return GY_KEY_9;
   //  case Qt::Key_Space:
   //      return GY_KEY_SPACE;
   //  case Qt::Key_Return:
   //      return GY_KEY_RETURN;
   //  case Qt::Key_Shift:
   //      return GY_KEY_LSHIFT;
   //  //case Qt::Key_Shift:
   //  //    return GY_KEY_RSHIFT;
   //  case Qt::Key_Control:
   //      return GY_KEY_LCTRL;
   // // case Qt::Key_Control:
   //     // return GY_KEY_RCTRL;
   //  case Qt::Key_Alt:
   //      return GY_KEY_LALT;
   //  //case Qt::Key_Alt:
   //    //  return GY_KEY_RALT;
   //  case Qt::Key_Back:
   //      return GY_KEY_BACKQUOTE;
   //  case Qt::Key_Backslash:
   //      return GY_KEY_BACKSLASH;
   //  case Qt::Key_Slash:
   //      return GY_KEY_FORWARDSLASH;
   //  default:
   //      return GY_KEY_NONE;
   //  }

  switch(keyCode) {
    case SDLK_a:
        return GY_KEY_A;
    case SDLK_b:
        return GY_KEY_B;
    case SDLK_c:
        return GY_KEY_C;
    case SDLK_d:
        return GY_KEY_D;
    case SDLK_e:
        return GY_KEY_E;
    case SDLK_f:
        return GY_KEY_F;
    case SDLK_g:
        return GY_KEY_G;
    case SDLK_h:
        return GY_KEY_H;
    case SDLK_i:
        return GY_KEY_I;
    case SDLK_j:
        return GY_KEY_J;
    case SDLK_k:
        return GY_KEY_K;
    case SDLK_l:
        return GY_KEY_L;
    case SDLK_m:
        return GY_KEY_M;
    case SDLK_n:
        return GY_KEY_N;
      case SDLK_UP:
             return GY_KEY_DPAD_U;
         case SDLK_DOWN:
             return GY_KEY_DPAD_D;
         case SDLK_LEFT:
             return GY_KEY_DPAD_L;
         case SDLK_RIGHT:
             return GY_KEY_DPAD_R;
  }
  return GY_KEY_LSHIFT;
}



extern "C" int gyKeyboardPoll(const unsigned slot, GYKeyboard**const keybd) {
    if(slot > 0) {
        *keybd = NULL;
        return 0;
    }



      SDL_Event e;
      // std::string key;
      while (SDL_PollEvent(&e)) {
        switch (e.type) {
          case SDL_KEYUP:
                  _gyLog(GY_DEBUG_VERBOSE, "SDL_KEYUP");
                  _setKeyUp(_inputFilter, _TranslateKeyFromSDL((&e.key)->keysym.sym));
            // key = (std::string)SDL_GetKeyName((&e.key)->keysym.sym);
            // std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            // callScriptMethod(this, "dispatch", "onKeyUp", key, (&e.key)->keysym.sym);
          break;

          case SDL_KEYDOWN:
                  _gyLog(GY_DEBUG_VERBOSE, "SDL_KEYDOWN");
              _setKeyDown(_inputFilter, _TranslateKeyFromSDL((&e.key)->keysym.sym));
            // key = (std::string)SDL_GetKeyName((&e.key)->keysym.sym);
            // std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            // callScriptMethod(this, "dispatch", "onKeyDown", key, (&e.key)->keysym.sym);
          break;
        }
      }




    *keybd = _inputFilter;
    (*keybd)->changed = 0;
    for(unsigned i = 0; i < GY_KEY_COUNT; ++i) {
        if(!_keysChangedThisFrame[i]) {
            unsigned wasDown = ((*keybd)->keys[i] & GY_BUT_FLAGS_IS_PRESSED);
            unsigned key = (*keybd)->keys[i];
            key <<= 1;
            key |= wasDown;
            key &= 0x03;
            (*keybd)->keys[i] = (GY_BUTTON_STATE)key;
        } else {
            _keysChangedThisFrame[i] = false;
            (*keybd)->changed = 1;
        }
    }

    return 1;
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
