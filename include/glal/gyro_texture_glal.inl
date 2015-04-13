#ifndef GYRO_TEXTURE_GLAL_INL
#define GYRO_TEXTURE_GLAL_INL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYTexCoord2 {
    float u, v;
} GYTexCoord2;

typedef struct GYTexCoordFrame {
    GYTexCoord2 topLeft;
    GYTexCoord2 botRight;
    uint16_t spriteWidth, spriteHeight;
} GYTexCoordFrame;


#ifdef __cplusplus
}
#endif

#endif // GYRO_TEXTURE_GLAL_H
