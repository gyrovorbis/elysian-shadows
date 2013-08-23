#include "elysian_system.hpp"

namespace elysian {

	System::ENGINE_STATE System::_engineState = System::ENGINE_NORMAL;
	bool System::_processInput = true;

	LevelSystem*		System::_levelSystem = NULL;
	MapSystem*			System::_mapSystem = NULL;
	CameraSystem*		System::_cameraSystem = NULL;
	AnimationSystem*	System::_animationSystem = NULL;
	CharacterSystem*	System::_characterSystem = NULL;
	PartySystem*		System::_partySystem = NULL;
	TimerSystem*		System::_timerSystem = NULL;
	WarpSystem*			System::_warpSystem = NULL;
	CollisionSystem*	System::_collisionSystem = NULL;
	GuiSystem*			System::_guiSystem = NULL;
	ExaminableSystem*	System::_examinableSystem = NULL;
	BehaviorSystem*		System::_behaviorSystem = NULL;;

}