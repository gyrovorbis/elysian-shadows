
#include "elysian_textured_attrib.hpp"
#include <gyro_texture_api.h>

namespace elysian {

TexturedAttrib::~TexturedAttrib(void) {
    if(_tex) {
        gyVidTexDestroy(_tex);
    }
}

//Gonna shit their pants if there is no texture allocated yet...
void TexturedAttrib::getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
void TexturedAttrib::getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
const char* TexturedAttrib::getTexPath(void) const { return _tex->filename; }

void TexturedAttrib::loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
    if(_tex) {

        gyVidTexDestroy(_tex);

    }

    _tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);

}


void TexturedAttrib::loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
        if(_tex) {
        gyVidTexDestroy(_tex);

    }

    _tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);


}

}
