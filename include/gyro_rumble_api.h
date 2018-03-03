#ifndef GYRO_RUMBLE_API_H
#define GYRO_RUMBLE_API_H

#include <gyro_platform.h>
#include <gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

// GYRumble can be
// DC - maple_device_t
// PC - SDL_Haptic*

typedef struct GYRumbleInitParams {
    unsigned maxDevices;

} GYRumbleInitParams;


struct GYRumbleInitParams;
struct GYRumbleEffect;
struct GYRumbleEffectInitParams;

struct GYVector2;
struct GYVector3;

typedef enum GY_RUMBLE_DIR_TYPE {
    GY_RUMBLE_DIR_1D,
    GY_RUMBLE_DIR_2D,
    GY_RUMBLE_DIR_3D
} GY_RUMBLE_DIR_TYPE;

typedef struct GYRumbleEffect{
#if 0
    typedef union {
        GYVector3       dir3;
        GYVector2       dir2;
        float           angle;
    } dir;
#endif
    GY_RUMBLE_DIR_TYPE  dirType;
    float               intensity;
    float               duration;
    int                 pulse;
    int                 decay;
} GYRumbleEffect;


typedef struct GYRumble {
    unsigned    featureFlags;
} GYRumble;


int                gyRumbleInit(struct GYRumbleInitParams* params);
int                gyRumbleUninit(void);

int                 gyRumblePoll(unsigned slot, GYRumble** dev);
int                 gyRumbleDeviceQuery(GYRumble* dev);

int                gyRumblePlay(struct GYRumble* dev, float intensity, uint32_t duration);
int                gyRumbleStop(struct GYRumble* dev);



int                gyRumbleDeviceEffectsDump(const struct GYRumble* dev);
//int                gyRumbleEffectInit(struct GYRumbleEffect* effect, const GYRumbleEffectInitParams* params);
int                gyRumbleEffectPlay(struct GYRumble* dev, const struct GYRumbleEffect* effect);
int                gyRumbleEffectStatus(const struct GYRumble* dev, const struct GYRumbleEffect* effect); // <0 - error, 0 - stopped, 1 - playing











#ifdef GYRO_RUMBLE_PLATFORM_INCLUDE
#	include GYRO_RUMBLE_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_RUMBLE_API_H
