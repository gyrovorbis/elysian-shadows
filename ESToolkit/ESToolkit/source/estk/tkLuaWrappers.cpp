#include "tkLuaWrappers.h"
#include "tkLuaManager.h"

void TkLuaManager::_registerWrappers(void) {
    elysian::LuaManagerBase::_registerWrappers();


    _mainState->register_class<estk>();
    _mainState->register_class_static<estk>("inspectTable", &estk_inspectTable);

    luaRegisterTkEnumsConstants(_mainState);
}

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(estk)
EXPORT_OOLUA_FUNCTIONS_0_CONST(estk)
