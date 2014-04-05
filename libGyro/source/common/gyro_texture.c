#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include "gyro_asset_bank.h"

#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#include <kos.h> //for PVR_PACK_16BIT_UV()
#endif

static GYTexCoord2 _texFrameCoords[4];

//Used internally for searches/construction
typedef struct _GYTexEntryDat {
	const char *const file;
	unsigned sheetWidth, sheetHeight;
	unsigned spriteWidth, spriteHeight;
} GYTexEntryDat;

static GYTexCoordSheet *_LoadSheetCoords(const GYTexEntryDat *const data) {
	char fname[GYRO_PATH_MAX_SIZE];
	GYFile *file;
    size_t file_size = 0;
    char *buffer = NULL;
	unsigned amount, i, x, y, w, h;
	GYTexCoordSheet *coord;

	strcpy(fname, data->file);
	strcat(fname, GYRO_SHEET_TEX_COORD_EXT);
	if(!gyFileOpen(fname, "r", &file)) {
		gyPrintf("! - _LoadSheetCoords(%s): unable to open file!\n", fname);
		//assert(file); //UNABLE TO OPEN SPRITE SHEET COORDINATE FILE!
		return NULL;
	}

	coord = malloc(sizeof(GYTexCoordSheet));
    gyFileLength(file, &file_size);
    buffer = malloc(sizeof(char) * file_size);
    gyFileRead(file, buffer, file_size, NULL);
	sscanf(buffer, "%u", &amount);
	coord->frames = amount;
	coord->sheetWidth = data->sheetWidth;
	coord->sheetHeight = data->sheetHeight;
	coord->spriteWidth = coord->spriteHeight = 0;
	coord->texCoord = malloc(sizeof(GYTexCoordFrame)*amount);

	for(i = 0; i < amount; ++i) {
		sscanf(buffer, "%u%u%u%u", &x, &y, &w, &h);
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
    gyFileClose(&file);
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
	if(!payload) return 0;
	const GYTexEntryDat* texData = data;
	return (strcmp(((const GYTexture*)payload)->filename, texData->file) == 0);
}

static void *_TexConstructor(void *const data) {
	GYTexture *tex;
	GYTexEntryDat *texData = data;

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
		_gyLog(GY_DEBUG_CRITICAL, "Could not load texture! - %s", texData->file);
		free(tex);
		return NULL;
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
	GYTexEntryDat data = { tex->filename, tex->_coord->sheetWidth, tex->_coord->sheetHeight, tex->_coord->spriteWidth, tex->_coord->spriteHeight};
	return gyAssetBankRemove(&_texBank, &data);
}

//Better make damn sure there were no more active references, unsafe.
int gyVidTexClear(void) {
	return gyAssetBankClear(&_texBank);
}

unsigned int gyVidTexAmount(void) {
	return gyAssetBankSize(&_texBank);
}

unsigned gyVidTexBytes(void) {
	unsigned totalBytes = 0;
	GYAssetBankEntry *entry = _texBank._head;
	GYTexture *tex;
	for(; entry; entry = entry->_next) {
		tex = entry->_payload;
		if(!tex) continue;
		totalBytes += tex->bytes;
	}
	return totalBytes;
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
		if(!tex) continue;
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

	_gyLog(GY_DEBUG_VERBOSE, "====TEX COORD WALKTHROUGH====");
	for(; entry; entry = entry->_next) {
		coord = entry->_payload;
		_gyLog(GY_DEBUG_VERBOSE, "%d) Sheet - %dx%d, Sprite - %dx%d", count, coord->sheetWidth, coord->sheetHeight, coord->spriteWidth, coord->spriteHeight);
		_gyLog(GY_DEBUG_VERBOSE, "\tFrames - %d, _refCount - %d", coord->frames, entry->_refCount);
        ++count;
    }
	_gyLog(GY_DEBUG_VERBOSE, "====/TEX COORD WALKTHROUGH====");

    count = 0;
	_gyLog(GY_DEBUG_VERBOSE, "====TEXTURE WALKTHROUGH====");
	for(entry = _texBank._head; entry; entry = entry->_next) {
		tex = entry->_payload;
		if(!tex) {
			_gyLog(GY_DEBUG_VERBOSE, "%d) NULL", count++);
			continue;
		}
		_gyLog(GY_DEBUG_VERBOSE, "%d) %s", count, tex->filename);
		_gyLog(GY_DEBUG_VERBOSE, "\tbpp - %d, palette - %d", tex->bpp, tex->palette);
		_gyLog(GY_DEBUG_VERBOSE, "\tSheet size - %dx%d", tex->_coord->sheetWidth, tex->_coord->sheetHeight);
		_gyLog(GY_DEBUG_VERBOSE, "\tSprite size - %dx%d", tex->_coord->spriteWidth, tex->_coord->spriteHeight);
		_gyLog(GY_DEBUG_VERBOSE, "\t_refCount - %d", entry->_refCount);
        ++count;
    }
	_gyLog(GY_DEBUG_VERBOSE, "====/TEXTURE WALKTHROUGH====");
}

const GYTexCoord2* gyVidTexCoordArrayFromFrame(const GYTexCoordFrame *const frame, const GY_RECT_TEX_ORIENT orient) {
	switch(orient) {
		case GY_RECT_TEX_ORIENT_NORMAL:
			_texFrameCoords[0].u = frame->topLeft.u; _texFrameCoords[0].v = frame->topLeft.v;
			_texFrameCoords[1].u = frame->botRight.u; _texFrameCoords[1].v = frame->topLeft.v;
			_texFrameCoords[2].u = frame->topLeft.u; _texFrameCoords[2].v = frame->botRight.v;
			_texFrameCoords[3].u = frame->botRight.u; _texFrameCoords[3].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
			_texFrameCoords[0].u = frame->botRight.u; _texFrameCoords[0].v = frame->topLeft.v;
			_texFrameCoords[1].u = frame->topLeft.u; _texFrameCoords[1].v = frame->topLeft.v;
			_texFrameCoords[2].u = frame->botRight.u; _texFrameCoords[2].v = frame->botRight.v;
			_texFrameCoords[3].u = frame->topLeft.u; _texFrameCoords[3].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_VERT:
			_texFrameCoords[0].u = frame->topLeft.u; _texFrameCoords[0].v = frame->botRight.v;
			_texFrameCoords[1].u = frame->botRight.u; _texFrameCoords[1].v = frame->botRight.v;
			_texFrameCoords[2].u = frame->topLeft.u; _texFrameCoords[2].v = frame->topLeft.v;
			_texFrameCoords[3].u = frame->botRight.u; _texFrameCoords[3].v = frame->topLeft.v;
			break;
		case GY_RECT_TEX_ORIENT_ROT_90:
			_texFrameCoords[2].u = frame->topLeft.u; _texFrameCoords[2].v = frame->topLeft.v;
			_texFrameCoords[0].u = frame->botRight.u; _texFrameCoords[0].v = frame->topLeft.v;
			_texFrameCoords[3].u = frame->topLeft.u; _texFrameCoords[3].v = frame->botRight.v;
			_texFrameCoords[1].u = frame->botRight.u; _texFrameCoords[1].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_ROT_180:
			_texFrameCoords[3].u = frame->topLeft.u; _texFrameCoords[3].v = frame->topLeft.v;
			_texFrameCoords[2].u = frame->botRight.u; _texFrameCoords[2].v = frame->topLeft.v;
			_texFrameCoords[1].u = frame->topLeft.u; _texFrameCoords[1].v = frame->botRight.v;
			_texFrameCoords[0].u = frame->botRight.u; _texFrameCoords[0].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_ROT_270:
			_texFrameCoords[1].u = frame->topLeft.u; _texFrameCoords[1].v = frame->topLeft.v;
			_texFrameCoords[3].u = frame->botRight.u; _texFrameCoords[3].v = frame->topLeft.v;
			_texFrameCoords[0].u = frame->topLeft.u; _texFrameCoords[0].v = frame->botRight.v;
			_texFrameCoords[2].u = frame->botRight.u; _texFrameCoords[2].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
			_texFrameCoords[3].u = frame->topLeft.u; _texFrameCoords[3].v = frame->topLeft.v;
			_texFrameCoords[1].u = frame->botRight.u; _texFrameCoords[1].v = frame->topLeft.v;
			_texFrameCoords[2].u = frame->topLeft.u; _texFrameCoords[2].v = frame->botRight.v;
			_texFrameCoords[0].u = frame->botRight.u; _texFrameCoords[0].v = frame->botRight.v;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
			_texFrameCoords[0].u = frame->topLeft.u; _texFrameCoords[0].v = frame->topLeft.v;
			_texFrameCoords[2].u = frame->botRight.u; _texFrameCoords[2].v = frame->topLeft.v;
			_texFrameCoords[1].u = frame->topLeft.u; _texFrameCoords[1].v = frame->botRight.v;
			_texFrameCoords[3].u = frame->botRight.u; _texFrameCoords[3].v = frame->botRight.v;
			break;
		default:
			assert(0);
		}

	return _texFrameCoords;
}
