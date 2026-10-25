/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/AssetIO/tk_entity.h>
#include <ESToolkit/AssetIO/tk_character.h>
#include <QESTkUtils/tk_menu_stack.h>

using namespace QESTkUtils;


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


    return menuStack.takeTopLevel();
}

