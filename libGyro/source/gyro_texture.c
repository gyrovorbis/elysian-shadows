#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "gyro_texture.h"
#include "gyro_system.h"
#include "gyro_asset_bank.h"
#include "gyro_platform.h"

#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#include <kos.h> //for PVR_PACK_16BIT_UV()
#endif

//Used internally for searches/construction
typedef struct _GYTexEntryDat {
	const char *const file;
	unsigned sheetWidth, sheetHeight;
	unsigned spriteWidth, spriteHeight;
} GYTexEntryDat;

static GYTexCoordSheet *_LoadSheetCoords(const GYTexEntryDat *const data) {
	char fname[GYRO_FILENAME_MAX_SIZE];
	FILE *file;
	unsigned amount, i, x, y, w, h;
	GYTexCoordSheet *coord;

	strcpy(fname, data->file);
	strcat(fname, GYRO_SHEET_TEX_COORD_EXT);
	file = fopen(fname, "r");
	if(!file) {
		gyPrintf("! - _LoadSheetCoords(%s): unable to open file!\n", fname);	
		//assert(file); //UNABLE TO OPEN SPRITE SHEET COORDINATE FILE!
		return NULL;
	}

	coord = malloc(sizeof(GYTexCoordSheet));
	fscanf(file, "%u", &amount);
	coord->frames = amount;
	coord->sheetWidth = data->sheetWidth;
	coord->sheetHeight = data->sheetHeight;
	coord->spriteWidth = coord->spriteHeight = 0;
	coord->texCoord = malloc(sizeof(GYTexCoordFrame)*amount);

	for(i = 0; i < amount; ++i) {
		fscanf(file, "%u%u%u%u", &x, &y, &w, &h); 
		coord->texCoord[i].spriteWidth = w;
		coord->texCoord[i].spriteHeight = h;
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
		coord->texCoord[i].topLeft.u = ((float)x/(float)data->sheetWidth);
		coord->texCoord[i].topLeft.v = ((float)y/(float)data->sheetHeight);
		coord->texCoord[i].botRight.u = ((float)(x+w)/(float)data->sheetWidth);
		coord->texCoord[i].botRight.v = ((float)(y+h)/(float)data->sheetHeight);
#else	//Pack the bitches on Dreamcast
		coord->texCoord[i].topLeft.uv = PVR_PACK_16BIT_UV((float)x/(float)data->sheetWidth, (float)y/(float)data->sheetHeight);
		coord->texCoord[i].botRight.uv = PVR_PACK_16BIT_UV((float)(x+w)/(float)data->sheetWidth, (float)(y+h)/(float)data->sheetHeight);
		coord->texCoord[i].topRight.uv = PVR_PACK_16BIT_UV((float)(x+w)/(float)data->sheetWidth, (float)y/data->sheetHeight);
		coord->texCoord[i].botLeft.uv = PVR_PACK_16BIT_UV((float)x/(float)data->sheetWidth, (float)(y+h)/(float)data->sheetHeight);
#endif
	}
	fclose(file);
	return coord;
}

static int _TexCoordComparator(const void *const payload, const void *const data) {
	return (((const GYTexCoordSheet*)payload)->sheetWidth == ((GYTexEntryDat*)data)->sheetWidth && ((GYTexCoordSheet*)payload)->sheetHeight == ((GYTexEntryDat*)data)->sheetHeight &&
			((const GYTexCoordSheet*)payload)->spriteWidth == ((GYTexEntryDat*)data)->spriteWidth && ((GYTexCoordSheet*)payload)->spriteHeight == ((GYTexEntryDat*)data)->spriteWidth);
}

static void *_TexCoordConstructor(void *const data) {
	GYTexCoordSheet *coord;
	const GYTexEntryDat *texData = data;

	unsigned int x_tiles = (unsigned int)(texData->sheetWidth / texData->spriteWidth);
	unsigned int y_tiles = (unsigned int)(texData->sheetHeight / texData->spriteHeight);

	float u_inc = (float)texData->spriteWidth / (float)texData->sheetWidth;
	float v_inc = (float)texData->spriteHeight / (float)texData->sheetHeight;
	unsigned int i,j,index;
	float u,v;

	//Allocate sprite coords and add entry
	coord = malloc(sizeof(GYTexCoordSheet));
	coord->sheetWidth = texData->sheetWidth;
	coord->sheetHeight = texData->sheetHeight;
	coord->spriteWidth = texData->spriteWidth;
	coord->spriteHeight = texData->spriteHeight;

	// Allocate x_tiles * y_tiles coordinates
	coord->frames = x_tiles * y_tiles;
	coord->texCoord = malloc(sizeof(GYTexCoordFrame)*coord->frames);

	for(i = 0; i < y_tiles; ++i) {
		v = v_inc*i;
		for (j = 0; j < x_tiles; ++j) {
			u = u_inc*j;
			index = j+(i*x_tiles);

			coord->texCoord[index].spriteWidth = texData->spriteWidth;
			coord->texCoord[index].spriteHeight = texData->spriteHeight;
//GAAAAAY, but it must be done...
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
			coord->texCoord[index].topLeft.u = (u);
			coord->texCoord[index].topLeft.v = (v);
			coord->texCoord[index].botRight.u = (u+u_inc);
			coord->texCoord[index].botRight.v = (v+v_inc);
#else	//Pack the bitches on Dreamcast
			coord->texCoord[index].topLeft.uv = PVR_PACK_16BIT_UV(u, v);
			coord->texCoord[index].botRight.uv = PVR_PACK_16BIT_UV((u+u_inc), (v+v_inc));
			coord->texCoord[index].topRight.uv = PVR_PACK_16BIT_UV((u+u_inc), v);
			coord->texCoord[index].botLeft.uv = PVR_PACK_16BIT_UV(u, (v+v_inc));
#endif
		}
	}

	return coord;
}

static int _TexCoordDestructor(void *const payload) {
	free(payload);
	return 1;
}

static GYAssetBank _texCoordBank = { 0, 0, 0, &_TexCoordComparator, &_TexCoordConstructor, &_TexCoordDestructor };

static int _TexComparator(const void *const payload, const void *const data) {
	return (strcmp(((const GYTexture*)payload)->filename, ((const char *)data)) == 0);
}

static void *_TexConstructor(void *const data) {
	GYTexture *tex;
	GYTexEntryDat *texData = data;
	char cwd[1024];

	//Allocate texture and add entry
	tex = malloc(sizeof(GYTexture));
	strcpy(tex->filename, texData->file);
	tex->bpp = GY_TEX_16BPP; //DEREEST ME!
	tex->palette = GY_PAL_NONE;
	switch(tex->bpp) {
	case GY_TEX_16BPP:
		tex->bytes = texData->sheetWidth * texData->sheetHeight * 2;
		break;
	case GY_TEX_8BPP:
		tex->bytes = texData->sheetWidth * texData->sheetHeight;
		break;
	case GY_TEX_4BPP:
		tex->bytes = texData->sheetWidth * texData->sheetHeight / 2;
		break;
	}

	//platform-specific texture allocation
	if(!_gyVidTexAlloc(tex, texData->sheetWidth, texData->sheetHeight)) {
		free(tex);
		_gyLog(GY_DEBUG_CRITICAL, "! - Could not load texture! - %s", tex->filename);
	}

	tex->_coord = (!texData->spriteWidth || !texData->spriteHeight)?
		_LoadSheetCoords(texData) :
		gyAssetBankInsert(&_texCoordBank, texData);
	assert(tex->_coord);
	return tex;
}

static int _TexDestructor(void *const payload) {
	GYTexture *tex = payload;

	int retVal = _gyVidTexFree(tex);
	if(!tex->_coord->spriteWidth || !tex->_coord->spriteHeight) { //non uniform
		free(tex->_coord->texCoord);
		free(tex->_coord);
	} else {
		retVal &= gyAssetBankRemove(&_texCoordBank, tex->_coord);
	}
	free(tex);
	return retVal;
}

static GYAssetBank _texBank = { 0, 0, 0, &_TexComparator, &_TexConstructor, &_TexDestructor };

GYTexture *gyVidTexCreateSheetUniform(const char *const filename, const unsigned int sheetWidth, const unsigned int sheetHeight, const unsigned int spriteWidth, const unsigned int spriteHeight) {
	GYTexEntryDat data = { filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight };
	return gyAssetBankInsert(&_texBank, &data);
}

GYTexture *gyVidTexCreateSheet(const char *const filename, const unsigned width, const unsigned height) {
	GYTexEntryDat data = { filename, width, height, 0, 0 };
	return gyAssetBankInsert(&_texBank, &data);
}

GYTexture *gyVidTexCreate(const char *const filename, const unsigned width, const unsigned height) {
	GYTexEntryDat data = { filename, width, height, width, height };
	return gyAssetBankInsert(&_texBank, &data);
}

int gyVidTexDestroy(GYTexture *const tex) {
	if(!tex) return 0;
	return gyAssetBankRemove(&_texBank, tex);
}

//Better make damn sure there were no more active references, unsafe.
int gyVidTexClear(void) {
	return gyAssetBankClear(&_texBank);
}

unsigned int gyVidTexAmount(void) {
	return gyAssetBankSize(&_texBank);
}

/*I hate to fucking do this, but SDL forces us with window resizing...
	ughhghg --Falco */
int gyVidTexReloadAll(void) {
	unsigned int success = 1;
	unsigned int width, height;
	GYAssetBankEntry *entry = _texBank._head;
	GYTexture *tex;
	for(; entry; entry = entry->_next) {
		tex = entry->_payload;
		width = tex->_coord->sheetWidth;
		height = tex->_coord->sheetHeight;
		success &= _gyVidTexFree(tex);
		success &= _gyVidTexAlloc(tex, width, height);
	}
	return success;
}

void gyVidTexWalkthrough(void) {
    unsigned int count = 0;
    GYTexCoordSheet *coord;
    GYTexture *tex;
	GYAssetBankEntry *entry = _texCoordBank._head;

    gyPrintf("====TEX COORD WALKTHROUGH====\n");
	for(; entry; entry = entry->_next) {
		coord = entry->_payload;
		gyPrintf("%d) Sheet - %dx%d, Sprite - %dx%d\n", count, coord->sheetWidth, coord->sheetHeight, coord->spriteWidth, coord->spriteHeight);
        gyPrintf("\tFrames - %d, _refCount - %d\n", coord->frames, entry->_refCount);
        ++count;
    }
    gyPrintf("====/TEX COORD WALKTHROUGH====\n");

    count = 0;
    gyPrintf("====TEXTURE WALKTHROUGH====\n");
	for(entry = _texBank._head; entry; entry = entry->_next) {
		tex = entry->_payload;
        gyPrintf("%d) %s\n", count, tex->filename);
        gyPrintf("\tbpp - %d, palette - %d\n", tex->bpp, tex->palette);
        gyPrintf("\tSheet size - %dx%d\n", tex->_coord->sheetWidth, tex->_coord->sheetHeight);
        gyPrintf("\tSprite size - %dx%d", tex->_coord->spriteWidth, tex->_coord->spriteHeight);
        gyPrintf("\t_refCount - %d\n", entry->_refCount);
        ++count;
    }
    gyPrintf("====/TEXTURE WALKTHROUGH====\n");
}


