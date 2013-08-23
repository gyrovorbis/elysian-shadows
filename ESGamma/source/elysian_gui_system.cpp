#include "elysian_gui_system.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_font.hpp"
#include "elysian_debug.hpp"

namespace elysian {

GuiSystem::GuiSystem(void): 
	_projWidth(0), 
	_projHeight(0) 
{}

bool GuiSystem::loadAssets(void) {
	bool success = true;
	
	Debug::printf(Debug::VERBOSE, "GuiSystem::LoadAssets()");
    Debug::push();
    
	success &= twcen.load(gyFilePath(ELYSIAN_FONT_IMAGE_PATH_TWCEN), 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_TWCEN);
	//success &= verdana.load(gyFilePath(ELYSIAN_FONT_IMAGE_PATH_VERDANA), 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_VERDANA);

	gyVidResolution(&_projWidth, &_projHeight);

    Debug::pop();
	return success;
}

void GuiSystem::update(void) {
	gyVidResolution(&_projWidth, &_projHeight);
	adventureLog.update();
}

void GuiSystem::renderOverlay(void) {
	adventureLog.render(twcen);
}

}