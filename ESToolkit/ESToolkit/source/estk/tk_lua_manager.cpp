#include "tk_lua_manager.h"

QStringList TkLuaManager::_udTypeNames;


bool TkLuaManager::initialize(void) {
    bool success = true;
    _udTypeNames.clear();
    success &= LuaManagerBase::initialize();

    success &= runScriptSandbox(TK_LUA_MANAGER_METATABLE_SCRIPT_PATH);

    //Put the bitches in alphabetical order!
    _udTypeNames.sort();

    return success;
}
