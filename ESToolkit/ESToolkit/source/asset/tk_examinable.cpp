#include <asset/tk_examinable.h>
#include <model/tk_property_item_delegate.h>
#include <estk/tk_lua_wrappers.h>

void TkExaminable::initializeDelegates(void) {
    auto* dirDelegate = new EnumDropdownPropertyItemDelegate({
        "DIR4_CENTER", "DIR4_UP", "DIR4_DOWN", "DIR4_LEFT", "DIR4_RIGHT"
    });
    OOLUA::push(*_mainState, dirDelegate);
    lua_setfield(*_mainState, -2, "relativeDir");
}
