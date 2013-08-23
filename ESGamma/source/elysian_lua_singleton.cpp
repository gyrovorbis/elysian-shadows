#include "elysian_lua_singleton.hpp"

namespace elysian {

Debug *Singletons::_debug = NULL;
GameInput *Singletons::_gameInput = NULL;

void Singletons::setDebug(Debug *const dbg) { _debug = dbg; }
void Singletons::setGameInput(GameInput *const p_input) { _gameInput = p_input; }

}

