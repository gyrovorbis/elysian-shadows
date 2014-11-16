#ifndef GYRO_TEXTURE_H
#define GYRO_TEXTURE_H

#include "gyro_defines.h"
#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_SHEET_TEX_COORD_EXT ".sheet"
    
//Every possible way to orient a texture's UV coords (3 bits)
typedef enum _GY_RECT_TEX_ORIENT { GY_RECT_TEX_ORIENT_NORMAL, GY_RECT_TEX_ORIENT_FLIP_HORIZ, GY_RECT_TEX_ORIENT_FLIP_VERT,
    GY_RECT_TEX_ORIENT_ROT_90, GY_RECT_TEX_ORIENT_ROT_180, GY_RECT_TEX_ORIENT_ROT_270,
    GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90, GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270,
    GY_RECT_TEX_ORIENT_MAX //not representable with 3 bits
} GY_RECT_TEX_ORIENT;

typedef enum _GY_TEX_BPP { GY_TEX_16BPP, GY_TEX_8BPP, GY_TEX_4BPP } GY_TEX_BPP;
//TODO: PALETTE SUPPORT!
typedef enum _GY_PALETTE   { GY_PAL_NONE, GY_PAL_1, GY_PAL_2, GY_PAL_3, GY_PAL_4 } GY_TEX_PAL;

struct _GYTexCoord2;
struct _GYTexCoordFrame;

typedef struct _GYTexCoordSheet {
	unsigned					sheetWidth;
	unsigned					sheetHeight;
	unsigned					spriteWidth;
	unsigned					spriteHeight; //if these are 0, sheet is not uniform
	unsigned					frames;
	struct _GYTexCoordFrame*	texCoord;
} GYTexCoordSheet;

typedef struct _GYTexture {
	char					filename[GYRO_PATH_MAX_SIZE];
	unsigned				bytes;
	GY_TEX_BPP				bpp;
	GY_TEX_PAL				palette;
	GYTexCoordSheet*		_coord;
	void*					_data;
} GYTexture;


#include "common/gyro_texture_common.h"

#ifdef GYRO_TEXTURE_PLATFORM_INCLUDE
#	include GYRO_TEXTURE_PLATFORM_INCLUDE
#endif

//===PLATFORM SPECIFIC PRIVATE====
int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned height);
int _gyVidTexFree(GYTexture *const tex);

#ifdef __cplusplus
}
#endif

#endif
