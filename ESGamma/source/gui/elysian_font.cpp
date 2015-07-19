#include <gyro_video_api.h>
#include <gyro_file_api.h>
#include <gyro_shader_api.h>
#include "elysian_font.hpp"
#include "elysian_debug.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_system.hpp"

namespace elysian {

GYColor4 Font::_colorPalette[ELYSIAN_FONT_MARKUP_COLOR_PALETTE_SIZE] = { GYColor4 { 255, 255, 255, 255 }, GYColor4 { 0, 0, 0, 0 } };

bool Font::loadTextData(const char*const imgFile, const int sheetWidth, const int sheetHeight, const int spriteWidth, const int spriteHeight, const char*const widthFile) {
	bool success = true;

    Debug::printf(Debug::DBG_VERBOSE, "Font::Load(%s, %s)", imgFile, widthFile);
    Debug::push();

    //_transform.setSize(16.0f, 32.0f);
    _transform.setSize((float)spriteWidth, (float)spriteHeight);

	//CHECK IF TEXTURE IS EVER LOADED
    if(!(_tex = gyVidTexCreateSheetUniform(imgFile, sheetWidth, sheetHeight, spriteWidth, spriteHeight))) {
        Debug::printf(Debug::DBG_WARNING, "Font::Load(%s): Unable to load sheet texture!", imgFile);
		success = false;
	}

    if(!_loadCharacterWidths(widthFile)) {
        Debug::printf(Debug::DBG_WARNING, "Font::LoadCharacterWidths(%s): could not open file!", widthFile);
		success = false;
	}

    Debug::pop();
	return success;
}

bool Font::_loadCharacterWidths(const char *widthFile) {
	char *buffer, *ptr;
	size_t buffer_len;
	int ptr_inc = 0;
	GYFile *file;

    if (!gyFileOpen(widthFile, "r", &file)) {
		return false;
	}
	if (!gyFileLength(file, &buffer_len)) {
		return false;
	}
	buffer = ptr = (char *) malloc(sizeof(char) * buffer_len + 1);
    if (!gyFileRead(file, buffer, buffer_len, nullptr)) {
		return false;
	}

    sscanf(ptr, "%hd%n", &_spacing, &ptr_inc);
    ptr += ptr_inc;

    for(int i = 1; i < 129; ++i) {
        sscanf(ptr, "%hd%n", &_charWidth[i-1], &ptr_inc);
		ptr += ptr_inc;
	}
	free(buffer);
	gyFileClose(&file);
	return true;
}

void Font::drawStringf(const ALIGNMENT align, const char *const str, ...) {
    char buffer[ELYSIAN_FONT_BUFFER_SIZE];
	va_list argptr;
	va_start(argptr,str);
	vsprintf(buffer,str,argptr);
	va_end(argptr);
    drawString(align, buffer);
}

void Font::drawString(const ALIGNMENT align, const char *const buffer) {
	int i;
    float currentX = _transform._pos.x;
    float currentY = _transform._pos.y;
	int temp = 0;

    auto getFirstIndex = [&]() {
        unsigned index = 0;
        if(buffer[0] == '~') {
            switch(buffer[1]) {
            case 'c':
                index = 4;
                break;
            case '~':
                index = 1;
                break;
            default:
                index = 2;
                break;
            }
        }
        return index;
    };

	if(align == C_ALIGN) {
        temp = getStringPixelWidth(buffer);
        currentX = (_transform._pos.x - (int)(temp/2.0f)) + 0.5f*_charWidth[(int)getFirstIndex()];
    }
    else if(align == R_ALIGN) {
        temp = getStringPixelWidth(buffer);
        currentX = (_transform._pos.x - temp) + 0.5f*_charWidth[(int)getFirstIndex()];
	}

	i = 0;

    //if(!_batching) {
		gyVidTexBind(_tex);
        gyVidNormalMapBind(nullptr);
        gyVidMaterialBind(nullptr);
        gyVidSpecularMapBind(nullptr);
		if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
		else gyVidAlphaLevel(GY_ALPHA_FULL);
		//gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
        gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
        //gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
		gyVidQuadListBegin(&_color);

    //}

	while (buffer[i] != '\0') {
		//Iterates through the buffer, drawing characters
        _renderChar(currentX, currentY, buffer, i);
	}

    /*if(!_batching)*/ gyVidQuadListEnd();
}

//	void RenderCharacter(FONT_FORMAT format, float &currentX, float &currentY, char *str, int &i);

/* Modifies cur_x depending on character width (to support loops easier), returns the index of
 the next character to process */
void Font::_renderChar(float &currentX, float &currentY, const char *const str, int &i) {
    bool renderChar = true;

	switch (str[i]) {
		case '~': //special character sequence following
            renderChar = false;
			++i;
			switch (str[i]) {
                case ELYSIAN_FONT_MARKUP_COLOR_CHAR: //change color
                    _processColorMarkup(str, i);

					break;
                case '~': //double tilde escape sequence for tilde
                    //--i;
                    renderChar = true;
                default: break;
			}
			break;
		case ' ': //don't bother rendering a space
            currentX += _charWidth[(int)str[i]];
            renderChar = false;
        default: break;
    }
			//this->setFrame((int)str[i]);
    if(renderChar) {
        gyMatPush();
        _transform.setPos(currentX, currentY);
        _transform.apply();
        gyVidRect((int)str[i]);
        gyMatPop();

        currentX += _charWidth[(int)str[i]] + _spacing;
    }
	++i;
}

unsigned int Font::getStringPixelWidth(const char *string) {
	unsigned int pixels = 0;
	unsigned int length = strlen(string);
	for(unsigned i = 0; i < length; ++i) {
        bool add = true;
        if(string[i] == '~') {
            ++i;
            add = false;
            switch(string[i]) {
            case ELYSIAN_FONT_MARKUP_COLOR_CHAR:
                i += 2;
                break;
            case '~':
                add = true;
            default: break;
            }
        } else if(string[i] == ' ') {
            pixels += _charWidth[(int)string[i]];
            add = false;
        }
        if(add) pixels += _charWidth[(int)string[i]]+_spacing;
    }
	return pixels;
}

}
