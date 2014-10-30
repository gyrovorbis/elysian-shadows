#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>

#include "elysian_lua_table_model.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"
#include "debug.h"


class LuaTableStackModel;

template<>
inline bool OOLUA::STRING::push<QString>(lua_State* state, QString const& string) {
    return OOLUA::push(state, string.toStdString().c_str());
}

template<>
inline bool OOLUA::STRING::pull<QString>(lua_State* state, QString& string) {
    string = lua_tostring(state, -1);
    lua_pop(state, 1);
    return (string.isNull())? true : false;
}


template<typename TVal, typename TKey=QString>
class LuaTableFieldItem:
    public PropertyStandardItem<TVal, LuaTableFieldItem<TVal, TKey>, false>, public elysian::LuaManager
{
protected:
    const OOLUA::Lua_table_ref          _tableRef;
    const TKey                          _key;

public:

    LuaTableFieldItem(const OOLUA::Lua_table_ref &ref, const TKey key):
        _tableRef(ref),
        _key(key)
    {
        PropertyStandardItem<TVal, LuaTableFieldItem<TVal, TKey>>::_latentInit(this, &LuaTableFieldItem<TVal, TKey>::getValue, &LuaTableFieldItem<TVal, TKey>::setValue, UndoItem::LUA_TABLE_FIELD_ITEM, nullptr);
       // qDebug() << "FieldItem(" << key << ")";
    }

    TVal getValue(void) const;

    void setValue(const TVal val);

};


class LuaTableStackModel: public elysian::LuaManager, public PropertyStackModel {

public:
    LuaTableStackModel*             _parent = nullptr;
    const OOLUA::Lua_table_ref      _luaRef;

protected:

    template<typename TKey>
    QStandardItem* _createTableFieldItemFromStack(const OOLUA::Lua_table_ref& tableRef) {
        TKey key;
        QString labelName;
        const int valueType   = lua_type(*_mainState, -1);

        lua_pushvalue(*_mainState, -2);
        OOLUA::pull(*_mainState, key);
        //lua_pop(*_mainState, 1);
       // qDebug() << "KEY - " << key;

        switch(valueType) {
        case LUA_TSTRING:
            return new LuaTableFieldItem<QString, TKey>(tableRef, key);
        case LUA_TNUMBER:
            return new LuaTableFieldItem<float, TKey>(tableRef, key);
        case LUA_TBOOLEAN:
            return new LuaTableFieldItem<bool, TKey>(tableRef, key);
       // case LUA_TTABLE:

        //    break;
        default:
            return new QStandardItem(QString("function"));

        }
    }

    void _addTableFields(lua_State*const state, const OOLUA::Lua_table_ref& tableRef) {
        OOLUA::Table curTable(tableRef);

    //    qDebug() << "oolua_pairs_begin ";
                   // _stackBalanceBegin();
        oolua_pairs(curTable) {
            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(state, -2);
            lua_pushvalue(state, -2);
            const QString           keyName     = luaL_tolstring(state, -1, nullptr);
            QStandardItem*          valueItem;

            lua_pop(state, 2);

            //_cStackDump();

         if(valueType == LUA_TTABLE) {
                pushLabel(keyName);

                lua_pushvalue(*_mainState, -1);
                _addTableFields(*_mainState, OOLUA::Lua_table_ref(*_mainState, luaL_ref(*_mainState, LUA_REGISTRYINDEX)));

                popLabel();
            } else {
                switch(keyType) {
                case LUA_TSTRING:
                    valueItem = _createTableFieldItemFromStack<QString>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TNUMBER:
                    //Debug::Logf(Debug::CRITICAL, "\tNumber");
                    qDebug() << "\t Number";
                    valueItem = _createTableFieldItemFromStack<float>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TBOOLEAN:
                    //Debug::Logf(Debug::CRITICAL, "\tBoolean");
                    qDebug() << "\t Boolean";
                    valueItem = _createTableFieldItemFromStack<bool>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TTABLE:
                    valueItem = new QStandardItem(QString("Table"));
                    addProperty(keyName, valueItem);

                    //Debug::Logf(Debug::CRITICAL, "\tTable");
                    qDebug() << "\t Table";
                   // pushLabel(keyName);

                    //lua_pushvalue(state, -1);
                    //_addTableFields(state, OOLUA::Lua_table_ref(state, luaL_ref(state, LUA_REGISTRYINDEX)));

                    //popLabel();

                    //recurse;
                    break;

                default:

                    valueItem = new QStandardItem(QString("function"));
                    addProperty(keyName, valueItem);
                    //Debug::Logf(Debug::CRITICAL, "\tOther");
                    //qDebug() << "\t Other";
                    break;
                }
           }


            lua_pop(state, 1);
            //_cStackDump();

            //_stackBalanceEnd();
        } oolua_pairs_end()


             //   qDebug() << "oolua_pairs_end";
        //_cStackDump();

    }

public:

    LuaTableStackModel(const int ref): _luaRef(*_mainState, ref) {}

    virtual void populateModel(void);
};



inline void LuaTableStackModel::populateModel(void) {

    //Debug::Logf(Debug::CRITICAL, "BEGIN POPULATION!");
    _addTableFields(*_mainState, _luaRef);

}



template<typename TVal, typename TKey>
inline TVal LuaTableFieldItem<TVal, TKey>::getValue(void) const {
    //qDebug() << "getField[" << _key << "]";
    TVal val;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _tableRef.ref());
    OOLUA::push(*_mainState, _key);
    lua_gettable(*_mainState, -2);
    OOLUA::pull(*_mainState, val);
    //qDebug() << "\t = " << val;
    lua_pop(*_mainState, 1);

    return val;
}

template<typename TVal, typename TKey>
inline void LuaTableFieldItem<TVal, TKey>::setValue(const TVal val) {
    //Debug::Logf(Debug::CRITICAL, "\tOther[SET] - %s", _key.toStdString().c_str());
    //qDebug() << "OTHER[SET] " << _key;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _tableRef.ref());
    OOLUA::push(*_mainState, _key);
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);

    //_parentTable->set(_key, val);
}
/*

template<>
inline QString LuaTableFieldItem<QString>::getValue(void) const {
    //Debug::Logf(Debug::CRITICAL, "\tString[GET] - %s", _key.toStdString().c_str());
    qDebug() << "STRING[GET] " << _key;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _tableRef.ref());
    OOLUA::push(*_mainState, _key);
    lua_gettable(*_mainState, -2);
    //lua_getfield(*_mainState, -1, _key.toStdString().c_str());
    lua_remove(*_mainState, -2);
    return lua_tostring(*_mainState, -1);
}

template<>
inline void LuaTableFieldItem<QString>::setValue(const QString val) {
    //Debug::Logf(Debug::CRITICAL, "\String[SET] - %s", _key.toStdString().c_str());
    qDebug() << "STRING[SET] " << _key;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _tableRef.ref());
    lua_pushstring(*_mainState, val.toStdString().c_str());
    lua_setfield(*_mainState, -2, _key.toStdString().c_str());
    lua_pop(*_mainState, 1);

    //_parentTable->set(_key, val);
}


*/


#endif // ESTK_LUA_TABLE_MODEL_HPP
