#include <tkRigidBody.h>
#include <tk_menu_stack.h>

bool TkRigidBody::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "ERROR: RigidBody component has no attrib field.");
    return success;
}


QMenu* TkRigidBody::createContextMenu(void) {
    MenuStack menuStack("RigidBody");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::RigidBodyAttrib : nullptr);
        },
        getAttrib()
    );

    return menuStack.first();

}

