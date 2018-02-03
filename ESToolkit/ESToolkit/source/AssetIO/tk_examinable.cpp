#include <AssetIO/tk_examinable.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <ESGamma/tk_lua_wrappers.h>
#include <AssetIO/tk_dialog.h>
#include <system/elysian_dialog_system.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <QApplication>

void TkExaminable::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* dirDelegate = new EnumDropdownPropertyItemDelegate({
        "DIR4_CENTER", "DIR4_UP", "DIR4_DOWN", "DIR4_LEFT", "DIR4_RIGHT"
    });

    OOLUA::push(*_mainState, dirDelegate);
    lua_setfield(*_mainState, -2, "relativeDir");


    DynamicContentPropertyItemDelegate* dialogDelegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto dialog = luaFieldItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
            if(dialog) {
                unsigned count = dialog->getScreenCount();
                option.palette.setColor(QPalette::Text, QColor(Qt::gray));
                option.text = QString("[%1 items]").arg(count);
            } else {
                option.palette.setColor(QPalette::Text, QColor(Qt::black));
                option.text = QString("nil");
            }
        }

    });

    dialogDelegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), [](ESTkStandardItem* item) {
        auto* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto dialog = luaFieldItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
            elysian::System::getDialogSys()->startDialog(dialog);
        }
    });

    dialogDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH), [](ESTkStandardItem* item) {
        auto* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto dialog = luaFieldItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
            if(!dialog) {
                auto* examinableItem = luaFieldItem->getAncestorLuaItem({"examinable"});
                if(examinableItem) {
                    auto* examinableComp = examinableItem->getVariant().getValue<TkExaminable*>();
                    examinableComp->setDialog(std::make_shared<TkDialog>());
                }
            }
        }
    });

    dialogDelegate->setDeleteCallback([](ESTkStandardItem* item) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto dialog = luaFieldItem->getVariant().getValue<std::shared_ptr<TkDialog>>();

            if(dialog) {
                auto* examinableItem = luaFieldItem->getAncestorLuaItem({"examinable"});
                if(examinableItem) {
                    auto* examinableComp = examinableItem->getVariant().getValue<TkExaminable*>();
                    if(examinableComp) {
                        examinableComp->setDialog(nullptr);
                    }
                    examinableItem->setDirty();
                }
            }
        }
    });


#if 0
    delegate->setContextMenuCallback([](ESTkStandardItem* item) -> QMenu* {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            TkSkeletonAttrib* attrib = luaFieldItem->getTableRef()["__ud"]->getValue<TkSkeletonAttrib*>();
            if(attrib) {
                unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;

                MenuStack stack("Animation");
                stack.insertAction("Delete", [=](bool) {
                    attrib->setAnimation(index, nullptr);

                    auto* attribItem = luaFieldItem->getAncestorSiblingLuaItem({"animations", "attrib"}, 1);
                    if(attribItem) {
                        attribItem->setDirty();
                    }

                });

                return stack.takeTopLevel();
            }
        }
        return nullptr;
    });
#endif
    OOLUA::push(*_mainState, dialogDelegate);
    lua_setfield(*_mainState, -2, "dialog");



    lua_pop(*_mainState, 1);
}
