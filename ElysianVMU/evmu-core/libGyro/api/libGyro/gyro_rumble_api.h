/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_RUMBLE_API_H
#define GYRO_RUMBLE_API_H

#include "gyro_platform.h"
#include "gyro_matrix_api.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_RUMBLE_EFFECT_DURATION_INFINITE -1.0f

typedef struct GYRumbleInitParams {
    unsigned maxDevices;
} GYRumbleInitParams;

typedef enum GY_RUMBLE_EFFECT_TYPE {
    GY_RUMBLE_EFFECT_NONE,
    GY_RUMBLE_EFFECT_LEFTRIGHT,
    GY_RUMBLE_EFFECT_CONSTANT,
    GY_RUMBLE_EFFECT_RAMP,
    GY_RUMBLE_EFFECT_PERIODIC
} GY_RUMBLE_EFFECT_TYPE;

typedef enum GY_RUMBLE_EFFECT_PERIODIC_TYPE {
    GY_RUMBLE_EFFECT_PERIODIC_SINE,
    GY_RUMBLE_EFFECT_PERIODIC_TRIANGLE,
    GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHUP,
    GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHDOWN
} GY_RUMBLE_EFFECT_PERIODIC_TYPE;

typedef struct GYRumbleEffectLeftRight {
    float                       largeIntensity;
    float                       smallIntensity;
} GYRumbleEffectLeftRight;

typedef struct GYRumbleEffectConstant {
    GYVector3                   direction;
    float                       delay;

    //SDL2_Haptic "Envelope"
    float                       startIntensity;
    float                       endIntensity;
    float                       fadeInDuration;
    float                       fadeOutDuration;

    //Constant-specific
    float                       intensity;
} GYRumbleEffectConstant;

typedef struct GYRumbleEffectRamp {
    //Directional
    GYVector3                   direction;
    float                       delay;

    //SDL2_Haptic "Envelope"
    float                       startIntensity;
    float                       endIntensity;
    float                       fadeInDuration;
    float                       fadeOutDuration;

    //Ramp-specific
    float                       rampStartIntensity;
    float                       rampEndIntensity;
} GYRumbleEffectRamp;

typedef struct GYRumbleEffectPeriodic {
    //Directional
    GYVector3                   direction;
    float                       delay;

    //SDL2_Haptic "Envelope"
    float                       startIntensity;
    float                       endIntensity;
    float                       fadeInDuration;
    float                       fadeOutDuration;

    //Periodic-Specific
    GY_RUMBLE_EFFECT_PERIODIC_TYPE
                                periodicType;
    float                       period;
    float                       phaseShift;       //pos phase shift given by 1/100th of a degree
    float                       intensityPeak;    //negative value => 180 deg phase-shift
    float                       intensityOffset;  //mean value of wave
} GYRumbleEffectPeriodic;

typedef struct GYRumbleEffect {
    GY_RUMBLE_EFFECT_TYPE       effectType;

    //SDL2_Haptic "Replay"
    float                       duration;

    //Effect-specific data
    union {
        GYRumbleEffectLeftRight leftRight;
        GYRumbleEffectConstant  constant;
        GYRumbleEffectRamp      ramp;
        GYRumbleEffectPeriodic  periodic;
    };

} GYRumbleEffect;


typedef struct GYRumble {
    unsigned                    featureFlags;
} GYRumble;

int                             gyRumbleInit(const struct GYRumbleInitParams* params);
int                             gyRumbleUninit(void);

int                             gyRumblePoll(unsigned slot, GYRumble** dev);
int                             gyRumbleDeviceQuery(GYRumble* dev);

int                             gyRumblePlay(struct GYRumble* dev, float intensity, uint32_t duration);
int                             gyRumblePlayEffect(GYRumble* dev, const GYRumbleEffect* effect, uint32_t iterations);
int                             gyRumbleStop(struct GYRumble* dev);




#ifdef GYRO_RUMBLE_PLATFORM_INCLUDE
#	include GYRO_RUMBLE_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif // GYRO_RUMBLE_API_H
