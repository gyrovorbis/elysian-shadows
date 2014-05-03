#include "elysian_character_system.hpp"
#include "elysian_character.hpp"
#include "elysian_area.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"

namespace elysian {

void CharacterSystem::update(void) {
	Character::_deltaTime = getTimerSys()->getDeltaTime();

	//Debug::Print(Debug::NORMAL, "Character amnt - %d\n", _curArea->character.size());
	for(unsigned int i = 0; i < _curArea->characters.size(); ++i) {
		_curArea->characters[i]->update();
		//_curArea->character[i]->getEntity()->transform.Scale(1.003f, 1.003f);
	}
}

}
