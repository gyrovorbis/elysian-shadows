#include <gyro_rumble_api.h>
#include <x86/gyro_sdl_input.h>
#include <gyro_system_api.h>
#include <x86/gyro_sdl_x86.inl>
#include <assert.h>

//========== INTERNALS =========

#define SDL_HAPTIC_DEVICE_UNUSED_PORT   -1

typedef struct SDLHapticDevice {
    GYRumble    _gyRumble;
    SDL_Haptic* _haptic;
    int         _port;

} SDLHapticDevice;

static GYRumbleInitParams _initParams = {
    0
};

static SDLHapticDevice  _tempDevice = {
    {
      0
    },
    NULL,
    SDL_HAPTIC_DEVICE_UNUSED_PORT
};
static SDLHapticDevice* _devices = NULL;
static unsigned         _deviceCount = 0;


static SDLHapticDevice* _claimFreeDevice(void) {
    for(unsigned i = 0; i < _initParams.maxDevices; ++i) {
        if(_devices[i]._port == SDL_HAPTIC_DEVICE_UNUSED_PORT) {
            ++_deviceCount;
            assert(_deviceCount <= _initParams.maxDevices);
            return &_devices[i];
        }
    }

    return NULL;
}

static void _releaseDevice(SDLHapticDevice* device) {
    if(device->_port != SDL_HAPTIC_DEVICE_UNUSED_PORT) {
        device->_port = SDL_HAPTIC_DEVICE_UNUSED_PORT;
        --_deviceCount;
    }
    SDL_HapticClose(device->_haptic);
    device->_haptic = NULL;
}

static SDLHapticDevice* _getSDLHapticDeviceByGYRumble(const GYRumble* rumble) {
    for(unsigned i = 0; i < _initParams.maxDevices; ++i) {
        if(&_devices[i]._gyRumble == rumble) return &_devices[i];
    }

    if(rumble == &_tempDevice._gyRumble) return &_tempDevice;

    return NULL;

}

static SDLHapticDevice* _getSDLHapticDeviceByPort(int port) {
    for(unsigned i = 0; i < _initParams.maxDevices; ++i) {
        if(_devices[i]._port == port) return &_devices[i];
    }

    return NULL;
}

int _gyRumbleSDLTryOpenHapticJoystick(SDL_Joystick* joystick, int port) {
    int success = 1;
    SDL_Haptic* haptic = NULL;

    GY_SDL_CLEAR_ERROR();
    if(!SDL_JoystickIsHaptic(joystick)) return 1;
    else {

        SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joystick);
        if(!haptic) {
            _gyLog(GY_DEBUG_ERROR, "[GYRO_RUMBLE] Could not open SDL_Haptic from SDL_Joystick on port: [%d]", port);

        } else {
            _gyLog(GY_DEBUG_VERBOSE, "[GYRO_RUMBLE] SDL Haptic Device Detected on port: [%d]", port);
            _gyPush();

            if (!SDL_HapticRumbleSupported(haptic)) {
                _gyLog(GY_DEBUG_VERBOSE, "[GYRO_RUMBLE] Haptic device does not have Rumble support!");
                goto haptic_close;

            } else {
                _gyLog(GY_DEBUG_VERBOSE, "[GYRO_RUMBLE]: Rumble Device attached to port: [%d]", port);

                if (SDL_HapticRumbleInit(haptic) != 0) {
                    _gyLog(GY_DEBUG_ERROR, "Haptic Rumble Init Failed: %s", SDL_GetError());
                    goto haptic_error;

                } else {
                    SDLHapticDevice* dev = (port == SDL_HAPTIC_DEVICE_UNUSED_PORT)? &_tempDevice : _claimFreeDevice();
                    if(!dev) dev = &_tempDevice;
                    else dev->_port = port;
                    dev->_haptic = haptic;
                    gyRumbleDeviceQuery(&dev->_gyRumble);
                    goto done;
                }
            }
        }
    }

 haptic_error:
    success = 0;
 haptic_close:
    SDL_HapticClose(haptic);
done:
    _gyPop(1);
    return success;

}

int _gyRumbleSDLTryCloseHapticJoystick(SDL_Joystick* joystick, int port) {
    (void) joystick;
    int success = 1;
        SDLHapticDevice* dev = (port == SDL_HAPTIC_DEVICE_UNUSED_PORT)? &_tempDevice : _getSDLHapticDeviceByPort(port);
        if(dev) {
            _releaseDevice(dev);
        } else success = 0;
    return success;
}


//========== PUBLIC API IMPLEMENTATION =========

int gyRumbleDeviceQuery(GYRumble* dev) {
    if(!dev) return 0;

    SDLHapticDevice* device = _getSDLHapticDeviceByGYRumble(dev);
    assert(device);

#define CHECK_HAPTIC_FEATURE(a) \
    if(flags & SDL_HAPTIC_##a) _gyLog(GY_DEBUG_VERBOSE, #a)

    _gyLog(GY_DEBUG_VERBOSE, "SDL_Haptic Supported Effects");
    _gyPush();
    int success = 1;
    assert(device);

    GY_SDL_CLEAR_ERROR();
    unsigned flags = SDL_HapticQuery(device->_haptic);
    if(!flags) {
        GY_SDL_CHECK_ERROR();
        success = 0;
    } else {
        device->_gyRumble.featureFlags = flags;
        CHECK_HAPTIC_FEATURE(CONSTANT);
        CHECK_HAPTIC_FEATURE(SINE);
        CHECK_HAPTIC_FEATURE(LEFTRIGHT);
        CHECK_HAPTIC_FEATURE(TRIANGLE);
        CHECK_HAPTIC_FEATURE(SAWTOOTHUP);
        CHECK_HAPTIC_FEATURE(SAWTOOTHDOWN);
        CHECK_HAPTIC_FEATURE(RAMP);
        CHECK_HAPTIC_FEATURE(SPRING);
        CHECK_HAPTIC_FEATURE(DAMPER);
        CHECK_HAPTIC_FEATURE(INERTIA);
        CHECK_HAPTIC_FEATURE(CUSTOM);
        CHECK_HAPTIC_FEATURE(GAIN);
        CHECK_HAPTIC_FEATURE(AUTOCENTER);
        CHECK_HAPTIC_FEATURE(STATUS);
        CHECK_HAPTIC_FEATURE(PAUSE);
    }

    _gyPop(1);

#undef CHECK_HAPTIC_FEATURE
    return success;
}

int gyRumbleInit(struct GYRumbleInitParams* params) {
    int success = 1;
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_RUMBLE_API]: Initializing SDL_Haptic");
    _gyPush();

    if(params) memcpy(&_initParams, params, sizeof(GYRumbleInitParams));

    GY_SDL_CLEAR_ERROR();
    if(SDL_Init(SDL_INIT_HAPTIC) < 0) {
        success = 0;
        _gyLog(GY_DEBUG_ERROR, "Could not initialize SDL_GAMECONTROLLER! [%s]", SDL_GetError());
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "Max requested rumble device ports: %d", _initParams.maxDevices);

        _devices    = malloc(sizeof(SDLHapticDevice)*_initParams.maxDevices);
        memset(_devices, 0, sizeof(SDLHapticDevice)*_initParams.maxDevices);
        for(unsigned i = 0; i < _initParams.maxDevices; ++i) {
            _devices[i]._port = SDL_HAPTIC_DEVICE_UNUSED_PORT;
        }
        _deviceCount = 0;
    }

    return success;
}

int gyRumbleUninit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "[GYRO_RUMBLE_API]: Uninitializing SDL_Haptic");

    for(unsigned i = 0; i < _initParams.maxDevices; ++i) {
        _releaseDevice(&_devices[i]);
    }
    assert(!_deviceCount);

    free(_devices);
    _devices = NULL;
    _deviceCount = 0;

    SDL_QuitSubSystem(SDL_INIT_HAPTIC);
    _initParams.maxDevices = 0;
    return 1;
}


int gyRumblePoll(unsigned slot, GYRumble** handle) {
   assert(slot < _initParams.maxDevices);
   SDLHapticDevice* dev = &_devices[slot];
   assert(dev);
   *handle = &dev->_gyRumble;
   return dev->_port != -1;

}

int gyRumblePlay(struct GYRumble* dev, float intensity, uint32_t duration) {
    SDLHapticDevice* sdlDev = _getSDLHapticDeviceByGYRumble(dev);
    if(!sdlDev || !sdlDev->_haptic) return 0;

    int success = 1;
    GY_SDL_CLEAR_ERROR();
    if(SDL_HapticRumblePlay(sdlDev->_haptic, intensity, duration) < 0) {
        GY_SDL_CHECK_ERROR();
        success = 0;
    }
    return success;
}

int gyRumbleStop(struct GYRumble* dev) {
    SDLHapticDevice* sdlDev = _getSDLHapticDeviceByGYRumble(dev);
    if(!sdlDev || !sdlDev->_haptic) return 0;

    int success = 1;
    GY_SDL_CLEAR_ERROR();

    if(SDL_HapticRumbleStop(sdlDev->_haptic) < 0) {
        GY_SDL_CHECK_ERROR();
        success = 0;
    }
    return success;
}


#if 0
int gyRumbleEffectInit(struct GYRumbleEffect* effect, const GYRumbleEffectInitParams* params) {
    GY_SDL_CLEAR_ERROR();
    effect->_handle = SDL_HapticNewEffect(effect->_handle, (SDL_HapticEffect*) effect->_handle);

}
#endif
int gyRumbleEffectPlay(struct GYRumble* dev, const struct GYRumbleEffect* effect) {

}
int gyRumbleEffectStatus(const struct GYRumble* dev, const struct GYRumbleEffect* effect) {

}



int gyRumbleDeviceDump(const struct GYRumble* dev) {

}
int gyRumbleDeviceEffectsDump(const struct GYRumble* dev) {

}
