#include <fstream>
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
	
	transform.setSize(16.0f, 32.0f);

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
	ifstream file(gyFilePath(width_file));
	if(!file.is_open()) return false;
	
	for(int i = 0; i < 128; ++i) {
		file >> _charWidth[i]; 
	}
	
	file.close();
	return true;
}

void Font::drawStringf(const FORMAT format, const ALIGNMENT align, const char *const str, ...) {
	char buffer[500];
	va_list argptr;  
	va_start(argptr,str); 
	vsprintf(buffer,str,argptr); 
	va_end(argptr); 
	drawString(format, align, buffer);
}

void Font::drawString(const FORMAT format, const ALIGNMENT align, const char *const buffer) {
	int i;
	float currentX = transform._pos.x;
	float currentY = transform._pos.y;
	int temp = 0;
	
	if(align == C_ALIGN) {
		temp = getStringPixelWidth(buffer);
		currentX = (transform._pos.x - (temp/2)-8);
	}
	else if(align == R_ALIGN) {
		for(i = 0; i < (int)strlen(buffer); ++i) {
			temp += _charWidth[(int)buffer[i]]+2;
		}
		currentX = (transform._pos.x - temp - 16);
	}
	
	i = 0;

	gyVidTexBind(_tex);
	if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
    else gyVidAlphaLevel(GY_ALPHA_FULL);
	gyVidQuadListBegin(&_color);

	while (buffer[i] != '\0') {
		//Iterates through the buffer, drawing characters
		_renderChar(format, currentX, currentY, buffer, i);
	}

	gyVidQuadListEnd();
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
			transform.setPos(current_x, current_y);
			transform.apply();
			gyVidRect((int)str[i]);
			gyMatPop();

			//if(camera == WITH_CAMERA) cameraSystem->RenderSprite(*this);
			//else System::gyroVideo->RenderSprite(*this);
			
			current_x += _charWidth[(int)str[i]]+2;
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
		pixels += _charWidth[(int)string[i]]+2;
	}
	return pixels;
}

}
