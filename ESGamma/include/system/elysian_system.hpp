#ifndef ELYSIAN_SYSTEM_HPP
#define ELYSIAN_SYSTEM_HPP

#include <level/elysian_level_manager.hpp>

ELYSIAN_ASSET_TYPEDEF(TimerSystem)
ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {
	class LevelSystem;
	class CameraSystem;
	class AnimationSystem;
	class CharacterSystem;
	class PartySystem;
	class WarpSystem;
	class CollisionSystem;
	class GuiSystem;
	class ExaminableSystem;
	class AudioSystem;
	class BehaviorSystem;
	class AudioSystem;
	class LightSystem;
	class ParticleSystem;
	class DaySystem;
	class DialogSystem;
	class ExaminableSystem;
	class RigidBodySystem;
	class CombatSystem;
    class SkeletonSystem;
    class MapSystem;

	class System: public LevelManager {
	public:
        enum ENGINE_STATE { ENGINE_NORMAL, ENGINE_WARPING, ENGINE_CONVERSATION, ENGINE_MODAL_CONVERSATION, ENGINE_CAMERA_DEBUG, ENGINE_ESTK_STOP };
		friend class Engine;
        friend class DebugBase;

    private:
		static ENGINE_STATE _engineState;
        static bool         _processInput;

		static LevelSystem		*_levelSystem;
		static MapSystem		*_mapSystem;
		static CameraSystem		*_cameraSystem;
		static AnimationSystem	*_animationSystem;
		static CharacterSystem	*_characterSystem;
		static PartySystem      *_partySystem;
		static TimerSystem		*_timerSystem;
		static WarpSystem		*_warpSystem;
		static CollisionSystem	*_collisionSystem;
		static RigidBodySystem	*_rigidBodySystem;
		static GuiSystem		*_guiSystem;
		static ExaminableSystem	*_examinableSystem;
		static AudioSystem		*_audioSystem;
		static LightSystem		*_lightSystem;
		static ParticleSystem	*_particleSystem;
		static DaySystem		*_daySystem;
		static DialogSystem		*_dialogSystem;
		static CombatSystem		*_combatSystem;
        static SkeletonSystem   *_skeletonSystem;
		static BehaviorSystem	*_behaviorSystem;

	public:
		static LevelSystem*			getLevelSys(void) { return _levelSystem; }
		static MapSystem*			getMapSys(void) { return _mapSystem; }
		static CameraSystem*		getCameraSys(void) { return _cameraSystem; }
		static AnimationSystem*		getAnimSys(void) { return _animationSystem; }
		static CharacterSystem*		getCharSys(void) { return _characterSystem; }
		static PartySystem*			getPartySys(void) { return _partySystem; }
		static TimerSystem*			getTimerSys(void) { return _timerSystem; }
		static WarpSystem*			getWarpSys(void) { return _warpSystem; }
		static CollisionSystem*		getCollisionSys(void) { return _collisionSystem; }
		static RigidBodySystem*		getRigidBodySys(void) { return _rigidBodySystem; }
		static GuiSystem*			getGuiSys(void) { return _guiSystem; }
		static ExaminableSystem*	getExamSys(void) { return _examinableSystem; }
		static AudioSystem*			getAudioSys(void) { return _audioSystem; }
		static LightSystem*			getLightSys(void) { return _lightSystem; }
		static ParticleSystem*		getPartSys(void) { return _particleSystem; }
		static DaySystem*			getDaySys(void) { return _daySystem; }
		static DialogSystem*		getDialogSys(void) { return _dialogSystem; }
		static CombatSystem*		getCombatSys(void) { return _combatSystem; }
        static SkeletonSystem*      getSkeletonSys(void) { return _skeletonSystem; }
		static BehaviorSystem*		getBehavSys(void) { return _behaviorSystem; }

        static bool         getProcessInput(void) { return _processInput; }
        static void         setProcessInput(const bool in=true) { _processInput = in; }
		static ENGINE_STATE getEngineState(void) { return _engineState; }
        static void         setEngineState(const ENGINE_STATE state) { _engineState = state; }

		//virtual void DebugDump()=0;
	};
}


#endif
