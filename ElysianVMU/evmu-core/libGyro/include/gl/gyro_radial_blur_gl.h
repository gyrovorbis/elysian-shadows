#ifndef GYRO_RADIAL_BLUR_GL
#define GYRO_RADIAL_BLUR_GL

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_GLAL_RADIAL_BLUR_VERTEX_SHADER     "/assets/shaders/radialBlur.vsh"
#define GYRO_GLAL_RADIAL_BLUR_FRAGMENT_SHADER   "/assets/shaders/radialBlur.fsh"

int glalVidInitializeRadialBlur(void);
int glalVidUninitializeRadialBlur(void);
void glalVidApplyRadialBlur(void);

#ifdef __cplusplus
}
#endif


#endif // GYRO_RADIAL_BLUR_GLAL_H
