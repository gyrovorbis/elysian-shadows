#ifndef TK_LUAWRAPPERS_H
#define TK_LUAWRAPPERS_H


#include <oolua.h>
#include <ESToolkit/ui/tk_estoolkit.h>
#include <lua/elysian_lua_singleton.hpp>
#include <AssetIO/tk_area.h>
#include <QESTkUtils/ui/delegate/tk_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_spinbox_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_combobox_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_color_swatch_property_item_delegate.h>


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

OOLUA_PROXY(ComboBoxPropertyItemDelegate, PropertyItemDelegate)
OOLUA_PROXY_END

OOLUA_PROXY(DynamicContentPropertyItemDelegate, PropertyItemDelegate)
OOLUA_PROXY_END

OOLUA_PROXY(ColorSwatchPropertyItemDelegate, PropertyItemDelegate)
OOLUA_PROXY_END


static inline void luaRegisterTkEnumsConstants(OOLUA::Script */*s*/) {

}





#endif // TK_LUAWRAPPERS_H
