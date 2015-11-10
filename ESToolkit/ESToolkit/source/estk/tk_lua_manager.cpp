#include "tk_lua_manager.h"
#include <crtp/elysian_lua_variant.hpp>

QStringList TkLuaManager::_udTypeNames;
LuaVariant TkLuaManager::_serializeFunc;
bool TkLuaManager::_serializing = false;


bool TkLuaManager::initialize(void) {
    bool success = true;
    _udTypeNames.clear();
    success &= LuaManagerBase::initialize();

    success &= runScriptSandbox(TK_LUA_MANAGER_METATABLE_SCRIPT_PATH);

    getGlobal("serialize");
    _serializeFunc.pull();
    lua_register(*_mainState, "serialize", &_serializeCFunc);

    //Put the bitches in alphabetical order!
    _udTypeNames.sort();

    return success;
}

int TkLuaManager::_serializeCFunc(lua_State */*vm*/) {
    _serializing = true;
    _serializeFunc.push();
    lua_insert(*_mainState, -2);
    _protectedCall(1, 1, nullptr);
    _serializing = false;
    return 1;
}
