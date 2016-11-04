#include "tk_lua_wrappers.h"
#include "tk_lua_manager.h"
#include <asset/tk_lua_table_proxy.h>
#include <proxy/elysian_entity_proxy.hpp>
//#include <model/tk_property_item_delegate.h>

static inline int Toolkit_findEntity(lua_State* const vm) {
    QString name;
    OOLUA::pull(vm, name);
    auto* entity = Debug::getCurrentArea()->findEntity(Q_CSTR(name));
    OOLUA::push(vm, entity);
    return entity? 1 : 0;
}

void TkLuaManager::_registerWrappers(void) {
    elysian::LuaManagerBase::_registerWrappers();

    _mainState->register_class<Toolkit>();
    _mainState->register_class<PropertyItemDelegate>();
    _mainState->register_class<EnumDropdownPropertyItemDelegate>();
    _mainState->register_class<SpinboxPropertyItemDelegate>();
    _mainState->register_class<FileBrowserPropertyItemDelegate>();
    _mainState->register_class_static<Toolkit>("setSelection", &Toolkit_setSelection);
    _mainState->register_class_static<Toolkit>("getSelection", &ESToolkit::pushSelection);
    _mainState->register_class_static<Toolkit>("findEntity", &Toolkit_findEntity);

    luaRegisterTkEnumsConstants(_mainState);
    TkLuaTableUserdata::staticInitialize();

}

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Toolkit)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Toolkit)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(PropertyItemDelegate)
EXPORT_OOLUA_FUNCTIONS_0_CONST(PropertyItemDelegate)

OOLUA_EXPORT_FUNCTIONS_CONST(EnumDropdownPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(EnumDropdownPropertyItemDelegate,
                       insertEnumValue)

OOLUA_EXPORT_FUNCTIONS_CONST(SpinboxPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(SpinboxPropertyItemDelegate,
                       setMaxValue,
                       setMinValue)

OOLUA_EXPORT_FUNCTIONS_CONST(FileBrowserPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(FileBrowserPropertyItemDelegate)
