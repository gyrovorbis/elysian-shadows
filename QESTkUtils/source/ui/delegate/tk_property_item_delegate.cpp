#include <QMouseEvent>
#include <ESToolkit/core/tk_file_utilities.h>
#include <QMenu>
#include <QInputDialog>
#include <QFileDialog>
#include <QAction>
#include <AssetIO/crtp/elysian_lua_variant.hpp>

#include <QESTkUtils/ui/delegate/tk_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/ui/dock/asset/tk_asset_dock_widget.h>
#include <ESToolkit/AssetIO/tk_area.h>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <QESTkUtils/tk_lua_mime_drop_manager.h>


class NewLineFilter: public QObject {
public:
    virtual bool eventFilter(QObject* object, QEvent* event) override;
};

bool NewLineFilter::eventFilter(QObject* object, QEvent* event) {
    QLineEdit* edit = qobject_cast<QLineEdit*>(object);
    if(edit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Return && keyEvent->modifiers()&Qt::ShiftModifier) {
            int pos = edit->cursorPosition();
            QString str = edit->text();
            str.insert(pos, "\n");
            edit->setCursorPosition(pos+1);
            edit->setText(str);
            event->accept();
            return true;
        }
    }
    return false;
}


QWidget* PropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(_readOnly) return nullptr;
    auto* widget = QStyledItemDelegate::createEditor(parent, option, index);

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

    if(lineEdit) {
        lineEdit->installEventFilter(new NewLineFilter);

        if(_completerCallback) {
            QStringList list = _completerCallback(modelIndexToESTkStandardItem(index));
            if(list.size()) {
                QCompleter* completer = new QCompleter(list);
                completer->setCompletionMode((QCompleter::CompletionMode)_completerMode);
                lineEdit->setCompleter(completer);
            }
        }
    }

    return widget;

}

bool PropertyItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    auto* propItem = modelIndexToLuaTableFieldItem(index);

    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::RightButton) {
            auto* menu = getContextMenu(propItem);
            if(menu) menu->exec(mouseEvent->globalPos());
            return true;
        }

    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}



QMenu* PropertyItemDelegate::getContextMenu(ESTkStandardItem*const item) {

    if(_menuCallback) return _menuCallback(item);

    LuaTableFieldItem* fieldItem =
            (item->type() == (int)ESTkStandardItemType::LuaTable)?
                static_cast<LuaTableFieldItem*>(item) : nullptr;

    if(!fieldItem) return nullptr;

    QESTkUtils::MenuStack menu("PropertyItem");

    menu.push("type");

    for(int i = 0; i < _supportedTypes.size(); ++i) {
        switch(_supportedTypes[i]) {
        case LUA_TNIL:
            menu.insertCheckableAction("nil", [=](const bool) {
                fieldItem->setData("nil", Qt::EditRole);

            }, fieldItem->getValueLuaType() == LUA_TNIL);
            break;
        case LUA_TBOOLEAN:
            menu.insertCheckableAction("bool", [=](const bool) {
                fieldItem->setData(false, Qt::EditRole);
            }, fieldItem->getValueLuaType() == LUA_TBOOLEAN);
            break;
        case LUA_TNUMBER:
            menu.insertCheckableAction("number", [=](const bool) {
                fieldItem->setData(0, Qt::EditRole);

            }, fieldItem->getValueLuaType() == LUA_TNUMBER);
            break;
        case LUA_TSTRING:
            menu.insertCheckableAction("string", [=](const bool) {
                fieldItem->setData("none", Qt::EditRole);
            }, fieldItem->getValueLuaType() == LUA_TSTRING);
            break;
        case LUA_TTABLE:
            menu.insertCheckableAction("table", [=](const bool) {
                LuaVariant v;
                lua_newtable(*_mainState);
                v.pull();
                fieldItem->setVariant(v);
            }, fieldItem->getValueLuaType() == LUA_TTABLE);
            break;
        case LUA_TUSERDATA:
            break;
        }
    }

    menu.push("userdata");
    for(auto it: LuaManager::_udTypeNames) {
        menu.insertCheckableAction(it, [=](const bool) {
            LuaVariant v = LuaManager::exec("if(%s.new) then return %s.new() else return {} end", Q_CSTR(it), Q_CSTR(it));
            fieldItem->setVariant(v);
        });
    }


    menu.pop(2);

    if(fieldItem->getValueLuaType() == LUA_TTABLE || fieldItem->getValueLuaType() == LUA_TUSERDATA) {
        menu.insertAction("Add Key", [=](bool) {
            QString key = QInputDialog::getText(nullptr, "Add New Field", "Key Name");
            if(key.isNull()) return;

            QStringList typeList = TkLuaManager::getUdTypeNames();
            typeList.push_front("nil");
            typeList.push_front("boolean");
            typeList.push_front("number");
            typeList.push_front("string");
            typeList.push_front("table");

            QString type = QInputDialog::getItem(nullptr, "Add New Field", "Type", typeList);

            if(type.isNull()) return;

            //lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, fieldItem->getTableData()->getLuaRef());
            fieldItem->getTableData()->getTableRef().push();
            OOLUA::push(*_mainState, QVariant(key));

            if(type == "nil") {
                OOLUA::push(*_mainState, LuaVariant(nullptr));
            } else if(type == "boolean") {
                OOLUA::push(*_mainState, LuaVariant(false));
            } else if(type == "number") {
                OOLUA::push(*_mainState, LuaVariant(0.0f));
            } else if(type == "string") {
                OOLUA::push(*_mainState, LuaVariant("none"));
            } else if(type == "table") {
              //  OOLUA::push(*_mainState, LuaVariant({1}));
                lua_newtable(*_mainState);
            } else {
                OOLUA::push(*_mainState, LuaManager::exec("if(%s.new) then return %s.new() else return {} end", Q_CSTR(type), Q_CSTR(type)));
            }

            lua_settable(*_mainState, -3);

#if 0
            elysian::LuaTableRef ref(fieldItem->getTableData()->getLuaRef());
            ref.insert(Q_CSTR(key), elysian::LuaVariant(12.0f));
           // fieldItem->getTableData()->insertTableFieldItem(key);
#endif

        });

        menu.insertAction("Make Prefab", [=](bool) {
            QString filename = QFileDialog::getSaveFileName(nullptr, "Save as Prefab", FileUtilities::getAbsoluteAssetPath(ELYSIAN_LUA_PREFAB_PATH));
            filename = PrefabAssetFileType::prefabifyFilePath(filename);
            if(LuaManager::saveTable(fieldItem->getTableData()->getTableRef(), Q_CSTR(filename))) {
                LuaVariant prefabRef = LuaManager::getPrefab(Q_CSTR(PrefabAssetFileType::prefabNameFromFilePath(filename)));
                fieldItem->setVariant(prefabRef);
            }
        });

        menu.insertAction("Save As...", [=](bool) {
            QString filename = QFileDialog::getSaveFileName(nullptr, "Save table as...", FileUtilities::getAssetPath());
            LuaManager::saveTable(fieldItem->getTableData()->getTableRef(), Q_CSTR(filename));
        });

        auto v = fieldItem->getVariant();
        if(strcmp(v.getTypeString(), "Entity") == 0) {
            menu.insertAction("Make Parent", [=](bool) {
                QString filename = QFileDialog::getSaveFileName(nullptr, "Save as Prefab", FileUtilities::getAbsoluteAssetPath(ELYSIAN_LUA_PREFAB_PATH));
                filename = PrefabAssetFileType::prefabifyFilePath(filename);
                if(LuaManager::saveTable(fieldItem->getTableData()->getTableRef(), Q_CSTR(filename))) {
                    //remove prefab from the area
                    getAssets()->getCurrentArea()->removeEntity(v.getValue<TkEntity*>());

                    LuaVariant prefabRef = LuaManager::getPrefab(Q_CSTR(PrefabAssetFileType::prefabNameFromFilePath(filename)));
                    auto entity = prefabRef["new"].getVariant()(prefabRef);

                    getAssets()->getCurrentArea()->addEntity(entity.getValue<TkEntity*>());
                    fieldItem->setVariant(entity);
                }
            });
        }


        menu.insertAction("Break Prefab", [=](bool) {
            LuaManager::callGlobalFunction<void>("separatefromparent", nullptr, fieldItem->getVariant());
        }, !LuaManager::callGlobalFunction<LuaVariant>("getparenttable", nullptr, fieldItem->getVariant()).isNil());

    }

    menu.insertAction("Find Documentation", [=](bool) {
        fieldItem->getKey();
        fieldItem->getTableRef().getTypeString();


    });


    return menu.takeTopLevel();
}


ESTkStandardItem* PropertyItemDelegate::modelIndexToESTkStandardItem(const QModelIndex& index) {
    if(index.isValid()) {
        auto* model = qobject_cast<const PropertyStackModel*>(index.model());
        if(model) {
            auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
            if(item->type() >= (int)ESTkStandardItemType::Generic) {
                return static_cast<ESTkStandardItem*>(item);
            }
        }
    }
    return nullptr;
}

LuaTableFieldItem*  PropertyItemDelegate::modelIndexToLuaTableFieldItem(const QModelIndex& index) {
    if(index.isValid()) {
        auto* model = qobject_cast<const PropertyStackModel*>(index.model());
        if(model) {
            auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
            if(item->type() == (int)ESTkStandardItemType::LuaTable)
                return static_cast<LuaTableFieldItem*>(item);
        }
    }
    return nullptr;
}

bool PropertyItemDelegate::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row , int col, const QModelIndex& parent) const {
    return _mimeDropManager?
                _mimeDropManager->canDrop(data, action, nullptr, row, col, parent) :
                false;
}

bool PropertyItemDelegate::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int col, const QModelIndex& parent) const {
    return _mimeDropManager?
                _mimeDropManager->drop(data, action, nullptr, row, col, parent) :
                false;
}
