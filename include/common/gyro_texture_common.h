#ifndef GYRO_TEXTURE_SHARED_H
#define GYRO_TEXTURE_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

struct GYTexture;
struct GYTexCoord2;

struct GYTexture* gyVidTexCreateSheetUniform(const char *const filename,
                                            const unsigned sheetWidth,
                                            const unsigned sheetHeight,
                                            const unsigned spriteWidth,
                                            const unsigned spriteHeight);
struct GYTexture* gyVidTexCreateSheet(const char *const filename, const unsigned width, const unsigned height);
struct GYTexture* gyVidTexCreate(const char *const filename);
const struct GYTexCoord2* gyVidTexCoordArrayFromFrame(const struct GYTexCoordFrame*const frame,
                                                       const GY_RECT_TEX_ORIENT orient);

int             gyVidTexReload(struct GYTexture* const tex);
int             gyVidTexReloadPath(const char*const path);
int             gyVidTexDestroy(struct GYTexture* const tex);
unsigned int    gyVidTexAmount(void);
unsigned        gyVidTexBytes(void);
int             gyVidTexClear(void);
int             gyVidTexReloadAll(void);
void            gyVidTexWalkthrough(void);
unsigned int    gyVidTexRefCount(const char*const filename);


#ifdef __cplusplus
}
#endif

#endif // GYRO_TEXTURE_SHARED_H
