#ifndef GYRO_VMU_GAMEPAD_H
#define GYRO_VMU_GAMEPAD_H

#ifdef __cplusplus
extern "C" {
#endif

struct VMUDevice;

typedef enum VMU_GAMEPAD_BUTTON {
    VMU_GAMEPAD_UP,
    VMU_GAMEPAD_DOWN,
    VMU_GAMEPAD_LEFT,
    VMU_GAMEPAD_RIGHT,
    VMU_GAMEPAD_A,
    VMU_GAMEPAD_B,
    VMU_GAMEPAD_MODE,
    VMU_GAMEPAD_SLEEP,
    VMU_GAMEPAD_MAX
} VMU_GAMEPAD_BUTTON;

int     gyVmuButtonStateGet(const struct VMUDevice* dev, VMU_GAMEPAD_BUTTON but);
void    gyVmuButtonStateSet(struct VMUDevice* dev, VMU_GAMEPAD_BUTTON but, int down);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_GAMEPAD_H

