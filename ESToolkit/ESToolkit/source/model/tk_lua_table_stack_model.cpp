#include <tk_lua_table_stack_model.h>
#include <asset/tk_lua_table_proxy.h>
#include <QVariant>
#include <asset/tk_area.h>
#include <estk/tk_lua_wrappers.h>
#include <core/tk_lua_qt_utils.h>

LuaTableFieldItem::LuaTableFieldItem(LuaFieldRef ref, QStandardItem*const labelItem):
    _labelItem(labelItem),
    _fieldRef(std::move(ref))
{
    if(_fieldRef->getType() == LUA_TTABLE || _fieldRef->getType() == LUA_TUSERDATA) {
        _settingFlags = true;
        setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::NoItemFlags);
        _settingFlags = false;
    }
}

void LuaTableFieldItem::insertingModelItem(void) {
    _latentInit(this, &LuaTableFieldItem::getValue, &LuaTableFieldItem::setValue,
                    UndoItem::LUA_TABLE_FIELD_ITEM, &getCurrentArea()->getUndoStack());
}


void LuaTableTableFieldData::onFieldChanged(const LuaVariant& key) {
    auto* item = static_cast<LuaTableFieldItem*>(_fieldItems[key]->getValue<void*>());
    item->setDirty();
    item->emitDataChanged();
}

void LuaTableTableFieldData::onFieldAdded(const LuaVariant& key) {
    insertTableFieldItem(key);
}

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
        LuaManager::_protectedCall(1, 1, nullptr, "LuaTableTableFieldData - downcasting");
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
            _protectedCall(2, 0, nullptr);
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

LuaTableFieldItem::~LuaTableFieldItem(void) {
    delete _tableData;
}

inline void LuaTableFieldItem::setDirty(const bool val) {
    if(val) {
        while(_labelItem->rowCount()) {
            auto&& row = _labelItem->takeRow(0);
            for(int j = 0; j < row.size(); ++j) {
                delete row[j];
            }
        }
        delete _tableData;
        _tableData = nullptr;
    }
}

QStyledItemDelegate* LuaTableFieldItem::getStyledItemDelegate(void) const {

    if(!_delegate) {

        LuaManager::stackBalanceBegin();
        int top = lua_gettop(*_mainState);

        _fieldRef.getTableRef().push();

        LuaManager::getMetatable(-1);
        lua_getfield(*_mainState, -1, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

        if(lua_isnil(*_mainState, -1)) {

            _fieldRef.getTableRef().push();

            lua_getfield(*_mainState, -1, ELYSIAN_LUA_PROXY_TYPE_METAFIELD);

            if(lua_isnil(*_mainState, -1)) {
                lua_pop(*_mainState, lua_gettop(*_mainState)-top);
                LuaManager::stackBalanceEnd();
                return nullptr;
            }

            QString registryName;
            OOLUA::pull(*_mainState, registryName);

            LuaManager::pushGlobalField(Q_CSTR(registryName));

            if(lua_isnil(*_mainState, -1)) {
                lua_pop(*_mainState, lua_gettop(*_mainState)-top);
                LuaManager::stackBalanceEnd();
                return nullptr;
            }

            //TK_LUA_TABLE_DELEGATE_FIELD_KEY
            lua_getfield(*_mainState, -1, TK_LUA_TABLE_DELEGATE_FIELD_KEY);
            if(lua_isnil(*_mainState, -1)) {
                lua_pop(*_mainState, lua_gettop(*_mainState)-top);
                LuaManager::stackBalanceEnd();
                return nullptr;
            }
        }

        int type = lua_type(*_mainState, -1);

        switch(type) {
        case LUA_TFUNCTION:
            _delegate = LuaManager::_callFunction<PropertyItemDelegate*>(nullptr, _fieldRef.getTableRef(), _fieldRef.getKey());
            break;
        case LUA_TTABLE:

            _fieldRef.getKey().push();

            lua_gettable(*_mainState, -2);

            if(lua_isnil(*_mainState, -1)) {
                lua_pop(*_mainState, lua_gettop(*_mainState)-top);
                LuaManager::stackBalanceEnd();
                return nullptr;
            }

            PropertyItemDelegate* delegate;
            OOLUA::pull(*_mainState, delegate);
            _delegate = static_cast<QStyledItemDelegate*>(delegate);
            break;

        default:
            break;

        }

        lua_pop(*_mainState, lua_gettop(*_mainState)-top);
        LuaManager::stackBalanceEnd();
    }

    return _delegate;

}

