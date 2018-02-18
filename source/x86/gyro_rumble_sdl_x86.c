#include <gyro_rumble_api.h>
#include <x86/gyro_sdl_input.h>
#include <gyro_system_api.h>

#define GY_SDL_CLEAR_ERROR() \
    (_clearSDLError(__FILE__, __LINE__, __func__))

#define GY_SDL_CHECK_ERROR() \
    (_checkSDLError(__FILE__, __LINE__, __func__))

static inline int _clearSDLError(const char* file, int line, const char* func) {
    const char* error = SDL_GetError();
    if(strlen(error)) {
        _gyLog(GY_DEBUG_ERROR, "[GYRO_RUMBLE]: SDL_Haptic error found before: '%s' [%s:%d (func %s)]", error, file, line, func);
        return 1;
    }
    return 0;
}

static int _checkSDLError(const char* file, int line, const char* func) {
    const char* error = SDL_GetError();
    if(strlen(error)) {
        _gyLog(GY_DEBUG_ERROR, "[GYRO_RUMBLE]: SDL_Haptic error raised: '%s' [%s:%d (func %s)]", error, file, line, func);
        return 1;
    }
    return 0;
}


typedef struct GYRumble {

    void* _handle;
} GYRumble;

typedef struct GYRumbleEffect {
    void* _handle;

} GYRumbleEffect;


typedef struct GYRumbleEffectInitParams {
    float               angle;
    float               dir1;
    struct GYVector2*   dir2;
    struct GYVector3*   dir3;
    float               intensity;
    float               duration;
    bool                pulse;
    bool                decay;
} GYRumbleEffectInitParams;

static GYRumbleInitParams* _initParams = NULL;


int gyRumbleInit(struct GYRumbleInitParams* params) {
    _initParams = params;
    return 1;
}

int gyRumbleUninit(void) {
    return 1;
}


struct GYRumble* gyRumblePoll(unsigned slot) {

}




int gyRumblePlay(struct GYRumble* dev, float intensity, float duration) {
    GY_SDL_CLEAR_ERROR();
    if(SDL_HapticRunEffect(dev->_handle, effect_id, 1)) {//iterations
        GY_SDL_CHECK_ERROR();
    }
}

int gyRumbleStop(struct GYRumble* dev) {
    GY_SDL_CLEAR_ERROR();
    if(SDL_HapticRumbleStop(dev->_handle)) {
        GY_SDL_CHECK_ERROR();
    }
}

int gyRumbleEffectInit(struct GYRumbleEffect* effect, const GYRumbleEffectInitParams* params) {
    GY_SDL_CLEAR_ERROR();
    effect->_handle = SDL_HapticNewEffect(effect->_handle, SDL_HapticEffect* effect);

}
int gyRumbleEffectPlay(struct GYRumble* dev, const struct GYRumbleEffect* effect) {

}
int gyRumbleEffectStatus(const struct GYRumble* dev, const struct GYRumbleEffect* effect) {

}



int gyRumbleDeviceDump(const struct GYRumble* dev) {

}
int gyRumbleDeviceEffectsDump(const struct GYRumble* dev) {

}
