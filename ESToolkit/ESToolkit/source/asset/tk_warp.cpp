#include <asset/tk_warp.h>
#include <asset/tk_entity.h>
#include <core/tk_menu_stack.h>
#include <model/tk_property_standard_item.h>
#include <estk/tk_lua_wrappers.h>

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

void TkWarp::initializeDelegates(void) {
    auto* areaDelegate = new FileBrowserPropertyItemDelegate;
    areaDelegate->setDir(true);
    OOLUA::push(*_mainState, areaDelegate);
    lua_setfield(*_mainState, -2, "areaPath");

    auto* levelDelegate = new FileBrowserPropertyItemDelegate;
    levelDelegate->setDir(true);
    OOLUA::push(*_mainState, levelDelegate);
    lua_setfield(*_mainState, -2, "levelPath");
}
