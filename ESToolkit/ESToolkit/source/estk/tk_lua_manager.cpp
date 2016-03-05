#include "tk_lua_manager.h"
#include <crtp/elysian_lua_variant.hpp>

QStringList TkLuaManager::_udTypeNames;
LuaVariant TkLuaManager::_serializeFunc;
unsigned TkLuaManager::_serializeDepth = 0;


bool TkLuaManager::initialize(void) {
    bool success = true;
    _udTypeNames.clear();
    success &= LuaManagerBase::initialize();

    success &= runScriptSandbox(TK_LUA_MANAGER_METATABLE_SCRIPT_PATH);

    pushGlobalField("serialize");
    _serializeFunc.pull();
    lua_register(*_mainState, "serialize", &_serializeCFunc);

    //Put the bitches in alphabetical order!
    _udTypeNames.sort();

    return success;
}

LuaVariant TkLuaManager::getParent(const LuaVariant &t) {
    return _callGlobalFunction<LuaVariant>("getparenttable", nullptr, t);
}

int TkLuaManager::_serializeCFunc(lua_State */*vm*/) {
    if(++_serializeDepth == 1) setSerializing(true);

    _serializeFunc.push();

    lua_insert(*_mainState, -2);
    _protectedCall(1, 1, nullptr);

    if(--_serializeDepth == 0) setSerializing(false);

    return 1;
}
