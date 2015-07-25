
#include <asset/tk_warp.h>
#include <asset/tk_entity.h>
#include <core/tk_menu_stack.h>

TkWarp::TkWarp(TkEntity &parent): elysian::WarpBase(parent) {}

bool TkWarp::canAddTo(const Entity &entity) const {
    return entity.getCollidable();
}

QMenu* TkWarp::createContextMenu(void) {
    MenuStack menuStack("Warp");

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeWarp();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.first();
}

