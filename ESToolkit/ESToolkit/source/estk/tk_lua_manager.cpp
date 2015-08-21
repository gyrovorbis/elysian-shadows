#include "tk_lua_manager.h"

bool TkLuaManager::initialize(void) {
    bool success = true;
    success &= LuaManagerBase::initialize();

    success &= runScriptSandbox(TK_LUA_MANAGER_METATABLE_SCRIPT_PATH);
    return success;
}
