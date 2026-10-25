/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <ESToolkit/AssetIO/tk_lua_table_proxy.h>
#include <QVariant>
#include <ESToolkit/AssetIO/tk_area.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>
#include <QESTkUtils/tk_lua_qt_utils.h>

//======== LuaTableFieldItem ========

LuaTableFieldItem::LuaTableFieldItem(LuaFieldRef ref, QStandardItem*const labelItem):
    _labelItem(labelItem),
    _fieldRef(std::move(ref))
{
    //f(_fieldRef->getType() == LUA_TTABLE || _fieldRef->getType() == LUA_TUSERDATA) {
        _settingFlags = true;
        setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);
        _settingFlags = false;
    //}
}

void LuaTableFieldItem::insertingModelItem(void) {
    _latentInit(this, &LuaTableFieldItem::getValue, &LuaTableFieldItem::setValue,
                    UndoItem::LUA_TABLE_FIELD_ITEM, &getCurrentArea()->getUndoStack());
}


//Pull value from Lua table/userdata to update model
inline QVariant LuaTableFieldItem::getValue(void) const {
    LuaManager::stackBalanceBegin();

    QVariant    val;
    int         valType;

    //retrieve field
    _fieldRef.getTableRef().push();
    _fieldRef.getKey().push();
    lua_gettable(*_mainState, -2);

    //check type
    valType = lua_type(*_mainState, -1);

    //Let QVariant handle all primitive types
    if(valType != LUA_TTABLE && valType != LUA_TUSERDATA) {
        if(_tableData) {
            //setDirty(true);
            //emitDataChanged();
        }
        OOLUA::pull(*_mainState, val);
    //Table/Userdata fields are out-of-date

    } else if(!_tableData) {
        _tableData = new LuaTableTableFieldData(LuaVariant::fromStack());
        //if(!_tableData->getParentRef().isNil() && (*_tableData->getParentRef()[_fieldRef.getKey()] == ))
        if(_tableData->isPrefab()) {
            _labelItem->setData(QVariant(ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR), Qt::ForegroundRole);
        }

        _tableData->insertTableFieldItems(_labelItem);
    } //else Table/userdata fields should be up-to-date
    else {
         //val.setValue(QString(lua_typename(*_mainState, valType)));
         lua_pop(*_mainState, 1); //don't actually give a fuck about the table
    }
    lua_pop(*_mainState, 1);

    LuaManager::stackBalanceEnd();
    return val;
}


//Push value from model to update Lua table/userdata
inline void LuaTableFieldItem::setValue(const QVariant val) {
    _fieldRef.getTableRef().push();
    _fieldRef.getKey().push();
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
}


LuaTableFieldItem::~LuaTableFieldItem(void) {
    if(_ownsDelegate) delete _delegate;
    delete _tableData;
}

void LuaTableFieldItem::setDirty(const bool val) {
    if(val) {
#if 1
        while(_labelItem->rowCount()) {
            auto&& row = _labelItem->takeRow(0);
            for(int j = 0; j < row.size(); ++j) {
                delete row[j];
            }
        }
#else


    static_cast<PropertyStackModel*>(model())->removeChildItems(this);
#endif
        model()->itemChanged(this);
        delete _tableData;
        _tableData = nullptr;
    }
}

QStyledItemDelegate* LuaTableFieldItem::getStyledItemDelegate(void) const {
    if(!_triedDelegate) {
        _triedDelegate = true;

        LuaManager::stackBalanceBegin();
        _fieldRef.getTableRef().push();

        if(LuaManager::getMetatable(-1)) {
            //Check for delegate associated with particular field
            lua_getfield(*_mainState, -1, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
            if(!lua_isnil(*_mainState, -1)) {
                int type = lua_type(*_mainState, -1);
                switch(type) {
                case LUA_TFUNCTION:
                    _delegate = LuaManager::_callFunction<PropertyItemDelegate*>(nullptr, _fieldRef.getTableRef(), _fieldRef.getKey());
                    if(_delegate) _ownsDelegate = true;
                    break;
                case LUA_TTABLE:
                    _fieldRef.getKey().push();
                    lua_gettable(*_mainState, -2);

                    if(!lua_isnil(*_mainState, -1)) {
                        PropertyItemDelegate* delegate;
                        OOLUA::pull(*_mainState, delegate);
                        _delegate = static_cast<QStyledItemDelegate*>(delegate);
                        GY_ASSERT(_delegate); //WTF
                    } else lua_pop(*_mainState, 1);

                    lua_pop(*_mainState, 1);
                    break;
                default:
                    lua_pop(*_mainState, 1);
                    GY_ASSERT(false); //trying to pull some kind of fucked-ass shit as a delegate!
                    break;
                }
            } else lua_pop(*_mainState, 1);
            lua_pop(*_mainState, 1);
        }
        lua_pop(*_mainState, 1);


        //Fall-back: Check for delegate associated with particular outer table/userdata type
        const int luaType = _fieldRef.getVariant().getType();
        if(!_delegate && (luaType == LUA_TTABLE || luaType == LUA_TUSERDATA)) {
            _fieldRef.getVariant().push();
            if(LuaManager::getMetatable(-1)) {
                lua_getfield(*_mainState, -1, TK_LUA_TABLE_OUTER_DELEGATE_FIELD_KEY);
                if(!lua_isnil(*_mainState, -1)) {
                    int type = lua_type(*_mainState, -1);
                    switch(type) {
                    case LUA_TUSERDATA: {
                        PropertyItemDelegate* delegate;
                        OOLUA::pull(*_mainState, delegate);
                        _delegate = static_cast<QStyledItemDelegate*>(delegate);
                        GY_ASSERT(_delegate); //WTF
                        break;
                    }
                    default:
                        lua_pop(*_mainState, 1);
                        GY_ASSERT(false); //trying to pull some kind of fucked-ass shit as a delegate!
                        break;
                    }
                } else lua_pop(*_mainState, 1);
                lua_pop(*_mainState, 1);
            }

            lua_pop(*_mainState, 1);
        }

        LuaManager::stackBalanceEnd();
    }

    return _delegate;

}


QStandardItem* LuaTableFieldItem::getAncestorItem(QStringList keyNames) {
    QStandardItem* curItem = this;
    for(int i = 0; i < keyNames.size(); ++i) {
        curItem = curItem->parent();
        if(!curItem ||
                (keyNames[i] != "*" && //wildcard!
                 curItem->data(Qt::DisplayRole).toString() != keyNames[i]))
        {
            return nullptr;
        }
    }
    return curItem;
}

QStandardItem* LuaTableFieldItem::getAncestorSiblingItem(QStringList keyNames, int col) {
    QStandardItem* item = getAncestorItem(keyNames);
    if(item) {
        QModelIndex itemIndex = item->index();
        QModelIndex siblingIndex = itemIndex.sibling(itemIndex.row(), itemIndex.column()+col);
        if(siblingIndex.isValid()) {
            return model()->itemFromIndex(siblingIndex);
        }
    }
    return nullptr;
}

//======== LuaTableFieldData ========

void LuaTableTableFieldData::onFieldChanged(const LuaVariant& key) {
    auto* item = static_cast<LuaTableFieldItem*>(_fieldItems[key]->getValue<void*>());
    if(item) {
        item->setDirty();
        item->emitDataChanged();
    } else {
        Debug::printf(Debug::LOG_ERROR, "LuaTableTableFieldData(LuaVariant key): Could not find key '%s' in the table!", key.asString().getValue<const char*>());

    }
}

void LuaTableTableFieldData::onFieldAdded(const LuaVariant& key) {
    insertTableFieldItem(key);
}


void LuaTableTableFieldData::insertTableFieldItem(LuaVariant key) {
    QList<QStandardItem*> items;

    auto* labelItem = PropertyStackModel::createReadOnlyItem(key.asString().getValue<const char*>());
    auto* valueItem = new LuaTableFieldItem(LuaFieldRef(LuaVariant::makeWeakRef(_tableRef), std::move(key)), labelItem);

    if(_isPrefab) {
        labelItem->setData(QVariant(ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR), Qt::ForegroundRole);
        valueItem->setData(QVariant(ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR), Qt::ForegroundRole);
    }

    if(!_parentRef.isNil() && (*_parentRef[valueItem->getKey()] == valueItem->getVariant())) {
        labelItem->setData(QVariant(QColor(Qt::darkGreen)), Qt::ForegroundRole);
        valueItem->setData(QVariant(QColor(Qt::darkGreen)), Qt::ForegroundRole);
    }

    items.push_back(labelItem);
    items.push_back(valueItem);

    _parentItem->appendRow(items);
    valueItem->insertingModelItem();
    _fieldItems[valueItem->getKey()] = (void*)valueItem;

}


//Iterates over table or proxied userdata, populating model based on k,v pairs
void LuaTableTableFieldData::insertTableFieldItems(QStandardItem*const parentItem) {

    LuaManager::stackBalanceBegin();
    bool metatable;
    bool iterable;

    _parentItem = parentItem; //Store reference to its own parent

    _tableRef.push();
    int valueType = lua_type(*_mainState, -1);

    //Downcast to most specific type before recursing over it
    if(valueType == LUA_TUSERDATA) {
        LuaManager::pushGlobalField("down_cast");
        lua_pushvalue(*_mainState, -2);
        ELYSIAN_LUA_MANAGER_PROTECTED_CALL(1, 1);
        //update reference
        lua_pushvalue(*_mainState, -1);

        _tableRef.pull();
        lua_remove(*_mainState, -2);
    }

    //Populate metatable
    if((metatable = getMetatable(-1))) {
        //int metaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
       // (void)metaRef;
        lua_pop(*_mainState, 1);
       /* auto* label = new QStandardItem("__metatable");
        auto* valueItem = new LuaTableItem(metaRef, label);
        stackModel->addRow(label, valueItem);*/
    }

    //Check if userdata is iterable
    if(valueType == LUA_TUSERDATA && metatable) {
        getMetatable(-1);
        lua_getfield(*_mainState, -1, "__connectSignals");
        //userdata has lua table proxy
        if(!lua_isnil(*_mainState, -1)) {
            lua_pushvalue(*_mainState, -3);
            lua_pushlightuserdata(*_mainState, static_cast<QObject*>(this));
            ELYSIAN_LUA_MANAGER_PROTECTED_CALL(2, 0, nullptr);
            iterable = true;
        //userdata doesn't have lua table proxy
        } else {
            iterable = false;
            lua_pop(*_mainState, 1); //pop nil
        }
        lua_pop(*_mainState, 1); //pop metatable
    //Populate table
    } else if(valueType == LUA_TTABLE) {
        iterable = true;
        auto* ud = TkLuaTableUserdata::inject(*_mainState, _tableRef);
        ud->disconnect(this);
        connect(ud, SIGNAL(fieldChanged(const LuaVariant&)), this, SLOT(onFieldChanged(const LuaVariant&)));
        connect(ud, SIGNAL(fieldAdded(const LuaVariant&)), this, SLOT(onFieldAdded(const LuaVariant&)));
    } else iterable = false;

    if(iterable) {
        genericIterate(-1, [&](void) {
            LuaVariant key;
            lua_pushvalue(*_mainState, -2);
            OOLUA::pull(*_mainState, key);

            insertTableFieldItem(std::move(key));

        });
    }
    lua_pop(*_mainState, 1);

    LuaManager::stackBalanceEnd();
}



//======== LuaTableStackModel ========


void LuaTableStackModel::populateModel(void) {
    if(_populated) return;
    setHorizontalHeaderItem(0, new QStandardItem("Key"));
    setHorizontalHeaderItem(1, new QStandardItem("Value"));
    //setHorizontalHeaderItem(2, new QStandardItem("Type"));


    //Create an invisible outer proxy table around table for table delegates to work properly/orthogonally on first item.
    LuaVariant v = {
        { 1, _tableRef }
    };

    QList<QStandardItem*> items;
    _parentItem = PropertyStackModel::createReadOnlyItem(_tableRef.getTypeString());//ESTkStandardItem(QString("Lua Table"));
    items.push_back(_parentItem);
    auto* valItem = new LuaTableFieldItem(LuaFieldRef(std::move(v), LuaVariant(1)), _parentItem);
    items.push_back(valItem);

    //items.push_back(PropertyStackModel::createReadOnlyItem());
    QStandardItemModel::appendRow(items);
    valItem->insertingModelItem();

    //_tableData = new LuaTableTableFieldData(_luaRef);
    //_tableData->insertTableFieldItems(_parentItem);

    _populated = true;
}


bool LuaTableStackModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {
    QStandardItem* item = itemFromIndex(parent);
    LuaTableFieldItem* luaItem = nullptr;

    if(item && item->type() == ESTkStandardItemType::LuaTable) {
        luaItem = static_cast<LuaTableFieldItem*>(item);
        PropertyItemDelegate* delegate = qobject_cast<PropertyItemDelegate*>(luaItem->getStyledItemDelegate());

        if(delegate) {
            return delegate->canDropMimeData(data, action, row, column, parent);
        }
    }

    return false;
}

bool LuaTableStackModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {
    QStandardItem* item = itemFromIndex(parent);
    LuaTableFieldItem* luaItem = nullptr;

    if(item && item->type() == ESTkStandardItemType::LuaTable) {
        luaItem = static_cast<LuaTableFieldItem*>(item);
        PropertyItemDelegate* delegate = qobject_cast<PropertyItemDelegate*>(luaItem->getStyledItemDelegate());

        if(delegate) {
            return delegate->dropMimeData(data, action, row, column, parent);
        }
    }

    return false;
}

Qt::DropActions LuaTableStackModel::supportedDropActions(void) const { return Qt::ActionMask; }
Qt::DropActions LuaTableStackModel::supportedDragActions(void) const { return Qt::ActionMask; }
QStringList LuaTableStackModel::mimeTypes(void) const {
    QStringList list;
    list << "text/plain" << "text/uri-list";
    return list;

}
