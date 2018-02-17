#include <AssetIO/tk_color.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_color_swatch_property_item_delegate.h>
#include <ESGamma/tk_lua_wrappers.h>

void TkColor4::initializeDelegates(void) {
    auto* typeDelegate = new QESTkUtils::ColorSwatchPropertyItemDelegate<Color4>();

    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_OUTER_DELEGATE_FIELD_KEY);

    auto* spinBox = new SpinboxPropertyItemDelegate(0, 255);
    LuaVariant fieldDelegate = {
        { "r", spinBox },
        { "g", spinBox },
        { "b", spinBox },
        { "a", spinBox }
    };
    fieldDelegate.push();
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

}
