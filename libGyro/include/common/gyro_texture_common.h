#ifndef GYRO_TEXTURE_SHARED_H
#define GYRO_TEXTURE_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

struct _GYTexture;
struct _GYTexCoord2;

struct _GYTexture* gyVidTexCreateSheetUniform(const char *const filename,
											const unsigned sheetWidth,
											const unsigned sheetHeight,
											const unsigned spriteWidth,
											const unsigned spriteHeight);
struct _GYTexture* gyVidTexCreateSheet(const char *const filename, const unsigned width, const unsigned height);
struct _GYTexture* gyVidTexCreate(const char *const filename, const unsigned int width, const unsigned int height);
const struct _GYTexCoord2* gyVidTexCoordArrayFromFrame(const struct _GYTexCoordFrame*const frame,
													   const GY_RECT_TEX_ORIENT orient);

int             gyVidTexReload(struct _GYTexture* const tex);
int             gyVidTexDestroy(struct _GYTexture* const tex);
unsigned int    gyVidTexAmount(void);
unsigned        gyVidTexBytes(void);
int             gyVidTexClear(void);
int             gyVidTexReloadAll(void);
void            gyVidTexWalkthrough(void);


#ifdef __cplusplus
}
#endif

#endif // GYRO_TEXTURE_SHARED_H
