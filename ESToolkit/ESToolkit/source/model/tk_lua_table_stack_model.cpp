#include <tk_lua_table_stack_model.h>
#include <asset/tk_lua_table_proxy.h>
#include <QVariant>
#include <asset/tk_area.h>
#include <estk/tk_lua_wrappers.h>
#include <core/tk_lua_qt_utils.h>

LuaTableFieldItem::LuaTableFieldItem(const int ref, const QVariant key, QStandardItem*const labelItem):
    _labelItem(labelItem),
    _luaRef(ref),
    _key(key)
{}

void LuaTableFieldItem::insertingModelItem(void) {
    _latentInit(this, &LuaTableFieldItem::getValue, &LuaTableFieldItem::setValue,
                    UndoItem::LUA_TABLE_FIELD_ITEM, &getCurrentArea()->getUndoStack());
}


void LuaTableTableFieldData::onFieldChanged(const QVariant key) {
    auto* item = _fieldItems[key];
    item->setDirty();
    item->emitDataChanged();
}

void LuaTableTableFieldData::onFieldAdded(const QVariant key) {
    insertTableFieldItem(key);
}

void LuaTableStackModel::populateModel(void) {
    if(_populated) return;
    setHorizontalHeaderItem(0, new QStandardItem("Name"));
    setHorizontalHeaderItem(1, new QStandardItem("Value"));
    //setHorizontalHeaderItem(2, new QStandardItem("Type"));

    QList<QStandardItem*> items;
    _parentItem = PropertyStackModel::createReadOnlyItem("Lua Table");//ESTkStandardItem(QString("Lua Table"));
    items.push_back(_parentItem);
    items.push_back(PropertyStackModel::createReadOnlyItem());
    //items.push_back(PropertyStackModel::createReadOnlyItem());
    QStandardItemModel::appendRow(items);


    _tableData = new LuaTableTableFieldData(_luaRef);
    _tableData->insertTableFieldItems(_parentItem);

    _populated = true;
}

void LuaTableTableFieldData::insertTableFieldItem(const QVariant key) {
    QList<QStandardItem*> items;
    auto* labelItem = (PropertyStackModel::createReadOnlyItem(key.toString()));
    auto* valueItem = new LuaTableFieldItem(_luaRef, key, labelItem);
    items.push_back(labelItem);
    items.push_back(valueItem);

    _parentItem->appendRow(items);
    valueItem->insertingModelItem();
    _fieldItems[key] = valueItem;

}


//Iterates over table or proxied userdata, populating model based on k,v pairs
void LuaTableTableFieldData::insertTableFieldItems(QStandardItem*const parentItem) {

    LuaManager::stackBalanceBegin();
    bool metatable;
    bool iterable;

    _parentItem = parentItem; //Store reference to its own parent

    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);   //push table ref

    int valueType = lua_type(*_mainState, -1);

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
        auto* ud = TkLuaTableUserdata::inject(*_mainState, _luaRef);
        connect(ud, SIGNAL(fieldChanged(QVariant)), this, SLOT(onFieldChanged(QVariant)));
        connect(ud, SIGNAL(fieldAdded(QVariant)), this, SLOT(onFieldAdded(QVariant)));
    } else iterable = false;

    if(iterable) {

        genericIterate(-1, [&](void) {
            QVariant key;
            lua_pushvalue(*_mainState, -2);
            OOLUA::pull(*_mainState, key);

            insertTableFieldItem(key);

        });

    }

    lua_pop(*_mainState, 1);                                //pop table ref
    LuaManager::stackBalanceEnd();
}

LuaTableFieldItem::~LuaTableFieldItem(void) {
    delete _tableData;
}

inline void LuaTableFieldItem::setDirty(const bool val) {
    if(val) {
        for(int i = 0; i < rowCount(); ++i) {
            auto&& row = takeRow(i);
            for(int j = 0; j < row.size(); ++j) {
                delete row[j];
            }
        }
    }
}

LuaTableTableFieldData::~LuaTableTableFieldData(void) {
    //We have to clear all the child QStandardItems or shit will reest

    luaL_unref(*_mainState, LUA_REGISTRYINDEX, _luaRef);

}

QStyledItemDelegate* LuaTableFieldItem::getStyledItemDelegate(void) const {

    if(!_delegate) {

        LuaManager::stackBalanceBegin();
        int top = lua_gettop(*_mainState);

#if 11
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
        lua_getmetatable(*_mainState, -1);
        LuaManager::getMetatable(-1);
        lua_getfield(*_mainState, -1, ELYSIAN_LUA_TABLE_STACK_MODEL_DELEGATE_METAFIELD);
#else
        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
        //lua_getmetatable(*_mainState, -1);
        lua_getfield(*_mainState, -1, ELYSIAN_LUA_PROXY_TYPE_METAFIELD);

        if(lua_isnil(*_mainState, -1)) {
            lua_pop(*_mainState, lua_gettop(*_mainState)-top);
            LuaManager::stackBalanceEnd();
            return nullptr;
        }

        QString registryName;
        OOLUA::pull(*_mainState, registryName);

        LuaManager::getGlobal(Q_CSTR(registryName));

        if(lua_isnil(*_mainState, -1)) {
            lua_pop(*_mainState, lua_gettop(*_mainState)-top);
            LuaManager::stackBalanceEnd();
            return nullptr;
        }

        lua_getfield(*_mainState, -1, ELYSIAN_LUA_TABLE_STACK_MODEL_DELEGATE_METAFIELD);


#endif

        if(lua_isnil(*_mainState, -1)) {
            lua_pop(*_mainState, lua_gettop(*_mainState)-top);
            LuaManager::stackBalanceEnd();
            return nullptr;
        }

        OOLUA::push(*_mainState, _key);
        lua_gettable(*_mainState, -2);

        if(lua_isnil(*_mainState, -1)) {
            lua_pop(*_mainState, lua_gettop(*_mainState)-top);
            LuaManager::stackBalanceEnd();
            return nullptr;
        }

        PropertyItemDelegate* delegate;
        OOLUA::pull(*_mainState, delegate);
        _delegate = static_cast<QStyledItemDelegate*>(delegate);

        lua_pop(*_mainState, lua_gettop(*_mainState)-top);
        LuaManager::stackBalanceEnd();

    }

    return _delegate;

}

