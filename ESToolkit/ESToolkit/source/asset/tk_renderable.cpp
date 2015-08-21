#include <tk_renderable.h>
#include <tk_menu_stack.h>
#include <QMenu>
#include <model/tk_property_item_delegate.h>

bool TkRenderable::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_spriteAttrib, "DBG_ERROR: Renderable component has no spriteAttrib.");

    return success;
}


QMenu* TkRenderable::createContextMenu(void) {
    MenuStack menuStack("Renderable");

    menuStack.insertCheckableAction("Sprite Attributes",
        [&](bool checked) {
            setSpriteAttrib(checked? new elysian::SpriteAttrib : nullptr);
        },
        getSpriteAttrib(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeRenderable();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.first();

}

void TkRenderable::initializeDelegates(void) {
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
}
