#ifndef ELYSIAN_LUA_SINGLETON_HPP
#define ELYSIAN_LUA_SINGLETON_HPP

#include "elysian_system.hpp"
#include "elysian_asset_typedefs.hpp"

ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {

	class GameInput;
    class MouseInput;
	
	//THESE ARE FOR LUA, NOT YOU.
	class Singletons: public System {
        Singletons(void) = default;
		
        static Debug*       _debug;
        static GameInput*   _gameInput;
        static MouseInput*  _mouseInput;

	public:
        static void         setDebug(Debug *const dbg);
        static void         setGameInput(GameInput *const p_input);
        static void         setMouseInput(MouseInput*const mouse);
		
        static Debug*       getDebug(void);
        static GameInput*   getGameInput(void);
        static MouseInput*  getMouseInput(void);
		
	};














    // ===== INLINEZ =======
    inline Debug* Singletons::getDebug(void) { return _debug; }
    inline GameInput* Singletons::getGameInput(void) { return _gameInput; }
    inline void Singletons::setMouseInput(MouseInput*const mouse) { _mouseInput = mouse; }
    inline MouseInput* Singletons::getMouseInput(void) { return _mouseInput; }
    inline void Singletons::setDebug(Debug *const dbg) { _debug = dbg; }
    inline void Singletons::setGameInput(GameInput *const p_input) { _gameInput = p_input; }

	
}

#endif
