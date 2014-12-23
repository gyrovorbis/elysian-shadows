#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>
#include "elysian_debug.hpp"

#include "elysian_lua_table_proxy.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"
#include "elysian_debug.hpp"

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

        //elysian::Debug::printf(elysian::Debug::VERBOSE, "ValueType - ");
        switch(valueType) {
        case LUA_TSTRING:
            return new LuaTableFieldItem<QString, TKey>(tableRef, key);
        case LUA_TNUMBER:
            return new LuaTableFieldItem<float, TKey>(tableRef, key);
        case LUA_TBOOLEAN:
            return new LuaTableFieldItem<bool, TKey>(tableRef, key);
		case LUA_TUSERDATA:
			return new QStandardItem(QString("userdata"));
        case LUA_TNIL:
            return new QStandardItem(QString("nil"));
        default:
            return new QStandardItem(QString("function"));

        }
    }

#if 0
	//Iterates over table or proxied userdata, populating model based on k,v pairs
    void _addTableFields(lua_State*const state, const int tableRef) {
        bool isUserdata         = false;
        bool nextMetamethod     = false;
        int	_oolua_table_index_ = 0;
        int iterNum             = 0;

		//Helper lambda function for uniform iteration over tables and userdata
        std::function<bool()> iterFunc[2] = {
			//table iteration
			[&](void) -> bool {
                bool retVal = (bool)lua_next(state, -2);
                qDebug() << "Lua_next() - " << retVal;
                return retVal;
            },
			//userdata iteration
             [&](void) -> bool {
                if(nextMetamethod) {
                    qDebug() << "ENTER __NEXT LAMBDA";
                    LuaManager::_cStackDump();
                    lua_pushvalue(state, -2);
                    //lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);

                    assert(lua_getmetatable(state, -1));
                    //lua_remove(state, -2);

                    if(lua_type(*_mainState, -1) != LUA_TTABLE) {

                        qDebug() << "CALLING __NEXT ON NONTABLE";


                    } else {
                        qDebug() << "CALLING NEXT ON TABLE";
                    }
                    LuaManager::_cStackDump();
                    lua_getfield(state, -1, "__next");
                    lua_remove(state, -2);
                    lua_insert(state, -2);
                    lua_pushvalue(state, -3);

					LuaManager::_protectedCall(2, 2, nullptr);

                    lua_remove(state, -3);

					if(lua_isnil(state, -2)) {
                        qDebug() << "__NEXT() - ISNIL";
                        LuaManager::_cStackDump();
						lua_pop(state, 2);
                        return false;
                    } else {
                        qDebug() << "__NEXT - TRUE";
                        return true;
                    }
                } else {
                    qDebug() << "__NEXT - Not found!";
                    lua_pop(state, 1);
                    return false;
                }
             }
        };

        _oolua_table_index_ = lua_gettop(state);
        lua_pushnil(state);

        qDebug() << "BEGIN REF(" << tableRef << ")";
        //lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);

        isUserdata = (lua_type(*_mainState, -2) == LUA_TUSERDATA);
		//Check if userdata is iterable
        if(isUserdata) {
            lua_getmetatable(*_mainState, -2);
            lua_getfield(*_mainState, -1, "__next");
            nextMetamethod = (!lua_isnil(*_mainState, -1));
            qDebug() << "IS USERDATA";
            lua_pop(*_mainState, 2);
            if(!nextMetamethod) {
                qDebug() << "HAS NO NEXT METAMETHOD";
                lua_pop(*_mainState, 1);
                return;
            } else {
                qDebug() << "NEXT METAMETHOD";
            }
        } else {
            qDebug() << "NOT USERDATA";
        }

		//Iterate over table/userdata
		while (iterFunc[isUserdata]()) {
            ++iterNum;
            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(state, -2);
            lua_pushvalue(state, -2);
            const QString           keyName     = luaL_tolstring(state, -1, nullptr);
            QStandardItem*          valueItem;

            lua_pop(state, 2);

            qDebug() << "keyName -" << keyName <<", iterNum - " << iterNum;

			//If current value is iterable, recurse over its k,v pairs

            if(valueType == LUA_TTABLE || valueType == LUA_TUSERDATA) {
                pushLabel(keyName, (valueType == LUA_TTABLE)? new QStandardItem(QString("table")) : new QStandardItem(QString("userdata")) );

                lua_pushvalue(*_mainState, -1);
                qDebug() << "keyName - TABLE/USERDATA RECURSE";
                _addTableFields(*_mainState, luaL_ref(*_mainState, LUA_REGISTRYINDEX));
                //lua_pop(*_mainState, 1);

                popLabel();

            } else {
                switch(keyType) {
                case LUA_TSTRING:
                    qDebug() << "STRING KEY ADD";
                    valueItem = _createTableFieldItemFromStack<QString>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TNUMBER:
                    qDebug() << "NUMBER KEY ADD";
                    valueItem = _createTableFieldItemFromStack<float>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TBOOLEAN:
                    qDebug() << "BOOL KEY ADD";
                    valueItem = _createTableFieldItemFromStack<bool>(tableRef);
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TTABLE:
                    qDebug() << "TABLE KEY ADD";
                    valueItem = new QStandardItem(QString("table"));
                    addProperty(keyName, valueItem);
                    break;
				case LUA_TUSERDATA:
                    qDebug() << "USERDATA KEY ADD";
					valueItem = new QStandardItem(QString("userdata"));
					addProperty(keyName, valueItem);
					break;
                default:
                    qDebug() << "DEFAULT KEY ADD";
                    valueItem = new QStandardItem(QString("function"));
                    break;
                }
           }

            qDebug() << "popped value for " << keyName;
            lua_pop(state, 1);
            LuaManager::_cStackDump();
			//Pop value, leave key
        }

        qDebug() << "popping userdata for " << tableRef;
		//Pop table/userdata
        //lua_pop(state, 1);
        LuaManager::_cStackDump();

    }
#else

    //Iterates over table or proxied userdata, populating model based on k,v pairs
    void _addTableFields(lua_State*const state, const int tableRef) {
        bool isUserdata         = false;
        bool nextMetamethod     = false;
        //int	_oolua_table_index_ = 0;
        int iterNum             = 0;
        OOLUA::Lua_func_ref    metamethod;

        using namespace elysian;
        using elysian::Debug;

        elysian::Debug::printf(elysian::Debug::VERBOSE, "_addTableFields(%d)", tableRef);
        elysian::Debug::push();

        //Helper lambda function for uniform iteration over tables and userdata
        std::function<bool()> iterFunc[2] = {
            //table iteration
            [&](void) -> bool {
                elysian::Debug::printf(elysian::Debug::VERBOSE, "lua_next()");
                elysian::Debug::push();

                bool retVal = (bool)lua_next(state, -2);
                elysian::Debug::printf(elysian::Debug::VERBOSE, "RetVal - %d", retVal);

                elysian::Debug::pop();
                return retVal;

            },
            //userdata iteration
             [&](void) -> bool {
                bool retVal = false;
                elysian::Debug::printf(elysian::Debug::VERBOSE, "meta.__next()");
                elysian::Debug::push();

               elysian::Debug::printf(elysian::Debug::VERBOSE, "nextMetamethod - %d", nextMetamethod);
               if(nextMetamethod) {

                    lua_pushvalue(state, -2);
                    lua_pushvalue(state, -2);
                    lua_remove(state, -3);
                    metamethod.push(state);
                    lua_insert(state, -3);

                    LuaManager::_protectedCall(2, 2, nullptr);

                    if((retVal = !lua_isnil(state, -2))) {
						//qDebug() << "SHITS PRISTINE!";
                    } else {
                        lua_pop(state, 2);
					   // qDebug() << "SHIT WAS NOT NIL!";
                    }

                } else {
                    lua_pop(state, 1);
                }

                elysian::Debug::printf(elysian::Debug::VERBOSE, "RetVal - %d", retVal);
                elysian::Debug::pop();
                return retVal;
            }


        };

        lua_pushnil(state);
        isUserdata = (lua_type(*_mainState, -2) == LUA_TUSERDATA);
        //Check if userdata is iterable

        //elysian::Debug::printf(elysian::Debug::VERBOSE, "isUserdata - %d", isUserdata);
        if(isUserdata) {
            lua_getmetatable(*_mainState, -2);
            lua_getfield(*_mainState, -1, "__next");
            nextMetamethod = (!lua_isnil(*_mainState, -1));

            //elysian::Debug::printf(elysian::Debug::VERBOSE, "nextMetamethod - %d", nextMetamethod);
            if(nextMetamethod) {

                metamethod.pull(*_mainState);
                lua_pop(*_mainState, 1);

                //propTableRef.push(*_mainState);
                //lua_insert(*_mainState, -2);

                elysian::Debug::printf(elysian::Debug::VERBOSE, "Pushed __property table!", nextMetamethod);

            } else {
                lua_pop(*_mainState, 2);
                //return;
            }
        }

        //Iterate over table/userdata
        while (iterFunc[isUserdata]()) {
            LuaManager::_stackBalanceBegin();
            ++iterNum;
            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(state, -2);
            lua_pushvalue(state, -2);
            const QString           keyName     = luaL_tolstring(state, -1, nullptr);
            lua_pushvalue(state, -3);
            const QString           valueName   = luaL_tolstring(state, -1, nullptr);
            QStandardItem*          valueItem;

            lua_pop(state, 4);

            elysian::Debug::printf(elysian::Debug::VERBOSE, "[%d] - <%s, %s>", iterNum, keyName.toStdString().c_str(), valueName.toStdString().c_str());
            elysian::Debug::push();

            //If current value is iterable, recurse over its k,v pairs

            if(valueType == LUA_TTABLE || valueType == LUA_TUSERDATA) {
                pushLabel(keyName, new QStandardItem(QString(lua_typename(state, lua_type(state, -1)))));

                //lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);
                lua_pushvalue(*_mainState, -1);

                _addTableFields(*_mainState, luaL_ref(*_mainState, LUA_REGISTRYINDEX));
                //lua_pop(*_mainState, 1);

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
                    valueItem = new QStandardItem(QString("table"));
                    addProperty(keyName, valueItem);
                    break;
                case LUA_TUSERDATA:
                    addProperty(keyName, new QStandardItem(QString("userdata")));
                    break;
                default:
                    valueItem = new QStandardItem(QString("function"));
                    break;
                }
           }

           LuaManager::_stackBalanceEnd();
            lua_pop(state, 1);
            //LuaManager::_cStackDump();
            //Pop value, leave key

            elysian::Debug::pop();
        }
/*
        if(isUserdata && nextMetamethod) {
            elysian::Debug::printf(elysian::Debug::VERBOSE, "%d - Popping __property table!", tableRef);
            //lua_pop(state, 1);
        } else {
            elysian::Debug::printf(elysian::Debug::VERBOSE, "%d - Not popping __property table!", tableRef);
        }
        //LuaManager::_cStackDump();
*/

        elysian::Debug::printf(elysian::Debug::VERBOSE, "%d - Popping userdata!", tableRef);
        elysian::Debug::pop();



    }

#endif

public:

    LuaTableStackModel(const int ref): _luaRef(ref) {}
    virtual void populateModel(void) override;
};



inline void LuaTableStackModel::populateModel(void) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    _addTableFields(*_mainState, _luaRef);
    lua_pop(*_mainState, 1);
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
