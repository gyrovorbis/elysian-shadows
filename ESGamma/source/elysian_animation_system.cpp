#include "elysian_animation_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_animation.hpp"
#include "elysian_area.hpp"
#include "elysian_debug.hpp"

namespace elysian {

void AnimationSystem::Update(void) {
	for(unsigned int i = 0; i < _curArea->animations.size(); ++i) {
		_curArea->animations[i]->update(getTimerSys()->getDeltaTime());
	}
}

void AnimationSystem::Render(void) {
	for(unsigned int i = 0; i < _curArea->animations.size(); ++i) {
		//Debug::Print(Debug::NORMAL, "Rendering this cunt - %s\n", _curArea->animation[i]->getEntity()->getName());
		//cameraSystem->RenderSprite(_curArea->animation[i]->gyroSprite);
	}
}

}