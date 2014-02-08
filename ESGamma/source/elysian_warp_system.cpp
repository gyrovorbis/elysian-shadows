#include "elysian_warp_system.hpp"
#include "elysian_area.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_warp.hpp"
#include "elysian_party_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_collidable.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_level_system.hpp"
#include "elysian_instance_transform.hpp"
#include "elysian_entity.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_zcoords.hpp"

namespace elysian {

WarpSystem::WarpSystem(void): 
	_currentWarp()
{
	_screenRect.transform._pos.z = ZCoordConstants::warpOverlay;
	_screenRect.setColor(0, 0, 0, 0);
}

void WarpSystem::Render(void) {
	//glDisable(GL_DEPTH_TEST);
	gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
	_screenRect.render(); 

}

void WarpSystem::Update(void) {
	unsigned width, height;

	//Update overlay size to match the current screen resolution
	getGuiSys()->getResolution(width, height);
	_screenRect.transform.setPos(width/2.0f, height/2.0f);
	_screenRect.transform.setSize(width, height);
	
	switch(_state) {
		case CHECK_WARP:
			UpdateCheckWarp();
			break;
		case FADE_OUT:
			UpdateFadeOut();
			break;
		case WARP:
			if(_currentWarp._changeLevel) getLevelSys()->switchLevel(_currentWarp._levelPath);
			if(_currentWarp._changeArea) getLevelSys()->switchArea(_currentWarp._areaPath);
			getPartySys()->getPartyLeader()->transform.setPos(_currentWarp._toPosition);
			getPartySys()->getPartyLeader()->setElev(_currentWarp._toElev);
			_state = FADE_IN;
			break;
		case FADE_IN:
			UpdateFadeIn();
			break;
		case PAUSE:
			UpdatePause();
			break;
		default:
			break;
	}
}

void WarpSystem::UpdateFadeOut(void) {
	if(_timer < 1.0f) {
		_timer += getTimerSys()->getDeltaTime();
		if(_timer > 1.0f) _timer = 1.0f;
		_screenRect.setColor(0.0f, 0.0f, 0.0f, _timer*255);
	}
	else {
		_timer = 1.0f;
		_state = WARP;
	}
}

void WarpSystem::UpdateFadeIn(void) {
	if(_timer > 0.0f) {
		_timer -= 1.0f*getTimerSys()->getDeltaTime();
		if(_timer < 0.0f) _timer = 0.0f;
		_screenRect.setColor(0.0f, 0.0f, 0.0f, _timer*255);
	}
	else {
		_timer = 0.0f;
		_state = PAUSE;
	}
}

void WarpSystem::UpdatePause(void) {
	
	Collidable *playerCollider = getPartySys()->getPartyLeader()->getCollidable();
	for(unsigned int i = 0; i < _curArea->warps.size(); ++i) {
		if(getCollisionSys()->checkCollision(*playerCollider, *_curArea->warps[i]->getEntity()->getCollidable())) {
			return;
		}
	}      
	//Debug::printf(Debug::VERBOSE, "CHANGING STATE!");
	_state = CHECK_WARP;
	setEngineState(System::ENGINE_NORMAL);
}

void WarpSystem::UpdateCheckWarp(void) {
	//Check for warp
	//Debug::printf(Debug::VERBOSE, "WARP CHECK");
	if(!getPartySys()->getPartyLeader()) return; //before we dereference it
	Collidable *playerCollider = getPartySys()->getPartyLeader()->getCollidable();

	for(unsigned int i = 0; i < _curArea->warps.size(); ++i) {
		if(getCollisionSys()->checkCollision(*playerCollider, *_curArea->warps[i]->getEntity()->getCollidable())) {
			//Debug::printf(Debug::VERBOSE, "Warp Collision! %s", _curArea->warps[i]->getEntity()->getName());
			_currentWarp = *_curArea->warps[i];
			_state = FADE_OUT;
			setEngineState(System::ENGINE_WARPING);
		} 
	}

}

void WarpSystem::setRenderWarps(const bool flag) { 
	_renderWarps = flag;
}

bool WarpSystem::getRenderWarps(void) const {
	return _renderWarps;
}

void WarpSystem::Disable(void) { _enabled = false; }
void WarpSystem::Enable(void) { _enabled = true; }

}
