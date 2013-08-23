#include "elysian_lua_script.hpp"
#include <cstring>

namespace elysian {
	
	LuaScript::LuaScript(const char *path): _refCount(1), _environRef(0), _error(false) { strcpy(_path, path); }
	LuaScript::~LuaScript(void) { /* set env table to nil (handled at higher layer), invoke gc manually? */ }


}