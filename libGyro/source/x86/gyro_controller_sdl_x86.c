#include <gyro_platform.h>
#include <x86/gyro_sdl_input.h>
#include <gyro_system_api.h>
#include <gyro_input_api.h>

#define GYRO_CONTROLLER_SDL_X86_GUID_SIZE    1024

//External symbol defined in SDL_Haptic API!
extern int _gyRumbleSDLTryOpenHapticJoystick(SDL_Joystick* joystick, int port);
extern int _gyRumbleSDLTryCloseHapticJoystick(SDL_Joystick* joystick, int port);

typedef struct SDLControllerProfile {
    char    GUID[GYRO_CONTROLLER_SDL_X86_GUID_SIZE];
} SDLControllerProfile;

typedef struct SDLController {
    GYController            cont;
    SDL_GameController*     sdlController;
    SDL_Joystick*           sdlJoystick;
    SDL_JoystickID          sdlJoystickInstanceId;
    int                     deviceIndex;
    int                     port;
} SDLController;

extern GYInputInitParams   _initParams;
static SDLController*      _devices        = NULL;

#if SDL_PATCHLEVEL >= 5
inline static const char* _sdlJoystickPowerLevelString(const SDL_JoystickPowerLevel level) {
    switch(level) {
    default:
    case SDL_JOYSTICK_POWER_UNKNOWN:    return "UNKNOWN";
    case SDL_JOYSTICK_POWER_EMPTY:      return "EMPTY";
    case SDL_JOYSTICK_POWER_LOW:        return "LOW";
    case SDL_JOYSTICK_POWER_MEDIUM:     return "MEDIUM";
    case SDL_JOYSTICK_POWER_FULL:       return "FULL";
    case SDL_JOYSTICK_POWER_WIRED:      return "WIRED";
    case SDL_JOYSTICK_POWER_MAX:        return "MAX";
    }
}
#endif

inline static SDLController* _findControllerByInstanceID(SDL_JoystickID id) {
    for(unsigned i = 0; i < _initParams.controllerCount; ++i) {
        if(_devices[i].sdlJoystickInstanceId == id) return &_devices[i];
    }
    return NULL;
}

inline static SDLController* _claimFreeController(void) {
    for(unsigned i = 0; i < _initParams.controllerCount; ++i) {
        if(!_devices[i].sdlJoystick) return &_devices[i];
    }
    return NULL;
}

int _sdlControllerInit(const struct GYInputInitParams*const  params) {
    int success = 1;
    SDL_version version;
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_INPUT_API]: Initializing SDL_GAMECONTROLLER and SDL_HAPTIC.");
    _gyPush();
    if(SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        success = 0;
        _gyLog(GY_DEBUG_ERROR, "Could not initialize SDL_GAMECONTROLLER! [%s]", SDL_GetError());
    }
    _gyLog(GY_DEBUG_VERBOSE, "Max requested controller ports: %d", params->controllerCount);


    _gyLog(GY_DEBUG_VERBOSE, "SDL Library Versions");
    _gyPush();
    SDL_VERSION(&version);
    _gyLog(GY_DEBUG_VERBOSE, "%-15s: %10d.%d.%d", "Compile-Time",  version.major, version.minor, version.patch);
    SDL_GetVersion(&version);
    _gyLog(GY_DEBUG_VERBOSE, "%-15s: %10d.%d.%d", "Run-time",  version.major, version.minor, version.patch);
    _gyPop(1);

    _devices = malloc(sizeof(SDLController)*params->controllerCount);
    memset(_devices, 0, sizeof(SDLController)*params->controllerCount);
    for(unsigned i = 0; i < params->controllerCount; ++i) {
        _devices[i].sdlJoystickInstanceId = -1;
        _devices[i].port = i;
        _devices[i].deviceIndex = -1;
    }

    _gyPop(1);

    return success;
}

int _sdlControllerUninit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_INPUT_API]: Uninitializing SDL_GAMECONTROLLER and SDL_HAPTIC");

    free(_devices);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);

    return 1;
}

static int _sdlOpenController(const int deviceIndex, SDLController*const cont) {

    cont->sdlJoystick   = SDL_JoystickOpen(deviceIndex);

    if(SDL_IsGameController(deviceIndex)) {
        cont->sdlController = SDL_GameControllerOpen(deviceIndex);
    }
    //cont->sdlJoystick   = SDL_GameControllerGetJoystick(cont->sdlController);
    cont->sdlJoystickInstanceId = SDL_JoystickInstanceID(cont->sdlJoystick);
    cont->deviceIndex = deviceIndex;

    _gyRumbleSDLTryOpenHapticJoystick(cont->sdlJoystick, cont->port);

    return 1;

}

static int _sdlCloseController(SDLController *const cont) {
    _gyRumbleSDLTryCloseHapticJoystick(cont->sdlJoystick, cont->port);
    if(cont->sdlController) SDL_GameControllerClose(cont->sdlController);
    SDL_JoystickClose(cont->sdlJoystick);

    cont->sdlController             = NULL;
    cont->sdlJoystick               = NULL;
    cont->sdlJoystickInstanceId     = -1;
    cont->deviceIndex               = -1;

    return 1;
}

static void _sdlPrintControllerStats(SDLController*const cont) {
    char guidStr[GYRO_CONTROLLER_SDL_X86_GUID_SIZE];

    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(cont->sdlJoystick), guidStr, sizeof(guidStr));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Device Type:", cont->sdlController? "SDL_GAMECONTROLLER" : "SDL_JOYSTICK");
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Name", SDL_JoystickName(cont->sdlJoystick));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "GUID", guidStr);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Device Index", cont->deviceIndex);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Instance ID", cont->sdlJoystickInstanceId);
#if SDL_PATCHLEVEL >= 5
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Power Level",
           _sdlJoystickPowerLevelString(SDL_JoystickCurrentPowerLevel(cont->sdlJoystick)));
#endif
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Number of Axes", SDL_JoystickNumAxes(cont->sdlJoystick));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Number of Buttons", SDL_JoystickNumButtons(cont->sdlJoystick));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Number of Hats", SDL_JoystickNumHats(cont->sdlJoystick));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Number of Balls", SDL_JoystickNumBalls(cont->sdlJoystick));
#if 0
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Haptic Support", cont->sdlHaptic? "TRUE" : "FALSE");

    if(cont->sdlHaptic) {
        _gyPush();
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40d", "Haptic Effects", SDL_HapticNumEffects(cont->sdlHaptic));
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40x", "Haptic Query", SDL_HapticQuery(cont->sdlHaptic));
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Haptic Rumble Support", SDL_HapticRumbleSupported(cont->sdlHaptic)? "TRUE" : "FALSE");
        _gyPop(1);
    }
#endif

}

void _sdlControllerDump(void) {

    _gyLog(GY_DEBUG_VERBOSE, "Controllers and Joysticks [SDL]");
    //_gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Controllers", SDL_Num);
    _gyLog(GY_DEBUG_VERBOSE, "Total Joystick Count: %d", SDL_NumJoysticks());

    _gyPush();

    for(unsigned i = 0; i < _initParams.controllerCount; ++i) {
        if(_devices[i].sdlJoystick) {
            _gyLog(GY_DEBUG_VERBOSE, "Port %d: [%s]", i, _devices[i].sdlController? "SDL_CONTROLLER" : "SDL_JOYSTICK");
            _gyPush();
            _sdlPrintControllerStats(&_devices[i]);
            _gyPop(1);
        } else {
            _gyLog(GY_DEBUG_VERBOSE, "Port %d: [Disconnected]", i);
        }
    }

    _gyPop(1);

}

static void _sdlAttachController(const int deviceIndex) {
    SDLController tempController;
    SDLController *cont = _claimFreeController();

    if(!cont) {
        cont = &tempController;
        tempController.port = -1;
    }

    if(cont != &tempController)
        _gyLog(GY_DEBUG_VERBOSE, "SDL Joystick Attached! [Port: %d]", deviceIndex);
    else
        _gyLog(GY_DEBUG_WARNING, "SDL Joystick Detected! [Port: %d (UNALLOCATED)]", deviceIndex);
    _gyPush();

    _sdlOpenController(deviceIndex, cont);
    _sdlPrintControllerStats(cont);

    //Close controller if it was a temporary opened on an unallocated port
    if(cont == &tempController) {
        _sdlCloseController(cont);
    }

    _gyPop(1);
}

static void _sdlRemoveController(const SDL_JoystickID instanceID) {
    SDLController*const cont = _findControllerByInstanceID(instanceID);
    if(!cont || !cont->sdlJoystick) return;
    _gyLog(GY_DEBUG_VERBOSE, "SDL Joystick Disconnected! [Port: %d]", cont->port);
    _sdlCloseController(cont);
}

static void _sdlControllerEvent(const SDL_JoystickID instanceID, const SDL_Event* event) {
    SDLController* cont = _findControllerByInstanceID(instanceID);
    if(!cont) return;
    enum GY_CONTROLLER_BUTTON gyButton;
    float analogVal;
    int state;

    switch(event->type) {
    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
        if(cont->sdlController) return; //process buttons from Controller events, not joystick!!!
        state = event->type == SDL_JOYBUTTONDOWN? 1 : 0;

        switch(event->jbutton.button) {
        case 12: gyButton        = GY_CONTROLLER_BUT_DPAD_DOWN;  break;
        case 11: gyButton        = GY_CONTROLLER_BUT_DPAD_UP;    break;
        case 13: gyButton        = GY_CONTROLLER_BUT_DPAD_LEFT;  break;
        case 14: gyButton        = GY_CONTROLLER_BUT_DPAD_RIGHT; break;
        case 8:  gyButton        = GY_CONTROLLER_BUT_START;      break;
        case 9:  gyButton        = GY_CONTROLLER_BUT_SELECT;     break;
        case 10: gyButton        = GY_CONTROLLER_BUT_HOME;       break;
        case 0:  gyButton        = GY_CONTROLLER_BUT_X;          break;
        case 1:  gyButton        = GY_CONTROLLER_BUT_CIRCLE;     break;
        case 2:  gyButton        = GY_CONTROLLER_BUT_SQUARE;     break;
        case 3:  gyButton        = GY_CONTROLLER_BUT_TRIANGLE;   break;
        case 6:  gyButton        = GY_CONTROLLER_BUT_LANALOG;    break;
        case 7:  gyButton        = GY_CONTROLLER_BUT_RANALOG;    break;
        case 4:  gyButton        = GY_CONTROLLER_BUT_LSHOULDER;  break;
        case 5:  gyButton        = GY_CONTROLLER_BUT_RSHOULDER;  break;
        default:
            _gyLog(GY_DEBUG_VERBOSE, "GYRO_INPUT_API: Unidentified key - %d", state);
            return;
        }
        cont->cont.buttons[gyButton] &= ~GY_BUT_FLAGS_IS_PRESSED;
        cont->cont.buttons[gyButton] |= state;

        break;


    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        state = event->type == SDL_CONTROLLERBUTTONDOWN? 1 : 0;

        switch(event->cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:       gyButton        = GY_CONTROLLER_BUT_DPAD_DOWN;  break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:         gyButton        = GY_CONTROLLER_BUT_DPAD_UP;    break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:       gyButton        = GY_CONTROLLER_BUT_DPAD_LEFT;  break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:      gyButton        = GY_CONTROLLER_BUT_DPAD_RIGHT; break;
        case SDL_CONTROLLER_BUTTON_START:           gyButton        = GY_CONTROLLER_BUT_START;      break;
        case SDL_CONTROLLER_BUTTON_BACK:            gyButton        = GY_CONTROLLER_BUT_SELECT;     break;
        case SDL_CONTROLLER_BUTTON_GUIDE:           gyButton        = GY_CONTROLLER_BUT_HOME;       break;
        case SDL_CONTROLLER_BUTTON_A:               gyButton        = GY_CONTROLLER_BUT_X;          break;
        case SDL_CONTROLLER_BUTTON_B:               gyButton        = GY_CONTROLLER_BUT_CIRCLE;     break;
        case SDL_CONTROLLER_BUTTON_X:               gyButton        = GY_CONTROLLER_BUT_SQUARE;     break;
        case SDL_CONTROLLER_BUTTON_Y:               gyButton        = GY_CONTROLLER_BUT_TRIANGLE;   break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:       gyButton        = GY_CONTROLLER_BUT_LANALOG;    break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:      gyButton        = GY_CONTROLLER_BUT_RANALOG;    break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:    gyButton        = GY_CONTROLLER_BUT_LSHOULDER;  break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:   gyButton        = GY_CONTROLLER_BUT_RSHOULDER;  break;
        default:
            _gyLog(GY_DEBUG_VERBOSE, "GYRO_INPUT_API: Unidentified Controller Button - %d", state);
            return;
        }
        cont->cont.buttons[gyButton] &= ~GY_BUT_FLAGS_IS_PRESSED;
        cont->cont.buttons[gyButton] |= state;

        //_gyLog(GY_DEBUG_VERBOSE, "But[%d] -> %d", gyButton, cont->cont.buttons[gyButton]);

        break;

    case SDL_JOYAXISMOTION:
        analogVal = (float)event->jaxis.value/(float)INT16_MAX;
        switch(event->jaxis.axis) {
        case 0: cont->cont.analogSticks[GY_CONTROLLER_ANALOG_LEFT].x    = analogVal; break;
        case 1: cont->cont.analogSticks[GY_CONTROLLER_ANALOG_LEFT].y    = analogVal; break;
        case 2: cont->cont.analogSticks[GY_CONTROLLER_ANALOG_RIGHT].x   = analogVal; break;
        case 3: cont->cont.analogSticks[GY_CONTROLLER_ANALOG_RIGHT].x   = analogVal; break;
        default: break;
        }
        break;
    default: break;
    }

}

static void _sdlPollEvents(void) {
    SDL_Event event;

    while(SDL_PollEvent( &event ) != 0)
    {
        switch (event.type) {
        case SDL_JOYAXISMOTION:
            _sdlControllerEvent(event.jaxis.which, &event);
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
            _sdlControllerEvent(event.cbutton.which, &event);
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            _sdlControllerEvent(event.jbutton.which, &event);
            break;
        case SDL_JOYDEVICEADDED:
            _sdlAttachController(event.jdevice.which);
            break;
        case SDL_CONTROLLERDEVICEADDED:
          //  _sdlAttachController(event.cdevice.which);
            break;
        case SDL_JOYDEVICEREMOVED:

            _sdlRemoveController(event.jdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
          //  _sdlRemoveController(event.cdevice.which);
            break;
        }
    }

}

int gyControllerPoll(const unsigned slot, struct GYController** const input) {
    GYVector2 analog = { 0.0f, 0.0f };
    unsigned al[GY_CONTROLLER_ANALOG_COUNT] = { 0 };
    unsigned ar[GY_CONTROLLER_ANALOG_COUNT] = { 0 };
    unsigned ad[GY_CONTROLLER_ANALOG_COUNT] = { 0 };
    unsigned au[GY_CONTROLLER_ANALOG_COUNT] = { 0 };

    if(!_devices[slot].sdlJoystick) {
        _devices[slot].cont.analogSticks[GY_CONTROLLER_ANALOG_LEFT]     = analog;
        _devices[slot].cont.analogSticks[GY_CONTROLLER_ANALOG_RIGHT]    = analog;
    } else {

        for(unsigned a = 0; a < GY_CONTROLLER_ANALOG_COUNT; ++a) {
            if(_devices[slot].cont.analogSticks[a].x < -GYRO_INPUT_CONTROLLER_ANALOG_MIN)     al[a] = 1;
            else if(_devices[slot].cont.analogSticks[a].x > GYRO_INPUT_CONTROLLER_ANALOG_MIN) ar[a] = 1;
            if(_devices[slot].cont.analogSticks[a].y < -GYRO_INPUT_CONTROLLER_ANALOG_MIN)     au[a] = 1;
            else if(_devices[slot].cont.analogSticks[a].y > GYRO_INPUT_CONTROLLER_ANALOG_MIN) ad[a] = 1;

        }
    }

    for(unsigned i = 0; i < GY_CONTROLLER_BUT_COUNT; ++i) {
        int wasDown = (_devices[slot].cont.buttons[i] & GY_BUT_FLAGS_IS_PRESSED);
        _devices[slot].cont.buttons[i] <<= 1;
        _devices[slot].cont.buttons[i] |= wasDown;
        _devices[slot].cont.buttons[i] &= 0x03;
    }

    _sdlPollEvents();

    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_LEFT]     &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_LEFT]     |= al[GY_CONTROLLER_ANALOG_LEFT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_RIGHT]    &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_RIGHT]    |= ar[GY_CONTROLLER_ANALOG_LEFT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_UP]       &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_UP]       |= au[GY_CONTROLLER_ANALOG_LEFT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_DOWN]     &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG1_DOWN]     |= ad[GY_CONTROLLER_ANALOG_LEFT];

    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_LEFT]     &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_LEFT]     |= al[GY_CONTROLLER_ANALOG_RIGHT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_RIGHT]    &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_RIGHT]    |= ar[GY_CONTROLLER_ANALOG_RIGHT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_UP]       &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_UP]       |= au[GY_CONTROLLER_ANALOG_RIGHT];
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_DOWN]     &= ~GY_BUT_FLAGS_IS_PRESSED;
    _devices[slot].cont.buttons[GY_CONTROLLER_BUT_ANALOG2_DOWN]     |= ad[GY_CONTROLLER_ANALOG_RIGHT];

    *input = &_devices[slot].cont;

    return _devices[slot].sdlJoystick? 1 : 0;
}

