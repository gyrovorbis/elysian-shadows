#ifndef GYRO_BLOOM_GLAL_H
#define GYRO_BLOOM_GLAL_H

#include <glal/gyro_shaders_glal.h>

#define GYRO_GLAL_BLOOM_TEXTURE_WIDTH    1024
#define GYRO_GLAL_BLOOM_TEXTURE_HEIGHT   GYRO_GLAL_BLOOM_TEXTURE_WIDTH
#define GYRO_GLAL_BLOOM_TEXTURE_FILTER   GL_LINEAR
#define GYRO_GLAL_BLOOM_BLIT_FILTER      GL_LINEAR
#define GYRO_GLAL_BLOOM_TEXTURE_WRAP     GL_CLAMP_TO_EDGE
#define GYRO_GLAL_BLOOM_VERTEX_SHADER    "/assets/shaders/gaussianBlur.vsh"
#define GYRO_GLAL_BLOOM_FRAGMENT_SHADER  "/assets/shaders/gaussianBlur.fsh"

#ifdef __cplusplus
extern "C" {
#endif


int glalVidInitializeBloom(void);
int glalVidUninitializeBloom(void);
int gyVidBloomResizeFramebuffer(unsigned width, unsigned height);


#ifdef __cplusplus
}
#endif

#endif // GYRO_BLOOM_GLAL_H
