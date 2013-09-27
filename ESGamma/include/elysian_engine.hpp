#ifndef ELYSIAN_ENGINE_HPP
#define ELYSIAN_ENGINE_HPP

#include "elysian_game_input.hpp"
#include "elysian_system.hpp"

namespace elysian {
	
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
