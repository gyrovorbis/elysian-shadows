#include <asset/tk_collidable.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>


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
    cAssert(_attrib->getColliderType() == GY_COLLIDER_AABB || _attrib->getColliderType() == GY_COLLIDER_OBB,
            "DBG_ERROR: Collidable component has invalid collider type!");

    return success;

}

QMenu* TkCollidable::createContextMenu(void) {
    MenuStack menuStack("Collidable");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::CollidableAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    return menuStack.first();
}
