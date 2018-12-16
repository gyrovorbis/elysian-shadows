#include <ESToolkit/AssetIO/tk_collidable.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>

bool TkCollidable::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
            return false;
        }
        return true;
    };

    if(cAssert(_attrib.get(), "LOG_ERROR: Collidable component has no attrib field.")) {
        cAssert(_attrib->getColliderType() == GY_COLLIDER_AABB || _attrib->getColliderType() == GY_COLLIDER_OBB,
            "LOG_ERROR: Collidable component has invalid collider type!");

    }
    return success;
}

QMenu* TkCollidable::createContextMenu(void) {
    QESTkUtils::MenuStack menuStack("Collidable");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? std::make_shared<elysian::CollidableAttrib>() : nullptr);
        },
        getAttrib().get(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeCollidable();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();
}

bool TkCollidable::canRemoveFrom(const Entity &entity) const {
    return !entity.getRigidBody() && !entity.getWarp();
}

void TkCollidableAttrib::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* typeDelegate = new EnumDropdownPropertyItemDelegate({
       "COLLIDER_NONE", "COLLIDER_AABB", "COLLIDER_OBB", "COLLIDER_RTRI", "COLLIDER_OTHER_TRIS", "COLLIDER_CIR"
    });

    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, "colliderType");

    lua_pop(*_mainState, 1);
}

