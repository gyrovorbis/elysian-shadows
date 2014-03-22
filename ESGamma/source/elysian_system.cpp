#include "elysian_system.hpp"

namespace elysian {

	System::ENGINE_STATE System::_engineState = System::ENGINE_NORMAL;
	bool System::_processInput = true;

	LevelSystem*		System::_levelSystem		= nullptr;
	MapSystem*			System::_mapSystem			= nullptr;
	CameraSystem*		System::_cameraSystem		= nullptr;
	AnimationSystem*	System::_animationSystem	= nullptr;
	CharacterSystem*	System::_characterSystem	= nullptr;
	PartySystem*		System::_partySystem		= nullptr;
	TimerSystem*		System::_timerSystem		= nullptr;
	WarpSystem*			System::_warpSystem			= nullptr;
	CollisionSystem*	System::_collisionSystem	= nullptr;
	RigidBodySystem*	System::_rigidBodySystem	= nullptr;
	GuiSystem*			System::_guiSystem			= nullptr;
	ExaminableSystem*	System::_examinableSystem	= nullptr;
	AudioSystem*		System::_audioSystem		= nullptr;
	LightSystem*		System::_lightSystem		= nullptr;
	ParticleSystem*		System::_particleSystem		= nullptr;
	DaySystem*			System::_daySystem			= nullptr;
	DialogSystem*		System::_dialogSystem		= nullptr;
	BehaviorSystem*		System::_behaviorSystem		= nullptr;

}
