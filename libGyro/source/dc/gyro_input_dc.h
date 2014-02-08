#ifndef GYRO_INPUT_DC_H
#define GYRO_INPUT_DC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _GY_DC_BUTTON { GY_CONT_BUT_A, GY_CONT_BUT_B, GY_CONT_BUT_X, GY_CONT_BUT_Y, GY_CONT_BUT_START,
						GY_CONT_DPAD_L, GY_CONT_DPAD_R, GY_CONT_DPAD_U, GY_CONT_DPAD_D,
						GY_CONT_LTRIG, GY_CONT_RTRIG, GY_CONT_LAST
} GY_DC_BUTTON;

typedef struct _GYInput {
	char analog1X, analog1Y;
	char analog2X, analog2Y;
	
	unsigned char butDown[GY_CONT_LAST];
	unsigned char butTapped[GY_CONT_LAST];
	
} GYInput;

#ifdef __cplusplus
}
#endif

#endif
