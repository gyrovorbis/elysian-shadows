#ifndef TK_LUAWRAPPERS_H
#define TK_LUAWRAPPERS_H


#include <oolua.h>
#include <estk/tk_estoolkit.h>
#include <lua/elysian_lua_singleton.hpp>
#include <asset/tk_area.h>
#include <model/tk_property_item_delegate.h>


namespace OOLUA
{




}

class Toolkit {};

OOLUA_PROXY(Toolkit)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END




OOLUA_PROXY(PropertyItemDelegate)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor,
        Abstract
    )
OOLUA_PROXY_END

OOLUA_PROXY(EnumDropdownPropertyItemDelegate, PropertyItemDelegate)
    OOLUA_MEM_FUNC(void, insertEnumValue, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(SpinboxPropertyItemDelegate, PropertyItemDelegate)
    OOLUA_MEM_FUNC(void, setMaxValue, int)
    OOLUA_MEM_FUNC(void, setMinValue, int)
OOLUA_PROXY_END

OOLUA_PROXY(FileBrowserPropertyItemDelegate, PropertyItemDelegate)
OOLUA_PROXY_END

static inline int Toolkit_setSelection(lua_State* const /*vm*/) {
    ESToolkit::select(LuaVariant::fromStack());
    return 0;
}

static inline void luaRegisterTkEnumsConstants(OOLUA::Script */*s*/) {

}





#endif // TK_LUAWRAPPERS_H
