#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>

#include "elysian_lua_table_proxy.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"
#include "debug.h"

class LuaTableStackModel;

//Template specialization allowing OOLUA to push a QString to Lua as a character array
template<>
inline bool OOLUA::STRING::push<QString>(lua_State* state, QString const& string) {
    return OOLUA::push(state, string.toStdString().c_str());
}

//Template specialization allowing OOLUA to pull a character array from Lua as a QString
template<>
inline bool OOLUA::STRING::pull<QString>(lua_State* state, QString& string) {
    string = lua_tostring(state, -1);
    lua_pop(state, 1);
    return (string.isNull())? true : false;
}

//QStandardItem representing a Lua k,v pair
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
		PropertyStandardItem<TVal, LuaTableFieldItem<TVal, TKey>>::
			_latentInit(this, &LuaTableFieldItem<TVal, TKey>::getValue, &LuaTableFieldItem<TVal, TKey>::setValue,
						UndoItem::LUA_TABLE_FIELD_ITEM, nullptr);
    }

    TVal getValue(void) const;
    void setValue(const TVal val);

};

//QStandardItemModel representing a Lua table or proxied userdata
class LuaTableStackModel: public elysian::LuaManager, public PropertyStackModel {
public:
    LuaTableStackModel*             _parent = nullptr;
    const int                       _luaRef = LUA_NOREF;

protected:

	//Receives key type and table reference, then creates FieldItem for pair
    template<typename TKey>
    QStandardItem* _createTableFieldItemFromStack(const int tableRef) {
        TKey key;
        QString labelName;
        const int valueType   = lua_type(*_mainState, -1);

		//retrieve key value
        lua_pushvalue(*_mainState, -2);
        OOLUA::pull(*_mainState, key);

        switch(valueType) {
        case LUA_TSTRING:
            return new LuaTableFieldItem<QString, TKey>(tableRef, key);
        case LUA_TNUMBER:
            return new LuaTableFieldItem<float, TKey>(tableRef, key);
        case LUA_TBOOLEAN:
            return new LuaTableFieldItem<bool, TKey>(tableRef, key);
		case LUA_TUSERDATA:
			return new QStandardItem(QString("userdata"));
        default:
            return new QStandardItem(QString("function"));

        }
    }

	//Iterates over table or proxied userdata, populating model based on k,v pairs
    void _addTableFields(lua_State*const state, const int tableRef) {
        bool isUserdata         = false;
        bool nextMetamethod     = false;
        int	_oolua_table_index_ = 0;

		//Helper lambda function for uniform iteration over tables and userdata
        std::function<bool()> iterFunc[2] = {
			//table iteration
			[&](void) -> bool {
                return (bool)lua_next(state, _oolua_table_index_);
            },
			//userdata iteration
             [&](void) -> bool {
                if(nextMetamethod) {
                    const int depth = lua_gettop(state);

                    lua_pushvalue(state, -2);
                    lua_insert(state, -3);

                    lua_getmetatable(state, _oolua_table_index_);

                    lua_getfield(state, -1, "__next");
                    lua_insert(state, -4);
                    lua_pop(state, 1);
					LuaManager::_protectedCall(2, 2, nullptr);

					if(lua_isnil(state, -2)) {
                        lua_pop(state, 2);
                        return false;
                    } else return true;
				} else return false;
            }
        };

        lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);

        isUserdata = (lua_type(*_mainState, -1) == LUA_TUSERDATA);
		//Check if userdata is iterable
        if(isUserdata) {
            lua_getmetatable(*_mainState, -1);
            lua_getfield(*_mainState, -1, "__next");
            nextMetamethod = (!lua_isnil(*_mainState, -1));
            lua_pop(*_mainState, 2);
        }

        _oolua_table_index_ = lua_gettop(state);
        lua_pushnil(state);
		//Iterate over table/userdata
		while (iterFunc[isUserdata]()) {
            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(state, -2);
            lua_pushvalue(state, -2);
            const QString           keyName     = luaL_tolstring(state, -1, nullptr);
            QStandardItem*          valueItem;

            lua_pop(state, 2);

			//If current value is iterable, recurse over its k,v pairs
			if(valueType == LUA_TTABLE || (valueType == LUA_TUSERDATA && nextMetamethod)) {
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
                    valueItem = _createTableFieldItemFromStack<float>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TBOOLEAN:
                    valueItem = _createTableFieldItemFromStack<bool>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TTABLE:
                    valueItem = new QStandardItem(QString("Table"));
                    addProperty(keyName, valueItem);
                    break;
				case LUA_TUSERDATA:
					valueItem = new QStandardItem(QString("userdata"));
					addProperty(keyName, valueItem);
					break;
                default:
                    valueItem = new QStandardItem(QString("function"));
                    break;
                }
           }

			//Pop value, leave key
            lua_pop(state, 1);
        }

		//Pop table/userdata being iterated over
        lua_pop(state, 1);

    }

public:

    LuaTableStackModel(const int ref): _luaRef(ref) {}
    virtual void populateModel(void) override;
};



inline void LuaTableStackModel::populateModel(void) {
    _addTableFields(*_mainState, _luaRef);
}

//Pull value from Lua table/userdata to update model
template<typename TVal, typename TKey>
inline TVal LuaTableFieldItem<TVal, TKey>::getValue(void) const {
    TVal val;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    lua_gettable(*_mainState, -2);
    OOLUA::pull(*_mainState, val);
    lua_pop(*_mainState, 1);
    return val;
}

//Push value from model to update Lua table/userdata
template<typename TVal, typename TKey>
inline void LuaTableFieldItem<TVal, TKey>::setValue(const TVal val) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
}

#endif // ESTK_LUA_TABLE_MODEL_HPP
