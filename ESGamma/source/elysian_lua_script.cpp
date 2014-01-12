#include "elysian_lua_script.hpp"
#include <lua/lua.hpp>
#include <cstring>

namespace elysian {
	
LuaScript::LuaScript(const char *path): _environRef(LUA_NOREF) { strcpy(_path, path); }


}
