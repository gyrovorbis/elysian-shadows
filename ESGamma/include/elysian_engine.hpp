#ifndef ELYSIAN_ENGINE_HPP
#define ELYSIAN_ENGINE_HPP

#include "elysian_game_input.hpp"
#include "elysian_system.hpp"

namespace elysian {

	class Debug;
	class MapSystem;
	class LevelSystem;
	class GameInput;
	class CameraSystem;
	class AnimationSystem;
	class CharacterSystem;
	class PartySystem;
	class TimerSystem;
	class WarpSystem;
	class CollisionSystem;
	class GuiSystem;
	class LuaSystem;
	class BehaviorSystem;
	
	class Engine: public System {
		friend class Debug;
	private:
		GYInput _platInput;
		GameInput _gameInput;
		
	public:
		Engine(void);
		~Engine(void);
		void update(void);
		bool loadAssets(void);
		void processInput(void);
		void render(void);
	};

}

#endif
