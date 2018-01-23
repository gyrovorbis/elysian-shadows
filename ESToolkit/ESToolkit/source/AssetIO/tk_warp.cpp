#include <AssetIO/tk_warp.h>
#include <AssetIO/tk_entity.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <ESGamma/tk_lua_wrappers.h>

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


    return menuStack.takeTopLevel();
}

void TkWarp::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* areaDelegate = new FileBrowserPropertyItemDelegate;
    areaDelegate->setDir(true);
    OOLUA::push(*_mainState, areaDelegate);
    lua_setfield(*_mainState, -2, "areaPath");

    auto* levelDelegate = new FileBrowserPropertyItemDelegate;
    levelDelegate->setDir(true);
    OOLUA::push(*_mainState, levelDelegate);
    lua_setfield(*_mainState, -2, "levelPath");

    lua_pop(*_mainState, 1);
}
