#include <asset/tk_entity.h>
#include <asset/tk_character.h>
#include <core/tk_menu_stack.h>

bool TkCharacter::canAddTo(const Entity &entity) const {
    return entity.getCollidable() && entity.getRigidBody();
}

bool TkCharacter::canRemoveFrom(const Entity &entity) const {
    return !entity.getCombat();
}



QMenu* TkCharacter::createContextMenu(void) {
    MenuStack menuStack("Character");

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeCharacter();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.first();
}

