#include <asset/tk_examinable.h>
#include <model/tk_property_item_delegate.h>
#include <estk/tk_lua_wrappers.h>

void TkExaminable::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* dirDelegate = new EnumDropdownPropertyItemDelegate({
        "DIR4_CENTER", "DIR4_UP", "DIR4_DOWN", "DIR4_LEFT", "DIR4_RIGHT"
    });
    OOLUA::push(*_mainState, dirDelegate);
    lua_setfield(*_mainState, -2, "relativeDir");

    lua_pop(*_mainState, 1);
}
