#ifndef ELYSIAN_LUA_SINGLETON_HPP
#define ELYSIAN_LUA_SINGLETON_HPP

#include "elysian_system.hpp"

namespace elysian {

	class GameInput;
	class Debug;
	
	//THESE ARE FOR LUA, NOT YOU.
	class Singletons: public System {
		Singletons(void) {}
		
		static Debug		*_debug;
		static GameInput	*_gameInput;

	public:
		static void setDebug(Debug *const dbg);
		static void setGameInput(GameInput *const p_input);
		
		static Debug *getDebug(void) { return _debug; }
		static GameInput *getGameInput(void) { return _gameInput; }
		
	};
	
}

#endif