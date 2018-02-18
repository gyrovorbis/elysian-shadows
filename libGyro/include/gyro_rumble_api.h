#ifndef GYRO_RUMBLE_API_H
#define GYRO_RUMBLE_API_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// GYRumble can be
// DC - maple_device_t
// PC - SDL_Haptic*

struct GYRumbleInitParams;
struct GYRumble;
struct GYRumbleEffect;
struct GYRumbleEffectInitParams;

struct GYVector2;
struct GYVector3;

typedef void GYRumble;

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


int                gyRumbleInit(struct GYRumbleInitParams* params);
int                gyRumbleUninit(void);


struct GYRumble*   gyRumblePoll(unsigned slot);
int                gyRumbleDeviceDump(const struct GYRumble* dev);
int                gyRumbleDeviceEffectsDump(const struct GYRumble* dev);

int                gyRumblePlay(struct GYRumble* dev, float intensity, float duration);
int                gyRumbleStop(struct GYRumble* dev);

int                gyRumbleEffectInit(struct GYRumbleEffect* effect, const GYRumbleEffectInitParams* params);
int                gyRumbleEffectPlay(struct GYRumble* dev, const struct GYRumbleEffect* effect);
int                gyRumbleEffectStatus(const struct GYRumble* dev, const struct GYRumbleEffect* effect); // <0 - error, 0 - stopped, 1 - playing











#ifdef GYRO_RUMBLE_PLATFORM_INCLUDE
#	include GYRO_RUMBLE_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_RUMBLE_API_H
