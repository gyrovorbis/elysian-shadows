#ifndef GYRO_TEXTURE_GLAL_H
#define GYRO_TEXTURE_GLAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYTexCoord2 {
	float u, v;
} GYTexCoord2;

typedef struct _GYTexCoordFrame {
	GYTexCoord2 topLeft;
	GYTexCoord2 botRight;
	uint16_t spriteWidth, spriteHeight;
} GYTexCoordFrame;


#ifdef __cplusplus
}
#endif

#endif // GYRO_TEXTURE_GLAL_H
