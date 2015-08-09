#include "tk_lua_wrappers.h"
#include "tk_lua_manager.h"
#include <asset/tk_lua_table_proxy.h>

void TkLuaManager::_registerWrappers(void) {
    elysian::LuaManagerBase::_registerWrappers();


    _mainState->register_class<estk>();
    _mainState->register_class_static<estk>("inspectTable", &estk_inspectTable);

    luaRegisterTkEnumsConstants(_mainState);
    TkLuaTableUserdata::staticInitialize();

}

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(estk)
EXPORT_OOLUA_FUNCTIONS_0_CONST(estk)
