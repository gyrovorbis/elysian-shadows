#include "gyro.h"
#include "elysian_font.hpp"
#include "elysian_debug.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_system.hpp"

namespace elysian {

bool Font::loadTextData(const char* const img_file, int sheet_width, int sheet_height, int sprite_width, int sprite_height, const char* const width_file) {
	bool success = true;

    Debug::printf(Debug::VERBOSE, "Font::Load(%s, %s)", img_file, width_file);
    Debug::push();

    //_transform.setSize(16.0f, 32.0f);
    _transform.setSize(sprite_width, sprite_height);

	//CHECK IF TEXTURE IS EVER LOADED
	if(!(_tex = gyVidTexCreateSheetUniform(img_file, sheet_width, sheet_height, sprite_width, sprite_height))) {
		Debug::printf(Debug::CRITICAL, "Font::Load(%s): Unable to load sheet texture!", img_file);
		success = false;
	}

	if(!_loadCharacterWidths(width_file)) {
		Debug::printf(Debug::CRITICAL, "Font::LoadCharacterWidths(%s): could not open file!", width_file);
		success = false;
	}

    Debug::pop();
	return success;
}

bool Font::_loadCharacterWidths(const char *width_file) {
	char *buffer, *ptr;
	size_t buffer_len;
	int ptr_inc = 0;
	GYFile *file;
	if (!gyFileOpen(width_file, "r", &file)) {
		return false;
	}
	if (!gyFileLength(file, &buffer_len)) {
		return false;
	}
	buffer = ptr = (char *) malloc(sizeof(char) * buffer_len + 1);
	if (!gyFileRead(file, buffer, buffer_len, NULL)) {
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

void Font::drawStringf(const FORMAT format, const ALIGNMENT align, const char *const str, ...) {
    char buffer[5000];
	va_list argptr;
	va_start(argptr,str);
	vsprintf(buffer,str,argptr);
	va_end(argptr);
	drawString(format, align, buffer);
}

void Font::drawString(const FORMAT format, const ALIGNMENT align, const char *const buffer) {
	int i;
    float currentX = _transform._pos.x;
    float currentY = _transform._pos.y;
	int temp = 0;

	if(align == C_ALIGN) {
        temp = getStringPixelWidth(buffer);
        currentX = (_transform._pos.x - (int)(temp/2.0f)) + 0.5f*_charWidth[buffer[0]];
    }
    else if(align == R_ALIGN) {
		for(i = 0; i < (int)strlen(buffer); ++i) {
            temp += _charWidth[(int)buffer[i]]+_spacing;
		}
        currentX = (_transform._pos.x - temp);
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
		_renderChar(format, currentX, currentY, buffer, i);
	}

    /*if(!_batching)*/ gyVidQuadListEnd();
}

//	void RenderCharacter(FONT_FORMAT format, float &currentX, float &currentY, char *str, int &i);

/* Modifies cur_x depending on character width (to support loops easier), returns the index of
 the next character to process */
void Font::_renderChar(const FORMAT /*format*/, float &current_x, float &current_y, const char *const str, int &i) {
	//if(format == FONT_FORMATTED) { // check for formatting characters
	switch (str[i]) {
		case '~': //special character sequence following
			++i;
			switch (str[i]) {
				case 'c': //change color
					//r = (int)(str[++i] - 48);
					//g = (int)(str[++i] - 48);
					//b = (int)(str[++i] - 48);
					break;
				case 'p': //pause--ignored by render
					++i;
					break;
			}
			break;
		case ' ': //don't bother rendering a space
			current_x += _charWidth[(int)str[i]];
			break;
		default: //render character
			//this->setFrame((int)str[i]);

			gyMatPush();
            _transform.setPos(current_x, current_y);
            _transform.apply();
			gyVidRect((int)str[i]);
			gyMatPop();

			//if(camera == WITH_CAMERA) cameraSystem->RenderSprite(*this);
			//else System::gyroVideo->RenderSprite(*this);

            current_x += _charWidth[(int)str[i]]+_spacing;
			break;
	}
	//	}
	++i;
	return;
}

unsigned int Font::getStringPixelWidth(const char *string) {
	unsigned int pixels = 0;
	unsigned int length = strlen(string);
	for(unsigned i = 0; i < length; ++i) {
        pixels += _charWidth[(int)string[i]]+_spacing;
	}
	return pixels;
}

}
