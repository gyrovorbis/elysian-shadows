#include "gyro_texture.h"
#include "ios_image.h"
#include <stdlib.h>

int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned heith) {
    unsigned success = 0;

    switch(tex->bpp) {
    case GY_TEX_16BPP:
        *(GLuint*)(&tex->_data) = create_texture_png(tex->filename, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, &success);
        break;
    case GY_TEX_8BPP: //8bpp paletted texture

        break;
    case GY_TEX_4BPP: //4bpp paletted texture

        break;
    }
    
    return success;
}

int _gyVidTexFree(GYTexture *const tex) {
    glDeleteTextures(1, (GLuint*)&tex->_data);
    return 1;
}


