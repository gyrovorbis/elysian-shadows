#include <ESToolkit/AssetIO/tk_dialog.h>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESGamma/system/elysian_dialog_system.hpp>
#include <QApplication>
#include <QESTkUtils/ui/delegate/tk_plain_text_edit_property_item_delegate.h>
#include <QMenu>

bool TkDialogOption::validate(const TkDialog* dialog, QString& errorMsg) const {
    if(getNextIndex() >= (int)dialog->getScreenCount() || getNextIndex() < -1) {
        errorMsg = "nextIndex is not a valid screen!";
        return false;
    } else return true;
}

bool TkDialogText::validate(const TkDialog* dialog, QString& errorMsg) const {
    if(getNextIndex() >= (int)dialog->getScreenCount() || getNextIndex() < -1) {
        errorMsg = "nextIndex is not a valid screen!";
        return false;
    } else return true;
}

void TkDialogOption::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    DynamicContentPropertyItemDelegate* nextIndexDelegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto* dialogOption = luaFieldItem->getTableRef().getValue<TkDialogOption*>();
            if(dialogOption) {
                int nextIndex = luaFieldItem->getVariant().getValue<int>();
                auto* dialogItem = luaFieldItem->getAncestorSiblingLuaItem({"*", "options", "*", "screens", "dialog"}, 1);
                if(dialogItem) {
                    auto dialog = dialogItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
                    if(dialog) {
                        if(nextIndex == 0 || nextIndex < -1 || nextIndex > (int)dialog->getScreenCount()) {
                            option.palette.setColor(QPalette::Text, QColor(Qt::red));
                            luaFieldItem->setToolTip("Invalid screen index!");
                        } else {
                            option.palette.setColor(QPalette::Text, QColor(Qt::black));
                            luaFieldItem->setToolTip("");
                        }
                    }
                }
            }
        }
    });
    OOLUA::push(*_mainState, nextIndexDelegate);
    lua_setfield(*_mainState, -2, "nextIndex");

    lua_pop(*_mainState, 1);
}

void TkDialogText::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    OOLUA::push(*_mainState, new QESTkUtils::PlainTextEditPropertyItemDelegate);
    lua_setfield(*_mainState, -2, "text");

    DynamicContentPropertyItemDelegate* nextIndexDelegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto* dialogText = luaFieldItem->getTableRef().getValue<TkDialogText*>();
            if(dialogText) {
                int nextIndex = luaFieldItem->getVariant().getValue<int>();
                auto* dialogItem = luaFieldItem->getAncestorSiblingLuaItem({"*", "screens", "dialog"}, 1);
                if(dialogItem) {
                    auto dialog = dialogItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
                    if(dialog) {
                        if(nextIndex == 0 || nextIndex < -1 || nextIndex > (int)dialog->getScreenCount()) {
                            option.palette.setColor(QPalette::Text, QColor(Qt::red));
                            luaFieldItem->setToolTip("Invalid screen index!");
                        } else {
                            option.palette.setColor(QPalette::Text, QColor(Qt::black));
                            luaFieldItem->setToolTip("");
                        }
                    }
                }
            }
        }
    });
    OOLUA::push(*_mainState, nextIndexDelegate);
    lua_setfield(*_mainState, -2, "nextIndex");

    lua_pop(*_mainState, 1);
}

bool TkDialogPrompt::validate(const TkDialog* dialog, QString& errorMsg) const {
    unsigned invalidCount = 0;

    if(!getOptionCount()) {
        errorMsg += "Must contain at least one option!";
        return false;
    }

    for(unsigned i = 0; i < getOptionCount(); ++i) {
        QString curErrorMsg;
        if(!getOption(i)->validate(dialog, curErrorMsg)) {
            if(invalidCount++) errorMsg += "\n";
            errorMsg += QString("options[%1]: ").arg(i+1) + curErrorMsg;
        }
    }
    return !invalidCount;
}

bool TkDialog::validate(QString& errorMsg) const {
    unsigned invalidCount = 0;
    if(!getScreenCount()) {
        errorMsg += "Dialog must contain at least one screen!";
        return false;
    }

    if(getStartScreenLua() > (int)getScreenCount() || getStartScreenLua() < 1) {
        errorMsg += "startScreen: invalid screen index!";
        ++invalidCount;
    }

    for(unsigned i = 0; i < getScreenCount(); ++i) {
        QString curErrorMsg;
        if(!getScreen(i)->validate(this, curErrorMsg)) {
            if(invalidCount++) errorMsg += "\n\n";
            errorMsg += QString("screens[%1]: ").arg(i+1) + QString("\n") + curErrorMsg;
        }
    }
    return !invalidCount;
}


void TkDialog::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    DynamicContentPropertyItemDelegate* screensDelegate =
            new DynamicContentPropertyItemDelegate(DynamicContentPropertyItemDelegate::luaArraySizeTextCallback<true>);

    screensDelegate->setReadOnly();

    screensDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH),
        [](ESTkStandardItem* item) {
        auto* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto dialog = luaFieldItem->getTableRef().getValue<shared_ptr<TkDialog>>();
            if(dialog) {
                MenuStack menu("Add DialogScreen");

                menu.insertAction("DialogText", [=](bool) {
                    luaFieldItem->getVariant()[dialog->getScreenCount()+1].setVariant(new TkDialogText);
                    luaFieldItem->setDirty(true);
                });

                menu.insertAction("DialogPrompt", [=](bool) {
                    luaFieldItem->getVariant()[dialog->getScreenCount()+1].setVariant(new TkDialogPrompt);
                    luaFieldItem->setDirty(true);
                });

                menu.execDeleteLater(QCursor::pos());
            }
        }
    });
    OOLUA::push(*_mainState, screensDelegate);
    lua_setfield(*_mainState, -2, "screens");

    DynamicContentPropertyItemDelegate* startScreenDelegate =
            new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
            auto* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
            if(luaFieldItem) {
                auto dialog = luaFieldItem->getTableRef().getValue<shared_ptr<TkDialog>>();
                const unsigned startScreen = dialog->getStartScreen();
                if(startScreen >= dialog->getScreenCount()) {
                    option.palette.setColor(QPalette::Text, QColor(Qt::red));
                    luaFieldItem->setToolTip("Invalid screen index!");
                } else {
                    luaFieldItem->setToolTip("");
                }
            }
        });

    OOLUA::push(*_mainState, startScreenDelegate);
    lua_setfield(*_mainState, -2, "startScreen");


#if 0
    animDelegate->setMimeDropManager(new LuaMimeDropManager(QMap<QString, LuaMimeDropManager::LuaHandler> {
        {
            QString("Entity"),
            {
                [](LuaVariant v, Qt::DropAction, QDropEvent*, int, int, const QModelIndex& parent) {
                    auto* model = qobject_cast<const QStandardItemModel*>(parent.model());
                    if(model) {
                        LuaTableFieldItem* luaItem = LuaTableFieldItem::fromStandardItem(model->itemFromIndex(parent));
                        if(luaItem) {
                            const LuaVariant& self = luaItem->getTableData()->getTableRef();
                            LuaVariant srcTable = v["skeleton"]["attrib"]["animations"].getVariant();

                            for(int i = 1; i <= srcTable.getLength(); ++i) {
                                self[i].setVariant(srcTable[i].getVariant());
                            }

                            return true;
                        }
                    }
                    return false;
                },
                [](LuaVariant v, Qt::DropAction, QDragEnterEvent*, int, int, const QModelIndex&) {
                    return !v["skeleton"]["attrib"]["animations"].getVariant().isNil();
                }
            }
        }

    }));
#endif



    lua_pop(*_mainState, 1);

}

void TkDialog::initializeVirtualTableDelegates(QString key) {

    if(key == "screens") {

        LuaVariant delegateFunc = [](LuaVariant t, LuaVariant k) -> PropertyItemDelegate* {
            auto self = t["__ud"]->getValue<std::shared_ptr<TkDialog>>();
            if(self && k.getType() == LUA_TNUMBER) {
                unsigned index = k.getValue<unsigned>();
                if(index <= self->getScreenCount()) {

                    DynamicContentPropertyItemDelegate* delegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto dialog = luaFieldItem->getTableRef()["__ud"]->getValue<std::shared_ptr<TkDialog>>();
                            if(dialog) {
                                unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                TkDialogScreen* screen = dialog->getScreen(index);
                                if(screen) {
                                    QString errorMsg;
                                    bool valid = screen->validate(dialog.get(), errorMsg);
                                    if(valid) {
                                        option.palette.setColor(QPalette::Text, QColor(Qt::gray));
                                        luaFieldItem->setToolTip("");
                                    } else {
                                        option.palette.setColor(QPalette::Text, QColor(Qt::red));
                                        luaFieldItem->setToolTip(errorMsg);
                                    }

                                    QString typeStr;
                                    if(screen->getScreenType() == TkDialogScreen::SCREEN_TEXT) {
                                        typeStr = "";
                                    } else if (screen->getScreenType() == TkDialogScreen::SCREEN_PROMPT) {
                                        unsigned optionCount = static_cast<TkDialogPrompt*>(screen)->getOptionCount();
                                        typeStr = QString("[Prompt, %1 option(s)] ").arg(optionCount);
                                    }

                                    QStringList lines = QString(screen->getText()).split("\n");

                                    QString text = lines.first() + ((lines.count() > 1)? QString(" [..]") : QString(""));
                                    option.text = typeStr + (text.isEmpty()? QString("No text") : text);
                                }
                            }
                        }
                    });
                    delegate->setReadOnly();

                    delegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), [](ESTkStandardItem* item) {
                        auto* luaItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaItem) {
                            auto* dialogItem = luaItem->getAncestorSiblingLuaItem({"screens", "dialog"}, 1);
                            if(dialogItem) {
                                auto dialog = dialogItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
                                if(dialog) {
                                    unsigned screenIndex = luaItem->getKey().getValue<unsigned>();
                                    elysian::System::getDialogSys()->startDialogAtScreenIndex(dialog, screenIndex-1);
                                }
                            }
                        }
                    });

                    delegate->setDeleteCallback([](ESTkStandardItem* item) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto* dialogItem = luaFieldItem->getAncestorSiblingLuaItem({"screens", "dialog"}, 1);
                            if(dialogItem) {
                                auto dialog = dialogItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
                                if(dialog) {
                                    unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                    if(index <= dialog->getScreenCount()) {
                                        dialog->setScreen(index, nullptr);
                                        dialogItem->setDirty();
                                    }
                                }
                            }
                        }
                    });

                    delegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_MINUS_PATH),
                        [](ESTkStandardItem* item) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto* dialogItem = luaFieldItem->getAncestorSiblingLuaItem({"screens", "dialog"}, 1);
                            if(dialogItem) {
                                auto dialog = dialogItem->getVariant().getValue<std::shared_ptr<TkDialog>>();
                                if(dialog) {
                                    unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                    if(index <= dialog->getScreenCount()) {
                                        dialog->setScreen(index, nullptr);
                                        dialogItem->setDirty();
                                    }
                                }
                            }
                        }
                    });

                    delegate->setContextMenuCallback([=](ESTkStandardItem* item) -> QMenu* {
                        MenuStack stack("DialogScreen");
                        stack.insertAction("Delete", [=](bool) {
                            delegate->getDeleteCallback()(item);

                        });

                        return stack.takeTopLevel();
                    });

                    return delegate;
                }
            }
            return nullptr;
        };

        delegateFunc.push();
        lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    }

}

void TkDialogPrompt::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    DynamicContentPropertyItemDelegate* optionsDelegate =
            new DynamicContentPropertyItemDelegate(DynamicContentPropertyItemDelegate::luaArraySizeTextCallback<true>);

    optionsDelegate->setReadOnly();

    optionsDelegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_PLUS_PATH),
        [](ESTkStandardItem* item) {
        auto* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaFieldItem) {
            auto* prompt = luaFieldItem->getTableRef().getValue<TkDialogPrompt*>();
            if(prompt) {
                luaFieldItem->getVariant()[prompt->getOptionCount()+1].setVariant(new TkDialogOption);
                luaFieldItem->setDirty();
            }
        }

    });

    OOLUA::push(*_mainState, optionsDelegate);
    lua_setfield(*_mainState, -2, "options");

    OOLUA::push(*_mainState, new QESTkUtils::PlainTextEditPropertyItemDelegate);
    lua_setfield(*_mainState, -2, "text");
    lua_pop(*_mainState, 1);
}

void TkDialogPrompt::initializeVirtualTableDelegates(QString key) {

    if(key == "options") {

        LuaVariant delegateFunc = [](LuaVariant t, LuaVariant k) -> PropertyItemDelegate* {
            auto* self = t["__ud"]->getValue<TkDialogPrompt*>();
            if(self && k.getType() == LUA_TNUMBER) {
                unsigned index = k.getValue<unsigned>();
                if(index <= self->getOptionCount()) {

                    DynamicContentPropertyItemDelegate* delegate = new DynamicContentPropertyItemDelegate([](ESTkStandardItem* item, QStyleOptionViewItem& option) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto* prompt = luaFieldItem->getTableRef()["__ud"]->getValue<TkDialogPrompt*>();
                            if(prompt) {
                                unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                TkDialogOption* opt = prompt->getOption(index);
                                if(opt) {
                                    option.palette.setColor(QPalette::Text, QColor(Qt::gray));
                                    QString text = opt->getText();
                                    option.text = text.isEmpty()? "[No text]" : text;
                                }
                            }
                        }
                    });
                    delegate->setReadOnly();

                    delegate->setDeleteCallback([](ESTkStandardItem* item) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto* promptItem = luaFieldItem->getAncestorSiblingLuaItem({"options", "*"}, 1);
                            if(promptItem) {
                                auto* prompt = promptItem->getVariant().getValue<TkDialogPrompt*>();
                                if(prompt) {
                                    unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                    if(index <= prompt->getOptionCount()) {
                                        prompt->setOption(index, nullptr);
                                        promptItem->setDirty();
                                    }
                                }
                            }
                        }
                    });

                    delegate->addButton(QIcon(DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_ICON_MINUS_PATH),
                        [](ESTkStandardItem* item) {
                        LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                        if(luaFieldItem) {
                            auto* promptItem = luaFieldItem->getAncestorSiblingLuaItem({"options", "*"}, 1);
                            if(promptItem) {
                                auto* prompt = promptItem->getVariant().getValue<TkDialogPrompt*>();
                                if(prompt) {
                                    unsigned index = luaFieldItem->getKey().getValue<unsigned>()-1;
                                    if(index <= prompt->getOptionCount()) {
                                        prompt->setOption(index, nullptr);
                                        promptItem->setDirty();
                                    }
                                }
                            }
                        }
                    });

                    delegate->setContextMenuCallback([=](ESTkStandardItem* item) -> QMenu* {
                        MenuStack stack("DialogOption");
                        stack.insertAction("Delete", [=](bool) {
                            delegate->getDeleteCallback()(item);

                        });

                        return stack.takeTopLevel();
                    });


                    return delegate;
                }
            }
            return nullptr;
        };

        delegateFunc.push();
        lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
    }

}
