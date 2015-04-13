#ifndef GYRO_INPUT_H
#define GYRO_INPUT_H

#include "gyro_platform.h"
#include "gyro_matrix_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GYInputInitParams;
struct GYInput;

typedef enum GY_MOUSE_BUTTON {
    GY_MOUSE_BUT_NONE,
    GY_MOUSE_BUT_LEFT,
    GY_MOUSE_BUT_MIDDLE,
    GY_MOUSE_BUT_RIGHT,
    GY_MOUSE_BUT_MAX
} GY_MOUSE_BUTTON;

typedef struct GYMouse {
    int         butDown[GY_MOUSE_BUT_MAX];
    int         butTapped[GY_MOUSE_BUT_MAX];
    GYVector2   scrollWheel;
    GYVector2   screenPos;
} GYMouse;

int gyInputInit(const struct GYInputInitParams*const params);
int gyInputUninit(void);

void gyInputPoll(struct GYInput *const input);

void gyMousePoll(struct GYMouse*const mouse);

#ifdef GYRO_INPUT_PLATFORM_INCLUDE
#	include GYRO_INPUT_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif
