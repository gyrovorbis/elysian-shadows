#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>

#include "elysian_lua_table_model.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"


class LuaTableStackModel;

template<typename TVal>
class LuaTableFieldItem:
    public PropertyStandardItem<TVal, LuaTableFieldItem<TVal>, true>, public elysian::LuaManager
{
protected:
    const LuaTableStackModel*           _parentTable     = nullptr;
    const QString                       _key;

public:

    LuaTableFieldItem(LuaTableStackModel*const parent, const QString key):
        PropertyStandardItem<TVal, LuaTableFieldItem<TVal>, true>(this, &LuaTableFieldItem<TVal>::getValue, &LuaTableFieldItem<TVal>::setValue, UndoItem::LUA_TABLE_FIELD_ITEM, nullptr),
        _parentTable(parent),
        _key(key)
    {}

    TVal getValue(void) const;

    void setValue(const TVal val);

};


class LuaTableStackModel: public elysian::LuaManager, public PropertyStackModel {

public:
    LuaTableStackModel*     _parent = nullptr;
    int                     _luaRef = LUA_NOREF;

public:

    LuaTableStackModel(int ref): _luaRef(ref) {}



    virtual void populateModel(void);
};



inline void LuaTableStackModel::populateModel(void) {

    const std::function<void (lua_State*const, const int)> addTableFields = [&](lua_State*const state, const int ref) {
        OOLUA::Table curTable(OOLUA::Lua_table_ref(state, ref));

        oolua_pairs(curTable)
        {
            const int               keyType     = lua_type(state, -2);
            const int               valueType   = lua_type(state, -1);
            const QString           keyName     = luaL_tolstring(state, -2, nullptr);
            QStandardItem*          valueItem;

            switch(valueType) {
            case LUA_TSTRING:
                valueItem = new LuaTableFieldItem<QString>(this, keyName);
                addProperty(keyName, valueItem);
                break;
            case LUA_TNUMBER:
                valueItem = new LuaTableFieldItem<float>(this, keyName);
                addProperty(keyName, valueItem);
                break;
            case LUA_TBOOLEAN:
                valueItem = new LuaTableFieldItem<bool>(this, keyName);
                addProperty(keyName, valueItem);
                break;
            case LUA_TTABLE:
                pushLabel(keyName);

                lua_pushvalue(state, -1);
                addTableFields(state, luaL_ref(state, -1));

                popLabel();

                //recurse;
                break;

            default:
                break;
            }


            lua_pop(state, 2);
        }
        oolua_pairs_end()

    };

    addTableFields(*_mainState, _luaRef);

}



template<typename TVal>
inline TVal LuaTableFieldItem<TVal>::getValue(void) const {
    TVal val;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _parentTable->_luaRef);
    lua_getfield(*_mainState, -1, _key.toStdString().c_str());
    lua_remove(*_mainState, -2);
    OOLUA::pull(*_mainState, val);
    return val;
}

template<typename TVal>
inline void LuaTableFieldItem<TVal>::setValue(const TVal val) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _parentTable->_luaRef);
    OOLUA::push(*_mainState, val);
    lua_setfield(*_mainState, -1, _key.toStdString().c_str());
    lua_pop(*_mainState, -1);

    //_parentTable->set(_key, val);
}


template<>
inline QString LuaTableFieldItem<QString>::getValue(void) const {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _parentTable->_luaRef);
    lua_getfield(*_mainState, -1, _key.toStdString().c_str());
    lua_remove(*_mainState, -2);
    return lua_tostring(*_mainState, -1);
}

template<>
inline void LuaTableFieldItem<QString>::setValue(const QString val) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _parentTable->_luaRef);
    lua_pushstring(*_mainState, val.toStdString().c_str());
    lua_setfield(*_mainState, -1, _key.toStdString().c_str());
    lua_pop(*_mainState, -1);

    //_parentTable->set(_key, val);
}





#endif // ESTK_LUA_TABLE_MODEL_HPP
