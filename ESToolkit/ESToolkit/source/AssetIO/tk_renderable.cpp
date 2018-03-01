#include <AssetIO/tk_renderable.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <QMenu>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <ESGamma/tk_lua_wrappers.h>

bool TkRenderable::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_spriteAttrib.get(), "LOG_ERROR: Renderable component has no spriteAttrib.");

    return success;
}


QMenu* TkRenderable::createContextMenu(void) {
    MenuStack menuStack("Renderable");

    menuStack.insertCheckableAction("Sprite Attributes",
        [&](bool checked) {
            setSpriteAttrib(checked? std::make_shared<elysian::SpriteAttrib>() : nullptr);
        },
        getSpriteAttrib().get(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeRenderable();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();

}

void TkRenderable::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* orientDelegate = new EnumDropdownPropertyItemDelegate({
        "RECT_TEX_ORIENT_NORMAL",
        "RECT_TEX_ORIENT_FLIP_HORIZ",
        "RECT_TEX_ORIENT_FLIP_VERT",
        "RECT_TEX_ORIENT_ROT_90",
        "RECT_TEX_ORIENT_ROT_180",
        "RECT_TEX_ORIENT_ROT_270",
        "RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90",
        "RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270"
    });
    OOLUA::push(*_mainState, orientDelegate);
    lua_setfield(*_mainState, -2, "texOrient");

    lua_pop(*_mainState, 1);
}
