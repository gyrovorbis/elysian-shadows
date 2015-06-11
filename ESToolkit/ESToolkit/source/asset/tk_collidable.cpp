#include <tk_collidable.h>
#include <tk_menu_stack.h>
#include <QMenu>


bool TkCollidable::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "DBG_ERROR: Collidable component has no attrib field.");
    cAssert(_collider.col.type == GY_COLLIDER_AABB || _collider.col.type == GY_COLLIDER_OBB,
            "DBG_ERROR: Collidable component has invalid collider type!");

    return success;

}

QMenu* TkCollidable::createContextMenu(void) {
    MenuStack menuStack("Collidable");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::CollidableAttrib : nullptr);
        },
        getAttrib()
    );

    return menuStack.first();
}
