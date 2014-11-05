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
    const int                           _luaRef = LUA_NOREF;
    const TKey                          _key;

public:

    LuaTableFieldItem(const int ref, const TKey key):
        _luaRef(ref),
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
    const int                       _luaRef = LUA_NOREF;

protected:

    template<typename TKey>
    QStandardItem* _createTableFieldItemFromStack(const int tableRef) {
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

    void _addTableFields(lua_State*const state, const int tableRef) {
        bool isUserdata         = false;
        bool nextMetamethod     = false;
        int	_oolua_table_index_ = 0;



        std::function<bool()> iterFunc[2] = {
            [&](void) -> bool {
                return (bool)lua_next(state, _oolua_table_index_);
            },
             [&](void) -> bool {
                if(nextMetamethod) {
                    const int depth = lua_gettop(state);

                    lua_pushvalue(state, -2);
                    lua_insert(state, -3);

                    lua_getmetatable(state, _oolua_table_index_);

                    qDebug() << "PUSHED METATABLE";
                    LuaManager::_cStackDump();

                    lua_getfield(state, -1, "__next");
                    lua_insert(state, -4);
                    lua_pop(state, 1);
                    LuaManager::_protectedCall(2, 2, nullptr);
                    if(lua_gettop(state) < depth) {
                        lua_pop(state, 2);
                        return false;
                    } else return true;
                } else {
                    return false;
                }


            }
        };


        //OOLUA::Table curTable(tableRef);

    //    qDebug() << "oolua_pairs_begin ";
                   // _stackBalanceBegin();
        //oolua_pairs(curTable)

        //OOLUA::push(lvm, table);
        lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);

        isUserdata = (lua_type(*_mainState, -1) == LUA_TUSERDATA);
        if(isUserdata) {
            lua_getmetatable(*_mainState, -1);
            lua_getfield(*_mainState, -1, "__next");
            nextMetamethod = (!lua_isnil(*_mainState, -1));
            lua_pop(*_mainState, 2);
        }

        _oolua_table_index_ = lua_gettop(state);
        lua_pushnil(state);
        while (iterFunc[isUserdata]())
        {
            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(state, -2);
            lua_pushvalue(state, -2);
            const QString           keyName     = luaL_tolstring(state, -1, nullptr);
            QStandardItem*          valueItem;

            qDebug() << "KEY " << keyName;

            lua_pop(state, 2);

         if(valueType == LUA_TTABLE || valueType == LUA_TUSERDATA) {
                pushLabel(keyName);

                lua_pushvalue(*_mainState, -1);
                _addTableFields(*_mainState, luaL_ref(*_mainState, LUA_REGISTRYINDEX));

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
        }
        lua_pop(state, 1);


             //   qDebug() << "oolua_pairs_end";
        //_cStackDump();

    }

public:

    LuaTableStackModel(const int ref): _luaRef(ref) {}

    virtual void populateModel(void) override;
};



inline void LuaTableStackModel::populateModel(void) {

    //Debug::Logf(Debug::CRITICAL, "BEGIN POPULATION!");
    _addTableFields(*_mainState, _luaRef);

}



template<typename TVal, typename TKey>
inline TVal LuaTableFieldItem<TVal, TKey>::getValue(void) const {
    //qDebug() << "getField[" << _key << "]";
    TVal val;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
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
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
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
