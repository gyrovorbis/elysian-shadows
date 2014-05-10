#include "elysian_character_system.hpp"
#include "elysian_character.hpp"
#include "elysian_area.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"

namespace elysian {

CharacterSystem::CharacterSystem(void) {
	auto& proc = get<SUBPROC_WAYPOINT_POOL> ( static_cast<SubProcessManager<CharacterSystem,
											  WalkToWayPointPool>& >(*this));

	proc.start(this);
}

void CharacterSystem::update(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	Character::_deltaTime = deltaTime;

	updateSubProcesses(deltaTime);

	//Debug::Print(Debug::NORMAL, "Character amnt - %d\n", _curArea->character.size());
	for(unsigned int i = 0; i < _curArea->characters.size(); ++i) {
		_curArea->characters[i]->update();
		//_curArea->character[i]->getEntity()->transform.Scale(1.003f, 1.003f);
	}
}

}
