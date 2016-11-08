#include <asset/tk_skeleton.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>
#include <spine/AnimationState.h>
#include <estk/tk_lua_wrappers.h>
#include <proxy/elysian_entity_proxy.hpp>
#include <delegate/tk_file_browser_property_item_delegate.h>
#include <delegate/tk_dynamic_content_property_item_delegate.h>
#include <delegate/tk_combobox_property_item_delegate.h>
#include <model/tk_lua_table_stack_model.h>
#include <core/tk_menu_stack.h>
#include <QStyleOptionViewItem>
#include <QApplication>

void TkSkeleton::onAtlasChanged(void) {
    setAttrib(_attrib);
}

void TkSkeleton::onJsonChanged(void) {
    setAttrib(_attrib);
}

bool TkSkeleton::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "LOG_ERROR: Skeleton component has no attrib field.");
    return success;
}


QMenu* TkSkeleton::createContextMenu(void) {
    MenuStack menuStack("Skeleton");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new SkeletonAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeSkeleton();
        },
        canRemoveFrom(*_entity)
    );

    return menuStack.first();

}

void TkSkeleton::setAnimByName(const int trackIndex, const char*const animName, const bool loop) {
    if(!spAnimationState_setAnimationByName(_animState, trackIndex, animName, loop)) {
        //qWarning() << "Skeleton::setAnimByName() - " << animName << "animation could not be found!";
    }
}

void TkSkeleton::initializeDelegates(void) {
    LuaVariant delegates = {
        {
            "curAnim", new ComboBoxPropertyItemDelegate([](ESTkStandardItem* item) {
                LuaTableFieldItem* luaFieldItem = static_cast<LuaTableFieldItem*>(item);
                TkSkeleton* skeleton = luaFieldItem->getTableRef().getValue<TkSkeleton*>();
                QStringList options;
                if(skeleton && skeleton->_attrib) {
                    for(auto&& it: skeleton->_attrib->_animations) {
                        options.push_back(it->getName());
                    }
                }
                return options;

            }, true)
        },
        {
            "skin", new ComboBoxPropertyItemDelegate([](ESTkStandardItem* item) {
                LuaTableFieldItem* luaFieldItem = static_cast<LuaTableFieldItem*>(item);
                TkSkeleton* skeleton = luaFieldItem->getTableRef().getValue<TkSkeleton*>();
                QStringList options;
                if(skeleton && skeleton->_attrib) {
                    int i = 0;
                    for(;;) {
                        const char* name = skeleton->_attrib->getSpineSkinNameByIndex(i++);
                        if(!name) break;
                        options.push_back(QString(name));
                    }
                }
                return options;

            }, true)
        }
    };

    delegates.push();
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
}

void TkSkeletonAttrib::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* atlasDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, atlasDelegate);
    lua_setfield(*_mainState, -2, "atlasPath");

    auto* jsonDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, jsonDelegate);
    lua_setfield(*_mainState, -2, "jsonPath");

    DynamicContentPropertyItemDelegate* animDelegate =
            new DynamicContentPropertyItemDelegate(DynamicContentPropertyItemDelegate::luaArraySizeTextCallback);
    animDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH), DynamicContentPropertyItemDelegate::luaAddItemButtonCallback<elysian::Animation>);


    OOLUA::push(*_mainState, animDelegate);
    lua_setfield(*_mainState, -2, "animations");

    lua_pop(*_mainState, 1);
}

void TkSkeletonAttrib::initializeVirtualTableDelegates(QString key) {
    if(key == "animations") {

        LuaVariant delegateFunc = [](LuaVariant t, LuaVariant k) -> PropertyItemDelegate* {
            TkSkeletonAttrib* self = t["__ud"]->getValue<TkSkeletonAttrib*>();
            if(k.getType() == LUA_TNUMBER) {
                unsigned index = k.getValue<unsigned>();
                if(index <= self->getAnimationCount()) {
                    DynamicContentPropertyItemDelegate* delegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
                        LuaTableFieldItem* luaFieldItem = static_cast<LuaTableFieldItem*>(item);
                        TkSkeletonAttrib* attrib = luaFieldItem->getTableRef()["__ud"]->getValue<TkSkeletonAttrib*>();
                        unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                        elysian::Animation* anim = attrib->getAnimation(index);
                        QPalette palette = option.palette;
                        palette.setColor(QPalette::Text, QColor(Qt::gray));
                        option.palette = palette;
                        option.text = QString(anim->getName());
                    });

                    delegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), [](ESTkStandardItem* item) {

                        QStandardItem* parent = item->parent();
                        if(parent && parent->data(Qt::DisplayRole).toString() == "animations") {
                            parent = parent->parent();
                            if(parent && parent->data(Qt::DisplayRole).toString() == "attrib") {
                                parent = parent->parent();
                                if(parent && parent->data(Qt::DisplayRole).toString() == "skeleton") {
                                    QModelIndex labelIndex = parent->index();
                                    QModelIndex tableIndex = labelIndex.sibling(labelIndex.row(), labelIndex.column()+1);
                                    if(tableIndex.isValid()) {
                                        LuaTableFieldItem* luaItem = static_cast<LuaTableFieldItem*>(item->model()->itemFromIndex(tableIndex));
                                        luaItem->getVariant()["curAnim"].setVariant(static_cast<LuaTableFieldItem*>(item)->getVariant()["name"].getVariant());
                                    }

                                }
                            }

                        }


                    });

                    MenuStack stack("Animation");
                    stack.insertAction("Remove Animation", [=](bool) {
                        /*for(auto&& it : self->_animations) {

                        }*/


                    });

                    delegate->setContextMenu(stack.first());

                    return delegate;
                }
            }
            return nullptr;
        };
       // qDebug() << "REGISTERING" << delegateFunc.getTypeString();
        delegateFunc.push();
        lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    }
}


void TkAnimationTrack::initializeDelegates(void) {
    LuaVariant delegates = {
        {
            "name", new ComboBoxPropertyItemDelegate([](ESTkStandardItem* item) {
                auto* luaFieldItem = static_cast<LuaTableFieldItem*>(item);
                //Traversal to get back to TkSkeleton
                //AnimationTrack->Animation->AnimationAttrib
                QStringList options;
                LuaTableFieldItem* animItem  = static_cast<LuaTableFieldItem*>(luaFieldItem->getTableData()->getParentItem());

                if(animItem && QString(animItem->getVariant().getTypeString()) == "Animation") {
                    LuaTableFieldItem* attribItem = static_cast<LuaTableFieldItem*>(animItem->getTableData()->getParentItem());

                    if(attribItem && QString(attribItem->getVariant().getTypeString()) == "SkeletonAttrib") {

                        TkSkeletonAttrib* attrib = attribItem->getVariant().getValue<TkSkeletonAttrib*>();
                        if(attrib) {
                            int i = 0;
                            for(;;) {
                                const char* name = attrib->getSpineAnimNameByIndex(i++);
                                if(!name) break;
                                options.push_back(QString(name));
                            }
                        }
                    }
                }

                return options;

            }, true)
        }

    };

    delegates.push();
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
}

void TkAnimation::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    DynamicContentPropertyItemDelegate* tracksDelegate =
            new DynamicContentPropertyItemDelegate(DynamicContentPropertyItemDelegate::luaArraySizeTextCallback);
    tracksDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH), DynamicContentPropertyItemDelegate::luaAddItemButtonCallback<AnimationTrack>);

    OOLUA::push(*_mainState, tracksDelegate);
    lua_setfield(*_mainState, -2, "tracks");

    lua_pop(*_mainState, 1);
}

void TkAnimation::initializeVirtualTableDelegates(QString key) {
    if(key == "tracks") {

        LuaVariant delegateFunc = [](LuaVariant t, LuaVariant k) -> PropertyItemDelegate* {
            TkAnimation* self = t["__ud"]->getValue<TkAnimation*>();
            if(k.getType() == LUA_TNUMBER) {
                unsigned index = k.getValue<unsigned>();
                if(index <= self->getTrackCount()) {
                    DynamicContentPropertyItemDelegate* delegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
                        LuaTableFieldItem* luaFieldItem = static_cast<LuaTableFieldItem*>(item);
                        TkAnimation* anim = luaFieldItem->getTableRef()["__ud"]->getValue<TkAnimation*>();
                        unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                        elysian::AnimationTrack* track = anim->getTrack(index);
                        QPalette palette = option.palette;
                        palette.setColor(QPalette::Text, QColor(Qt::gray));
                        option.palette = palette;
                        option.text = QString(track->getName());
                    });

                    MenuStack stack("Animation");
                    stack.insertAction("Remove Animation", [=](bool) {
                        /*for(auto&& it : self->_animations) {

                        }*/


                    });

                    delegate->setContextMenu(stack.first());

                    return delegate;
                }
            }
            return nullptr;
        };
       // qDebug() << "REGISTERING" << delegateFunc.getTypeString();
        delegateFunc.push();
        lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    }
}

