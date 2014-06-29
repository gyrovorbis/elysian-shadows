#include "elysian_gui_system.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_font.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_day_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_light_system.hpp"
#include "elysian_rigid_body_system.hpp"
#include "elysian_rigid_body.hpp"

namespace elysian {

Font* MenuText::_fonts[2];

void MenuText::recursiveRender(void) {
	GYVector4 localVert = { _relPos.x, _relPos.y, 1.0f, 1.0f };
	GYVector4 worldVert;

	gyMatMultVec(&localVert, &worldVert, 1);

	gyMatPush();
	gyMatLoadIdentity();

	_fonts[_type]->transform.setPos(worldVert.x, worldVert.y);
	_fonts[_type]->transform.setPosZ(worldVert.z);
	_fonts[_type]->transform.setScale(1.0f, 1.0f);
	_fonts[_type]->setColor(_color);
	_fonts[_type]->drawString(Font::NO_FORMAT, _align, _text);

	gyMatPop();

}

MenuEntity::~MenuEntity(void) {
	//recursively delete children
	for(auto it = _childWidgets.begin(); it < _childWidgets.end(); ++it) delete *it;
}

void MenuEntity::recursiveRender(void) {
	GYVector4 localVert = { transform._pos.x, transform._pos.y, transform._pos.z, 1.0f };
	GYVector4 worldVert;

	if(_sizeType == SIZE_ABSOLUTE) {
		gyMatMultVec(&localVert, &worldVert, 1);
		gyMatPush();
		gyMatLoadIdentity();
		gyMatTranslate(worldVert.x-transform._pos.x, worldVert.y-transform._pos.y, worldVert.z-transform._pos.z);
	}

	Entity::render();

	gyMatPush();
	Entity::transform.apply();
	for(auto it = _childWidgets.begin(); it < _childWidgets.end(); ++it) (*it)->recursiveRender();
	gyMatPop();

	if(_sizeType == SIZE_ABSOLUTE) gyMatPop();
}

const GYVector2 GuiSystem::_combatTextGravity = ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY;

GuiSystem::GuiSystem(void): 
	_projWidth(0), 
	_projHeight(0) 
{
	gyVidResolution(&_projWidth, &_projHeight);

}

bool GuiSystem::loadAssets(void) {
	bool success = true;
	
	Debug::printf(Debug::VERBOSE, "GuiSystem::LoadAssets()");
    Debug::push();

	success &= twcen.loadTextData(ELYSIAN_FONT_IMAGE_PATH_TWCEN, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_TWCEN);
	success &= verdana.loadTextData(ELYSIAN_FONT_IMAGE_PATH_VERDANA, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_VERDANA);

	MenuText::_fonts[0] = &twcen;
	MenuText::_fonts[1] = &verdana;

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

		gyVec2AddAccum(&(*it).pos, &(*it).vel);
		gyVec2AddAccum(&(*it).vel, &_combatTextGravity);
		(*it).timer += deltaTime;
		(*it).color.a = 255.0f - ((*it).timer/ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE)*255.0f;

		if((*it).timer >= ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE) {
			_combatText.remove(it);
		}
	}
}

void GuiSystem::renderBackground(void) {
	_renderMenuWidget(_bgMenuWidget);
}

void GuiSystem::renderForeground(void) {
	adventureLog.render(twcen);
	_renderDebugOverlay();
	_renderMenuWidget(_fgMenuWidget);
}

void GuiSystem::_printDebugLine(const char *title, const char *str, ...) {
	char buffer[100];
	va_list argptr;
	va_start(argptr,str);
	vsprintf(buffer,str,argptr);
	va_end(argptr);

	twcen.transform._pos.y += 25.0f;
	twcen.transform._pos.x = _projWidth-200.0f;
	twcen.drawStringf(Font::NO_FORMAT, Font::R_ALIGN, title);
	twcen.transform._pos.x = _projWidth-185.0f;
	twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, buffer);
}

void GuiSystem::_renderDebugOverlay(void) {
	if(!_debugOverlayEnabled) return;

	twcen.setColor(0, 255, 100, 255);
	twcen.transform._pos.y = 100.0f;
	twcen.transform._pos.z = 50.0f;

	twcen.startBatchRender();

	_printDebugLine("FPS:", "%.2f", getTimerSys()->getFramerate());
	_printDebugLine("Verts:", "%d", gyVidVertexRenderCount());
	_printDebugLine("Uniform Uploads:", "%d", getLightSys()->getShaderUniformChangesLastFrame());
	_printDebugLine("Textures:", "%d", gyVidTexAmount());
	_printDebugLine("Texture MBytes:", "%f", gyVidTexBytes()/1024.0f/1024.0f);
	twcen.transform._pos.y += 25.0f;
	_printDebugLine("Onscreen Lights:", "%d", getLightSys()->getOnscreenLightCount());
	_printDebugLine("Entities:", "%d", getCurArea()->entities.size());
	_printDebugLine("Contacts:", "%d", getCollisionSys()->getContactCount());
	_printDebugLine("Pen Res Iter:", "%d", getCollisionSys()->getPenResIterUsed());
	_printDebugLine("Vel Res Iter:", "%d", getRigidBodySys()->getVelResIterUsed());
	twcen.transform._pos.y += 25.0f;
	_printDebugLine("Lua Scripts:", "%d", LuaManager::getScriptCount());
	_printDebugLine("Lua Script Instances:", "%d", LuaManager::getScriptInstances());
	_printDebugLine("Lua MBytes:", "%f", LuaManager::getKbytesUsed()/1024.0f);
	twcen.transform._pos.y += 25.0f;
	_printDebugLine("Game Time:", "%d:%d:%d", getDaySys()->getDays(), getDaySys()->getHours(), getDaySys()->getMinutes());
	if(getPartySys()->getPartyLeader()) {
		_printDebugLine("player.pos:", "<%.2f, %.2f, %.2f>",
						getPartySys()->getPartyLeader()->transform._pos.x,
						getPartySys()->getPartyLeader()->transform._pos.y,
						getPartySys()->getPartyLeader()->transform._pos.z);
        _printDebugLine("player.resting:", "%d",
                        getPartySys()->getPartyLeader()->getCollidable()->getRestingOnBody());
        _printDebugLine("player.contacts:", "%d",
                        getPartySys()->getPartyLeader()->getCollidable()->getContactCount());
	}

	twcen.endBatchRender();

}


void GuiSystem::render(void) {
	for(auto it = _combatText.begin(); it < _combatText.end(); ++it) {
		twcen.transform._pos.x = (*it).pos.x;
		twcen.transform._pos.y = (*it).pos.y;
		twcen.transform._pos.z = 50.0f;

		twcen.setColor((*it).color);
		twcen.setAlpha((*it).color.a);

		twcen.drawString(Font::NO_FORMAT, Font::L_ALIGN, (*it).text);
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

void GuiSystem::_renderMenuWidget(MenuWidget* widget) {
	if(!widget) return;

	gyMatPush();
	gyMatLoadIdentity();
	gyMatTranslate(_projWidth*0.5f, _projHeight*0.5f, 50.0f);
	gyMatScale(_projWidth, _projHeight, 1.0f);

	widget->recursiveRender();

	gyMatPop();

}




}
