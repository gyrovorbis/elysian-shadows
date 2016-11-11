#include <asset/tk_dialog_screen.h>
#include <delegate/tk_property_item_delegate.h>

void TkDialogScreen::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);


    OOLUA::push(*_mainState, new PropertyItemDelegate);
    lua_setfield(*_mainState, -2, "text");
    lua_pop(*_mainState, 1);

}
