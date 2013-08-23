#ifndef GYRO_TEXTURE_H
#define GYRO_TEXTURE_H

#include "gyro_defines.h"
#include "gyro_platform.h"
#include <stdint.h>

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

typedef struct _GYTexCoord2 {
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
    float u, v;
#else
	uint32_t uv; //packed together on Dreamcast
#endif
} GYTexCoord2;

typedef struct _GYTexCoordFrame {
	GYTexCoord2 topLeft;
	GYTexCoord2 botRight;
#if GYRO_PLATFORM == GYRO_PLATFORM_DC
	GYTexCoord2 topRight;
	GYTexCoord2 botLeft;
#endif
	uint16_t spriteWidth, spriteHeight;
} GYTexCoordFrame;

typedef struct _GYTexCoordSheet {
	unsigned int sheetWidth, sheetHeight;
	unsigned int spriteWidth, spriteHeight; //if these are 0, sheet is not uniform
	unsigned int frames;
	GYTexCoordFrame *texCoord;
} GYTexCoordSheet;

typedef struct _GYTexture {
	char filename[GYRO_FILENAME_MAX_SIZE];
	//unsigned int sheetWidth, sheetHeight; //duplicate data?
	//unsigned int spriteWidth, spriteHeight; //duplicate data?
	unsigned int bytes;
	GY_TEX_BPP bpp;
	GY_TEX_PAL palette;
	GYTexCoordSheet *_coord;
	void *_data;
} GYTexture;

GYTexture *gyVidTexCreateSheetUniform(const char *const filename, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight);
GYTexture *gyVidTexCreateSheet(const char *const filename, const unsigned width, const unsigned height);
GYTexture *gyVidTexCreate(const char *const filename, const unsigned int width, const unsigned int height);

int gyVidTexDestroy(GYTexture *const tex);
unsigned int gyVidTexAmount(void);
int gyVidTexClear(void);
int gyVidTexReloadAll(void);
void gyVidTexWalkthrough(void);

//===PRIVATE - INVOKED BY VIDEO SUBSYSTEM OR IS THIS SHIT EVER USED?====
int _gyVidTexInit(void);
int _gyVidTexUninit(void);

//===PLATFORM SPECIFIC PRIVATE====
int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned height);
int _gyVidTexFree(GYTexture *const tex);

    /*
#if GYRO_PLATFORM != GYRO_PLATFORM_DREAMCAST    

    
    static inline void gyVidTexCoordArrayFromFrame(GYTexCoord2 *const coord, const GYTexCoordFrame *const frame, const GY_RECT_TEX_ORIENT orient) {
        switch(orient) {
            case GY_RECT_TEX_ORIENT_NORMAL:
                coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
                coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
                coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
                coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
                coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
                coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
                coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; 
                coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_FLIP_VERT:
                coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
                coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; 
                coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
                coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; 
                break;
            case GY_RECT_TEX_ORIENT_ROT_90:
                coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
                coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
                coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; 
                coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_ROT_180:
                coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
                coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
                coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
                coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_ROT_270:
                coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
                coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; 
                coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
                coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
                coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
                coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
                coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
                coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v; 
                break;
            case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
                coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
                coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
                coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
                coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; 
                break;
            default:
                //assert(0);
                break;
		}
    }

#endif
*/
#ifdef __cplusplus
}
#endif

#endif
