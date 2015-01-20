#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>
#include "elysian_debug.hpp"

#include "elysian_lua_table_proxy.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"
#include "elysian_debug.hpp"

class LuaTableStackModel;

Q_DECLARE_METATYPE(OOLUA::Lua_table_ref);
Q_DECLARE_METATYPE(OOLUA::Lua_func_ref);

namespace OOLUA {
    namespace STRING {
        //Template specialization allowing OOLUA to push a QString to Lua as a character array
        template<>
        inline bool push<QString>(lua_State* state, QString const& string) {
            return OOLUA::push(state, string.toStdString().c_str());
        }

        //Template specialization allowing OOLUA to pull a character array from Lua as a QString
        template<>
        inline bool pull<QString>(lua_State* state, QString& string) {
            string = lua_tostring(state, -1);
            lua_pop(state, 1);
            return (string.isNull())? true : false;
        }
    }
}

//QStandardItem representing a Lua k,v pair
template<typename TVal, typename TKey=QString>
class LuaTableFieldItem:
    public PropertyStandardItem<TVal, LuaTableFieldItem<TVal, TKey>, false>,
    public elysian::LuaManager
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

//custom qVariant type required

class LuaTableItem: public PropertyStandardItem<QString, LuaTableItem> {
protected:
    typedef PropertyStandardItem<QString, LuaTableItem> ParentType;

    const int                       _luaRef         = LUA_NOREF;
    const int                       _luaType        = LUA_TNONE;
    mutable bool                    _populated      = false;
    const QStandardItem*            _labelItem      = nullptr;
    QHash<QString, QStandardItem*>  _fieldItems;

private slots:
    void    _onFieldChanged(QString field);

public:
            LuaTableItem(const int ref, const int type, const QStandardItem* label);
            ~LuaTableItem(void);

    QString getValue(void) const;
};

//QStandardItemModel representing a Lua table or proxied userdata
//Also needs to be a LuaTableItem!
class LuaTableStackModel: public LuaTableItem, public elysian::LuaManager, public PropertyStackModel {
    friend class LuaTableItem;
public:
    int                             _luaRef = LUA_NOREF;

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

    //Iterates over table or proxied userdata, populating model based on k,v pairs
    void _addTableFields(lua_State*const state, const int tableRef) {
        bool isUserdata         = false;
        bool nextMetamethod     = false;
        //int	_oolua_table_index_ = 0;
        int iterNum             = 0;
        OOLUA::Lua_func_ref    metamethod;

        using namespace elysian;
        using elysian::Debug;

      //  elysian::Debug::printf(elysian::Debug::VERBOSE, "_addTableFields(%d)", tableRef);
        elysian::Debug::push();

        //Helper lambda function for uniform iteration over tables and userdata
        std::function<bool()> iterFunc[2] = {
            //table iteration
            [&](void) -> bool {
              //  elysian::Debug::printf(elysian::Debug::VERBOSE, "lua_next()");
                elysian::Debug::push();

                bool retVal = (bool)lua_next(state, -2);
            //    elysian::Debug::printf(elysian::Debug::VERBOSE, "RetVal - %d", retVal);

                elysian::Debug::pop();
                return retVal;

            },
            //userdata iteration
             [&](void) -> bool {
                bool retVal = false;
             //   elysian::Debug::printf(elysian::Debug::VERBOSE, "meta.__next()");
                elysian::Debug::push();

            //   elysian::Debug::printf(elysian::Debug::VERBOSE, "nextMetamethod - %d", nextMetamethod);
               if(nextMetamethod) {

                    lua_pushvalue(state, -2);
                    lua_pushvalue(state, -2);
                    lua_remove(state, -3);
                    metamethod.push(state);
                    lua_insert(state, -3);

                    _protectedCall(2, 2, nullptr);

                    if((retVal = !lua_isnil(state, -2))) {
						//qDebug() << "SHITS PRISTINE!";
                    } else {
                        lua_pop(state, 2);
					   // qDebug() << "SHIT WAS NOT NIL!";
                    }

                } else {
                    lua_pop(state, 1);
                }

              //  elysian::Debug::printf(elysian::Debug::VERBOSE, "RetVal - %d", retVal);
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

              // elysian::Debug::printf(elysian::Debug::VERBOSE, "Pushed __property table!", nextMetamethod);

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

           // elysian::Debug::printf(elysian::Debug::VERBOSE, "[%d] - <%s, %s>", iterNum, keyName.toStdString().c_str(), valueName.toStdString().c_str());
            elysian::Debug::push();

            //If current value is iterable, recurse over its k,v pairs

            if(valueType == LUA_TTABLE || valueType == LUA_TUSERDATA) {

                lua_pushvalue(*_mainState, -1);
                int ref = luaL_ref(state, LUA_REGISTRYINDEX);
                auto* label = new QStandardItem(keyName);
                addRow(label, new LuaTableItem(ref, valueType, label));

                //pushLabel(keyName, new QStandardItem(QString(lua_typename(state, lua_type(state, -1)))));

                //lua_rawgeti(state, LUA_REGISTRYINDEX, tableRef);

                //Don't recurse, add table item to latently populate upon access!
                //lua_pushvalue(*_mainState, -1);
                //_addTableFields(*_mainState, luaL_ref(*_mainState, LUA_REGISTRYINDEX));
                //lua_pop(*_mainState, 1);



                //popLabel();

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

       //elysian::Debug::printf(elysian::Debug::VERBOSE, "%d - Popping userdata!", tableRef);
        elysian::Debug::pop();



    }

public:

                    LuaTableStackModel(void) = default;
                    LuaTableStackModel(const int ref);

    void            setLuaRef(const int ref);
    virtual void    populateModel(void) override;
private slots:
    void            _onFieldAdded(QString field);
};


inline void LuaTableStackModel::_onFieldAdded(QString /*field*/) {}

inline void LuaTableStackModel::setLuaRef(const int ref) { _luaRef = ref; }

inline LuaTableStackModel::LuaTableStackModel(const int ref): _luaRef(ref) {}

inline void LuaTableStackModel::populateModel(void) {
    insertHeaders();
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    _addTableFields(*_mainState, _luaRef);
    lua_pop(*_mainState, 1);
}

//Pull value from Lua table/userdata to update model
template<typename TVal, typename TKey>
inline TVal LuaTableFieldItem<TVal, TKey>::getValue(void) const {
    //qDebug() << "GETVAL -" << _key;
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
