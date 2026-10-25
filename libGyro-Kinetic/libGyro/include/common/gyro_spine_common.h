#ifndef GYRO_SPINE_COMMON_H
#define GYRO_SPINE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_SPINE_ATLAS_TEXTURE_NORMAL_SUFFIX      "_n.png"
#define GYRO_SPINE_ATLAS_TEXTURE_SPECULAR_SUFFIX    "_e.png"

struct GYTexture;

typedef struct GYSpineRenderObject {
    struct GYTexture* textureMap;
    struct GYTexture* normalMap;
    struct GYTexture* specularMap;
} GYSpineRenderObject;


#ifdef __cplusplus
}
#endif

#endif // GYRO_SPINE_COMMON_H
