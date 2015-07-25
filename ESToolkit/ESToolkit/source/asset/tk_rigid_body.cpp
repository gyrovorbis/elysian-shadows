#include <asset/tk_rigid_body.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>

bool TkRigidBody::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "DBG_ERROR: RigidBody component has no attrib field.");
    return success;
}


QMenu* TkRigidBody::createContextMenu(void) {
    MenuStack menuStack("RigidBody");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::RigidBodyAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    return menuStack.first();

}

bool TkRigidBody::canAddTo(const Entity &entity) const {
    return entity.getCollidable();
}

