/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_rumble_api.h>
#include <sdl/gyro_sdl_input.h>
#include <libGyro/gyro_system_api.h>
#include <sdl/gyro_sdl.inl>
#include <assert.h>

//========== INTERNALS =========

#define SDL_HAPTIC_DEVICE_UNUSED_PORT   -1

typedef struct SDLHapticDevice {
    GYRumble    _gyRumble;
    SDL_Haptic* _haptic;
    int         _port;
    int         _curEffect;

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

int gyRumbleInit(const struct GYRumbleInitParams* params) {
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
            _devices[i]._port       = SDL_HAPTIC_DEVICE_UNUSED_PORT;
            _devices[i]._curEffect  = -1;
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

inline static int _gyPeriodicTypeToSDLType(int type) {
    switch(type) {
    case GY_RUMBLE_EFFECT_PERIODIC_SINE:            return SDL_HAPTIC_SINE;
    case GY_RUMBLE_EFFECT_PERIODIC_TRIANGLE:        return SDL_HAPTIC_TRIANGLE;
    case GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHUP:      return SDL_HAPTIC_SAWTOOTHUP;
    case GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHDOWN:    return SDL_HAPTIC_SAWTOOTHDOWN;
    default:                                        return 0;
    }
}

inline static const char* _gyRumblePeriodicTypeStr(int type) {
    switch(type) {
    case SDL_HAPTIC_SINE:           return "SINE";
    case SDL_HAPTIC_TRIANGLE:       return "TRIANGLE";
    case SDL_HAPTIC_SAWTOOTHDOWN:   return "SAWTOOTHDOWN";
    case SDL_HAPTIC_SAWTOOTHUP:     return "SAWTOOTHUP";
    default:                        return "DIRECT";
    }
}

inline static int _gyRumbleEffectPeriodicSupportPriority(int flags, int mask1, int mask2, int mask3) {
    if(flags & mask1) return mask1;
    else if(flags & mask2) return mask2;
    else if(flags & mask3) return mask3;
    else return 0;
}


int gyRumblePlayEffect(GYRumble *dev, const GYRumbleEffect *effect, uint32_t iterations) {
    assert(effect);
    if(!dev) return 0;

    SDLHapticDevice* sdlDev = _getSDLHapticDeviceByGYRumble(dev);
    if(!sdlDev || !sdlDev->_haptic) return 0;

    uint32_t duration = (effect->duration == GY_RUMBLE_EFFECT_DURATION_INFINITE)? SDL_HAPTIC_INFINITY : effect->duration  * 1000.0f * iterations;
    float fallbackIntensity = 1.0f;
    SDL_HapticEffect sdlEffect;
    switch(effect->effectType) {
    case GY_RUMBLE_EFFECT_LEFTRIGHT:
        fallbackIntensity = (effect->leftRight.largeIntensity > 0)? effect->leftRight.largeIntensity : effect->leftRight.smallIntensity;
        if(!(dev->featureFlags & SDL_HAPTIC_LEFTRIGHT)) {
            _gyLog(GY_DEBUG_WARNING, "[GYRO_RUMBLE] RumbleDevice does not support LEFTRIGHT effects! (Falling through to direct playback)");
            return gyRumblePlay(dev, fallbackIntensity, duration);
        }
        sdlEffect.leftright.type            = SDL_HAPTIC_LEFTRIGHT;
        sdlEffect.leftright.length          = duration;
        sdlEffect.leftright.large_magnitude = effect->leftRight.largeIntensity * UINT16_MAX;
        sdlEffect.leftright.small_magnitude = effect->leftRight.smallIntensity * UINT16_MAX;

        break;

    case GY_RUMBLE_EFFECT_CONSTANT:
        if(!(dev->featureFlags & SDL_HAPTIC_CONSTANT)) {
            fallbackIntensity = effect->constant.intensity;
            _gyLog(GY_DEBUG_WARNING, "[GYRO_RUMBLE] RumbleDevice does not support CONSTANT effects! (Falling through to direct playback)");
            return gyRumblePlay(dev, fallbackIntensity, duration);
        }

        sdlEffect.constant.type             = SDL_HAPTIC_CONSTANT;
        sdlEffect.constant.length           = duration;
        sdlEffect.constant.attack_level     = effect->constant.startIntensity  * UINT16_MAX;
        sdlEffect.constant.fade_level       = effect->constant.endIntensity    * UINT16_MAX;
        sdlEffect.constant.attack_length    = effect->constant.fadeInDuration  * 1000.0f;
        sdlEffect.constant.fade_length      = effect->constant.fadeOutDuration * 1000.0f;
        sdlEffect.constant.delay            = effect->constant.delay            * 1000.0f;
        sdlEffect.constant.direction.type   = SDL_HAPTIC_CARTESIAN;
        sdlEffect.constant.direction.dir[0] = effect->constant.direction.x * ((effect->constant.direction.x < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.constant.direction.dir[1] = effect->constant.direction.y * ((effect->constant.direction.y < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.constant.direction.dir[2] = effect->constant.direction.z * ((effect->constant.direction.z < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.constant.level            = effect->constant.intensity * ((effect->constant.intensity < 0)? -INT16_MIN : INT16_MAX);
        break;

    case GY_RUMBLE_EFFECT_RAMP:
        if(!(dev->featureFlags & SDL_HAPTIC_RAMP)) {
            fallbackIntensity = (effect->ramp.startIntensity + effect->ramp.endIntensity)*0.5f;
            _gyLog(GY_DEBUG_WARNING, "[GYRO_RUMBLE] RumbleDevice does not support RAMP effects! (Falling through to direct playback)");
            return gyRumblePlay(dev, fallbackIntensity, duration);
        }

        sdlEffect.ramp.type             = SDL_HAPTIC_CONSTANT;
        sdlEffect.ramp.length           = duration;
        sdlEffect.ramp.attack_level     = effect->ramp.startIntensity  * UINT16_MAX;
        sdlEffect.ramp.fade_level       = effect->ramp.endIntensity    * UINT16_MAX;
        sdlEffect.ramp.attack_length    = effect->ramp.fadeInDuration  * 1000.0f;
        sdlEffect.ramp.fade_length      = effect->ramp.fadeOutDuration * 1000.0f;
        sdlEffect.ramp.delay            = effect->ramp.delay            * 1000.0f;
        sdlEffect.ramp.direction.type   = SDL_HAPTIC_CARTESIAN;
        sdlEffect.ramp.direction.dir[0] = effect->ramp.direction.x * ((effect->ramp.direction.x < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.ramp.direction.dir[1] = effect->ramp.direction.y * ((effect->ramp.direction.y < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.ramp.direction.dir[2] = effect->ramp.direction.z * ((effect->ramp.direction.z < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.ramp.start            = effect->ramp.rampStartIntensity * ((effect->ramp.rampStartIntensity < 0)? -INT16_MIN : INT16_MAX);
        sdlEffect.ramp.end              = effect->ramp.rampEndIntensity * ((effect->ramp.rampEndIntensity < 0)? -INT16_MIN : INT16_MAX);
        break;


    case GY_RUMBLE_EFFECT_PERIODIC: {
        fallbackIntensity = effect->periodic.intensityPeak;
        int actualType = _gyPeriodicTypeToSDLType(effect->periodic.periodicType);

        switch(actualType) {
        case GY_RUMBLE_EFFECT_PERIODIC_SINE:
            if(!(dev->featureFlags & SDL_HAPTIC_SINE)) {
                actualType = _gyRumbleEffectPeriodicSupportPriority(dev->featureFlags, SDL_HAPTIC_TRIANGLE, SDL_HAPTIC_SAWTOOTHUP, SDL_HAPTIC_SAWTOOTHDOWN);
            }
            break;
        case GY_RUMBLE_EFFECT_PERIODIC_TRIANGLE:
            if(!(dev->featureFlags & SDL_HAPTIC_TRIANGLE)) {
                actualType = _gyRumbleEffectPeriodicSupportPriority(dev->featureFlags, SDL_HAPTIC_SINE, SDL_HAPTIC_SAWTOOTHUP, SDL_HAPTIC_SAWTOOTHDOWN);
            }
            break;
        case GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHUP:
            if(!(dev->featureFlags & SDL_HAPTIC_SAWTOOTHUP)) {
                actualType = _gyRumbleEffectPeriodicSupportPriority(dev->featureFlags, SDL_HAPTIC_SAWTOOTHDOWN, SDL_HAPTIC_TRIANGLE, SDL_HAPTIC_SINE);
            }
            break;
        case GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHDOWN:
            if(!(dev->featureFlags & SDL_HAPTIC_SAWTOOTHDOWN)) {
                actualType = _gyRumbleEffectPeriodicSupportPriority(dev->featureFlags, SDL_HAPTIC_SAWTOOTHUP, SDL_HAPTIC_TRIANGLE, SDL_HAPTIC_SINE);
            }
            break;
        }
        int sdlEffectType = _gyPeriodicTypeToSDLType(effect->periodic.periodicType);

        if(actualType != sdlEffectType) {
            _gyLog(GY_DEBUG_WARNING, "[GYRO_RUMBLE] Periodic type %s not supported on device. Falling back to %s.", _gyRumblePeriodicTypeStr(sdlEffectType), _gyRumblePeriodicTypeStr(actualType));

            if(!actualType) {
                return gyRumblePlay(dev, fallbackIntensity, duration);
            }
        }

        sdlEffect.periodic.type             = actualType;
        sdlEffect.periodic.length           = duration;
        sdlEffect.periodic.attack_level     = effect->periodic.startIntensity  * UINT16_MAX;
        sdlEffect.periodic.fade_level       = effect->periodic.endIntensity    * UINT16_MAX;
        sdlEffect.periodic.attack_length    = effect->periodic.fadeInDuration  * 1000.0f;
        sdlEffect.periodic.fade_length      = effect->periodic.fadeOutDuration * 1000.0f;
        sdlEffect.periodic.delay            = effect->periodic.delay            * 1000.0f;
        sdlEffect.periodic.direction.type   = SDL_HAPTIC_CARTESIAN;
        sdlEffect.periodic.direction.dir[0] = effect->periodic.direction.x * ((effect->periodic.direction.x < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.periodic.direction.dir[1] = effect->periodic.direction.y * ((effect->periodic.direction.y < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.periodic.direction.dir[2] = effect->periodic.direction.z * ((effect->periodic.direction.z < 0)? -INT8_MIN : INT8_MAX);
        sdlEffect.periodic.period           = effect->periodic.period * 1000.0f;
        sdlEffect.periodic.magnitude        = effect->periodic.intensityPeak   * ((effect->periodic.intensityPeak < 0)? -INT16_MIN : INT16_MAX);
        sdlEffect.periodic.offset           = effect->periodic.intensityOffset * ((effect->periodic.intensityOffset < 0)? -INT16_MIN : INT16_MAX);
        sdlEffect.periodic.phase            = effect->periodic.phaseShift * 1000.0f;
        break;
    }
    case GY_RUMBLE_EFFECT_NONE:
        _gyLog(GY_DEBUG_ERROR, "[GYRO_RUMBLE] Tried to play a RumbleEffect of type NONE!");
        break;
    default: assert(0); break;
    }

    GY_SDL_CLEAR_ERROR();
    if(sdlDev->_curEffect >= 0) {
        SDL_HapticDestroyEffect(sdlDev->_haptic, sdlDev->_curEffect);
    }

    sdlDev->_curEffect = SDL_HapticNewEffect(sdlDev->_haptic, &sdlEffect);
    if(sdlDev->_curEffect < 0) {
        GY_SDL_CHECK_ERROR();
        gyRumblePlay(dev, fallbackIntensity, duration);
        return 0;
    }

    if(SDL_HapticRunEffect(sdlDev->_haptic, sdlDev->_curEffect, iterations) < 0) {
        GY_SDL_CHECK_ERROR();
        gyRumblePlay(dev, fallbackIntensity, duration);
        return 0;
    }

    return 1;
}


