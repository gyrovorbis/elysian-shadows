#include <ESToolkit/AssetIO/tk_rigid_body.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/AssetIO/tk_entity.h>

bool TkRigidBody::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib.get(), "LOG_ERROR: RigidBody component has no attrib field.");
    return success;
}


QMenu* TkRigidBody::createContextMenu(void) {
    MenuStack menuStack("RigidBody");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? std::make_shared<elysian::RigidBodyAttrib>() : nullptr);
        },
        getAttrib().get(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeRigidBody();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();

}

bool TkRigidBody::canAddTo(const Entity &entity) const {
    return entity.getCollidable();
}

bool TkRigidBody::canRemoveFrom(const Entity &entity) const {
    return !entity.getCharacter();
}
