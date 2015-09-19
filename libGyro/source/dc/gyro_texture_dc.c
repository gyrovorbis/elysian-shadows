#include <gyro_texture_api.h>
//#include "shtexture.h"

#include <kos.h>
#include <png/png.h>

//typedef  pvr_ptr_t _GYTexPayload;

int _gyVidTexAlloc(GYTexture *const tex) {
#if 0
    texture_t *shtex;

/*    if(pvr_mem_stats() < tex->bytes) { //not enough VRAM
        return 0;
    }*/

    tex->_data = shtex = malloc(sizeof(texture_t));
    shtex->width = width;
    shtex->height = height;
    shtex->size = tex->bytes;


    switch(tex->bpp) {
    case GY_TEX_16BPP:
        shtex->vram_ptr = pvr_mem_malloc(width * height * 2);
        png_to_texture(tex->filename, shtex->vram_ptr, PNG_MASK_ALPHA);
        shtex->palette = NULL;
        shtex->format = TEXFMT_ARGB1555;
        shtex->flags = TEXFLAG_TWIDDLED;
        break;
    case GY_TEX_8BPP:
        //TVSPELS_LOADER
        break;
    case GY_TEX_4BPP:
        //TVSPELS_LOADER
        break;
    }

    return 1;
#endif
}

int _gyVidTexFree(GYTexture *const tex) {
#if 0
    pvr_mem_free(((texture_t*)tex->_data)->vram_ptr);
    free(tex->_data);
    return 1;
#endif
}

