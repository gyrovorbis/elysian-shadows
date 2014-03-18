#include "elysian_gui_system.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_font.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"

namespace elysian {

const GYVector2 GuiSystem::_combatTextGravity = ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY;

GuiSystem::GuiSystem(void): 
	_projWidth(0), 
	_projHeight(0) 
{}

bool GuiSystem::loadAssets(void) {
	bool success = true;
	
	Debug::printf(Debug::VERBOSE, "GuiSystem::LoadAssets()");
    Debug::push();
    
	success &= twcen.loadTextData(gyFilePath(ELYSIAN_FONT_IMAGE_PATH_TWCEN), 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_TWCEN);
	success &= verdana.loadTextData(gyFilePath(ELYSIAN_FONT_IMAGE_PATH_VERDANA), 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_VERDANA);

	gyVidResolution(&_projWidth, &_projHeight);

    Debug::pop();
	return success;
}

void GuiSystem::update(void) {
	gyVidResolution(&_projWidth, &_projHeight);
	adventureLog.update();

	const float deltaTime = getTimerSys()->getDeltaTime();

	//update CombatText
	for(auto it = _combatText.begin(); it < _combatText.end(); ++it) {

		gyVec2AddAccum(&it->pos, &it->vel);
		gyVec2AddAccum(&it->vel, &_combatTextGravity);
		it->timer += deltaTime;
		it->color.a = 255.0f - (it->timer/ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE)*255.0f;

		if(it->timer >= ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE) {
			_combatText.remove(it);
		}
	}
}

void GuiSystem::renderOverlay(void) {
	adventureLog.render(twcen);
}

void GuiSystem::render(void) {
	for(auto it = _combatText.begin(); it < _combatText.end(); ++it) {
		twcen.transform._pos.x = it->pos.x;
		twcen.transform._pos.y = it->pos.y;
		twcen.transform._pos.z = 50.0f;

		twcen.setColor(it->color);
		twcen.setAlpha(it->color.a);

		twcen.drawString(Font::NO_FORMAT, Font::L_ALIGN, it->text);
	}
}

void GuiSystem::addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel) {

	CombatText* combat = &_combatText.claimFreeEntry();

	strcpy(combat->text, text);
	combat->color = color;
	combat->color.a = 255;
	combat->pos = pos;
	combat->vel = vel;
	combat->timer = 0.0f;

}





}
