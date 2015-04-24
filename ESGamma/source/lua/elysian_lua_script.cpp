#include "elysian_lua_script.hpp"
#include "elysian_lua_behavior.hpp"
#include <lua/lua.hpp>
#include <cstring>

namespace elysian {
	
LuaScript::LuaScript(const char *path): _environRef(LUA_NOREF) { strcpy(_path, path); }


void LuaBehaviorScript::cacheTriggers(lua_State*const state) {
    LuaManager::stackBalanceBegin();

    lua_rawgeti(state, LUA_REGISTRYINDEX, _environRef);

    for(unsigned i = 0; i < LuaBehavior::TRIGGER_MAX_INDEX; ++i) {
        if((_attrib.getSupportedTriggers()&(1ul<<i))) {
            lua_pushnumber(state, i);
            lua_getfield(state, -2, LuaBehavior::triggerStrings[i]);
            lua_settable(state, -3);
        }
    }

    lua_pop(state, 1);

    LuaManager::stackBalanceEnd();
}



}
