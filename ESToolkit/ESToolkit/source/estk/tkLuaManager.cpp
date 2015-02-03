#include "tkLuaManager.h"

void TkLuaManager::initialize(void) {
    LuaManagerBase::initialize();

    runScriptSandbox(TK_LUA_MANAGER_METATABLE_SCRIPT_PATH);
}
