#include <tk_renderable.h>
#include <tk_menu_stack.h>
#include <QMenu>

bool TkRenderable::validate(QString &str) const {
    return true;
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

    return menuStack.first();

}
