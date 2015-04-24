#include <luaTableStackModel.h>
#include <elysian_lua_table_proxy.hpp>
#include <QVariant>
#include <tkArea.h>

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



//Iterates over table or proxied userdata, populating model based on k,v pairs
void LuaTableTableFieldData::insertTableFieldItems(QStandardItem*const parentItem) {

    LuaManager::stackBalanceBegin();
    bool metatable;
    bool iterable;

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
    } else iterable = false;

    if(iterable) {

        genericIterate(-1, [&](void) {
            QVariant key;
            lua_pushvalue(*_mainState, -2);
            OOLUA::pull(*_mainState, key);

            QList<QStandardItem*> items;
            auto* labelItem = (PropertyStackModel::createReadOnlyItem(key.toString()));
            auto* valueItem = new LuaTableFieldItem(_luaRef, key, labelItem);
            items.push_back(labelItem);
            items.push_back(valueItem);
            //items.push_back(

            parentItem->appendRow(items);
            valueItem->insertingModelItem();
            _fieldItems[key] = valueItem;


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

