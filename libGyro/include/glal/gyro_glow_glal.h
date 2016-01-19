#ifndef GYRO_GLOW_GLAL_H
#define GYRO_GLOW_GLAL_H

#include <glal/gyro_shaders_glal.h>

#define GYRO_GLAL_GLOW_TEXTURE_WIDTH    512
#define GYRO_GLAL_GLOW_TEXTURE_HEIGHT   512
#define GYRO_GLAL_GLOW_TEXTURE_FILTER   GL_NEAREST
#define GYRO_GLAL_GLOW_TEXTURE_WRAP     GL_CLAMP_TO_EDGE
#define GYRO_GLAL_GLOW_VERTEX_SHADER    "/assets/shaders/glowConvolution.vsh"
#define GYRO_GLAL_GLOW_FRAGMENT_SHADER  "/assets/shaders/glowConvolution.fsh"

#ifdef __cplusplus
extern "C" {
#endif


int glalVidInitializeGlow(void);
int glalVidUninitializeGlow(void);
void gyVidApplyGlow(void);


#ifdef __cplusplus
}
#endif

#endif // GYRO_GLOW_GLAL_H
