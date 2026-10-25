/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/AssetIO/tk_warp.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <ESGamma/system/elysian_warp_system.hpp>
#include <QApplication>

bool TkWarp::canAddTo(const Entity &entity) const {
    return entity.getCollidable();
}

QMenu* TkWarp::createContextMenu(void) {
    QESTkUtils::MenuStack menuStack("Warp");

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeWarp();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();
}

void TkWarp::initializeDelegates(void) {

    DynamicContentPropertyItemDelegate* previewWarpDelegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {

            TkWarp* warp = luaFieldItem->getVariant().getValue<TkWarp*>();
            if(warp) {
                QString str = "[";
                if(warp->getChangeLevel()) {
                    str += QString(warp->getLevelPath()).split("/").last();
                    str += "/";
                }
                if(warp->getChangeArea()) {
                    str += QString(warp->getAreaPath()).split("/").last();
                    str += ": ";
                }
                str += QString("<%1, %2, %3>").arg(warp->getToPosVec().getX())
                                              .arg(warp->getToPosVec().getY())
                                              .arg(warp->getToPosVec().getZ());
                str += "]";

                option.text = str;
                option.palette.setColor(QPalette::Text, QColor(Qt::gray));
            }
        }
    });
    previewWarpDelegate->setReadOnly();
    previewWarpDelegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), [](ESTkStandardItem* item) {
        if(elysian::Singletons::getEngineState() == elysian::System::ENGINE_ESTK_STOP) {
            QMessageBox::warning(nullptr, "Could not Preview Warp", "The engine must be running to evaluate warps!");
            return;
        }

        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            TkWarp* warp = luaFieldItem->getVariant().getValue<TkWarp*>();
            if(warp && warp->getEntity()) {
                TkCollidable* collidable = warp->getEntity()->getCollidable();
                TkEntity* leader = elysian::System::getPartySys()->getPartyLeader();

                if(collidable && leader) {
                    //Force it to give a shit about this warp...
                    elysian::Singletons::getWarpSys()->setState(elysian::WarpSystem::CHECK_WARP);
                    leader->getTransform().setPos(collidable->getCenterWorldPos());
                }
            }
        }
    }, "Take Warp");

    OOLUA::push(*_mainState, previewWarpDelegate);
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_OUTER_DELEGATE_FIELD_KEY);

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

    DynamicContentPropertyItemDelegate* toPosDelegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {

            TkWarp* warp = luaFieldItem->getTableRef().getValue<TkWarp*>();
            if(warp) {
                QString str = QString("<%1, %2, %3>").arg(warp->getToPosVec().getX())
                                              .arg(warp->getToPosVec().getY())
                                              .arg(warp->getToPosVec().getZ());

                option.text = str;
                option.palette.setColor(QPalette::Text, QColor(Qt::gray));
            }
        }
    });
    toPosDelegate->setReadOnly();
    toPosDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_POSITION_PATH), [](ESTkStandardItem* item) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            TkWarp* warp = luaFieldItem->getTableRef().getValue<TkWarp*>();
            if(warp) {
                TkEntity* leader = elysian::System::getPartySys()->getPartyLeader();

                if(leader) {
                    warp->setToPosVec(leader->getTransform().getPos());
                }
            }
        }
    }, "Use party leader's location.");

    OOLUA::push(*_mainState, toPosDelegate);
    lua_setfield(*_mainState, -2, "toPos");

    lua_pop(*_mainState, 1);
}
