#ifndef GYRO_INPUT_X86_H
#define GYRO_INPUT_X86_H

#ifdef __cplusplus
extern "C" {
#endif

//Two goddamn layers of translation? Christ.
typedef enum _GY_X86_KEY {
            GY_KEY_NONE=-1, GY_KEY_A, GY_KEY_B, GY_KEY_C, GY_KEY_D, GY_KEY_E, GY_KEY_F, GY_KEY_G, GY_KEY_H, GY_KEY_I, GY_KEY_J, GY_KEY_K, GY_KEY_L, GY_KEY_M, GY_KEY_N, GY_KEY_O, GY_KEY_P,
            GY_KEY_Q, GY_KEY_R, GY_KEY_S, GY_KEY_T, GY_KEY_U, GY_KEY_V, GY_KEY_W, GY_KEY_X, GY_KEY_Y, GY_KEY_Z,
            GY_KEY_DPAD_U, GY_KEY_DPAD_D, GY_KEY_DPAD_L, GY_KEY_DPAD_R,
			GY_KEY_0, GY_KEY_1, GY_KEY_2, GY_KEY_3, GY_KEY_4, GY_KEY_5, GY_KEY_6, GY_KEY_7, GY_KEY_8, GY_KEY_9,
			GY_KEY_SPACE, GY_KEY_RETURN, GY_KEY_ESC,
			GY_KEY_BACKSLASH, GY_KEY_FORWARDSLASH,
			GY_KEY_LSHIFT, GY_KEY_RSHIFT, GY_KEY_LCTRL, GY_KEY_RCTRL, GY_KEY_RALT, GY_KEY_LALT, GY_KEY_BACKQUOTE,
            GY_KEY_LAST
} GY_X86_KEY;

typedef struct _GYInput {
    unsigned char keyDown[GY_KEY_LAST];
    unsigned char keyTapped[GY_KEY_LAST];
} GYInput;

#ifdef __cplusplus
}
#endif

#endif // GYRO_INPUT_X86_H
