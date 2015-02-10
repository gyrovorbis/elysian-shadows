#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>
#include "elysian_debug.hpp"

#include "elysian_lua_table_proxy.hpp"
#include "propertyStackModel.h"
#include "propertyStandardItem.h"
#include "elysian_debug.hpp"

class LuaTableStackModel;


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
    const int       _luaRef = LUA_NOREF;
    const TKey      _key;

public:
                    LuaTableFieldItem(const int ref, const TKey key);
    virtual         ~LuaTableFieldItem(void) = default;
    virtual void    insertingModelItem(void) override;

    TVal            getValue(void) const;
    void            setValue(const TVal val);

};

//custom qVariant type required

class LuaTableItem: public QObject, public PropertyStandardItem<QString, LuaTableItem>, public elysian::LuaManager {
    Q_OBJECT
protected:
    typedef PropertyStandardItem<QString, LuaTableItem> ParentType;

    int                                 _luaRef         = LUA_NOREF;
    int                                 _luaType        = LUA_TNONE;
    bool                                _proxyTable     = false;
    mutable bool                        _populated      = false;
    OOLUA::Lua_func_ref                 _nextMetamethod;
    const QStandardItem*                _labelItem      = nullptr;
    QHash<QString, ESTkStandardItem*>   _fieldItems;

public slots:
    void    onFieldChanged(QString field);
  //  void    _onFieldAdded(QString field);
public:
            LuaTableItem(const int ref, const QStandardItem* label=nullptr);
            ~LuaTableItem(void); //delete reference

    virtual void insertingModelItem(void) override;

    QString getValue(void) const;
   // void    populateFields(void);


    //Receives key type and table reference, then creates FieldItem for pair
    template<typename TKey>
    ESTkStandardItem* _createTableFieldItemFromStack(const int tableRef) {
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
            return new ESTkStandardItem(QString("userdata"));
        case LUA_TNIL:
            return new ESTkStandardItem(QString("nil"));
        default:
            return new ESTkStandardItem(QString("function"));

        }

        //lua_pop(*_mainState, 1); //pop value from stack
    }

    //Iterates over table or proxied userdata, populating model based on k,v pairs
    void _populateFieldItems(PropertyStackModel*const stackModel) {
        stackModel->pushItem(const_cast<QStandardItem*>(_labelItem));

        using namespace elysian;
        using elysian::Debug;

      //  elysian::Debug::printf(elysian::Debug::VERBOSE, "_addTableFields(%d)", tableRef);
        //elysian::Debug::push();

        //Helper lambda function for uniform iteration over tables and userdata

        lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);

        //add metatable
        LuaManager::_stackBalanceBegin();
        if(getMetatable(-1)) {
            int metaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
            auto* label = new QStandardItem("__metatable");
            auto* valueItem = new LuaTableItem(metaRef, label);
            stackModel->addRow(label, valueItem);
        }
        LuaManager::_stackBalanceEnd();

        _luaType = lua_type(*_mainState, -1);
        //Check if userdata is iterable

        //elysian::Debug::printf(elysian::Debug::VERBOSE, "isUserdata - %d", isUserdata);
        if(_luaType == LUA_TUSERDATA) {
            //check if iterable and shit

        } else {
            auto* ud = TkLuaTableUserdata::inject(*_mainState, _luaRef);
            connect(ud, SIGNAL(fieldChanged(QString)), this, SLOT(onFieldChanged(QString)));
        }


        lua_getglobal(*_mainState, "pairs");
        lua_pushvalue(*_mainState, -2);
        _protectedCall(1, 3, nullptr);
        lua_pushvalue(*_mainState, -3);
        lua_insert(*_mainState, -4);

        lua_pushvalue(*_mainState, -2);
        lua_insert(*_mainState, -4);

        //LuaManager::_cStackDump();

        do {
            // LuaManager::_cStackDump();

            _protectedCall(2, 2, nullptr);

            //LuaManager::_cStackDump();

            lua_pushvalue(*_mainState, -4);
            lua_insert(*_mainState, -5);
            lua_pushvalue(*_mainState, -3);
            lua_insert(*_mainState, -5);

            //LuaManager::_cStackDump();

            if(lua_isnil(*_mainState, -2)) break;

            LuaManager::_stackBalanceBegin();

            const int valueType   = lua_type(*_mainState, -1);
            const int               keyType     = lua_type(*_mainState, -2);
            lua_pushvalue(*_mainState, -2);
            const QString           keyName     = luaL_tolstring(*_mainState, -1, nullptr);
            lua_pushvalue(*_mainState, -3);
            const QString           valueName   = luaL_tolstring(*_mainState, -1, nullptr);
            ESTkStandardItem*       valueItem;

            //LuaManager::_cStackDump();

            lua_pop(*_mainState, 4);

           //elysian::Debug::printf(elysian::Debug::VERBOSE, "<%s, %s>", keyName.toStdString().c_str(), valueName.toStdString().c_str());
            //elysian::Debug::push();

            //If current value is iterable, recurse over its k,v pairs

            if(valueType == LUA_TTABLE || valueType == LUA_TUSERDATA) {

                LuaManager::_stackBalanceBegin();
                lua_pushvalue(*_mainState, -1);
                int ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
                auto* label = new QStandardItem(keyName);
                valueItem = new LuaTableItem(ref, label);

                stackModel->addRow(label, valueItem);
                LuaManager::_stackBalanceEnd();
            } else {
                LuaManager::_stackBalanceBegin();
                //LuaManager::_cStackDump();
                switch(keyType) {
                case LUA_TSTRING:
                    valueItem = _createTableFieldItemFromStack<QString>(_luaRef);
                    break;
                case LUA_TNUMBER:
                    valueItem = _createTableFieldItemFromStack<float>(_luaRef);
                    break;
                case LUA_TBOOLEAN:
                    valueItem = _createTableFieldItemFromStack<bool>(_luaRef);
                    break;
                case LUA_TTABLE:
                    valueItem = new ESTkStandardItem(QString("table"));
                    break;
                case LUA_TUSERDATA:
                    valueItem = new ESTkStandardItem(QString("userdata"));
                    break;
                default:
                    valueItem = new ESTkStandardItem(QString("function"));
                    break;
                }
                //LuaManager::_cStackDump();
                stackModel->addProperty(keyName, valueItem);
                //LuaManager::_cStackDump();
                LuaManager::_stackBalanceEnd();
            }

            _fieldItems[keyName] = valueItem;

            LuaManager::_stackBalanceEnd();
            lua_pop(*_mainState, 1);
            //LuaManager::_cStackDump();
            //Pop value, leave key

            //elysian::Debug::pop();
        } while(1);

        //elysian::Debug::pop();

        stackModel->popItem();

        lua_pop(*_mainState, 1);
    }

};

//QStandardItemModel representing a Lua table or proxied userdata
//Also needs to be a LuaTableItem!
class LuaTableStackModel: public LuaTableItem, public PropertyStackModel {
    friend class LuaTableItem;
public:
   // int                             _luaRef = LUA_NOREF;

protected:

public:

                    LuaTableStackModel(void) = default;
                    LuaTableStackModel(const int ref);

    void            setLuaRef(const int ref);
    virtual void    populateModel(void) override;

};

inline void LuaTableStackModel::setLuaRef(const int ref) { _luaRef = ref; }

inline LuaTableStackModel::LuaTableStackModel(const int ref): LuaTableItem(ref) {}

inline void LuaTableStackModel::populateModel(void) {
    if(!_labelItem) {
        _labelItem = new ESTkStandardItem(QString("Lua Table"));
        QList<QStandardItem*> items;
        items.push_back(const_cast<QStandardItem*>(_labelItem));
        items.push_back(new QStandardItem(""));
        QStandardItemModel::appendRow(items);
    }
    _populateFieldItems(this);
    popLabel();
}

template<typename TVal, typename TKey>
inline LuaTableFieldItem<TVal, TKey>::LuaTableFieldItem(const int ref, const TKey key):
    _luaRef(ref),
    _key(key)
{}

template<typename TVal, typename TKey>
inline void LuaTableFieldItem<TVal, TKey>::insertingModelItem(void) {
    PropertyStandardItem<TVal, LuaTableFieldItem<TVal, TKey>>::
        _latentInit(this, &LuaTableFieldItem<TVal, TKey>::getValue, &LuaTableFieldItem<TVal, TKey>::setValue,
                    UndoItem::LUA_TABLE_FIELD_ITEM, nullptr);
}

//Pull value from Lua table/userdata to update model
template<typename TVal, typename TKey>
inline TVal LuaTableFieldItem<TVal, TKey>::getValue(void) const {
    //qDebug() << "GETVAL -" << _key;
    LuaManager::_stackBalanceBegin();
    TVal val;
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    //LuaManager::_cStackDump();
    lua_gettable(*_mainState, -2);
   // LuaManager::_cStackDump();
    OOLUA::pull(*_mainState, val);
    lua_pop(*_mainState, 1);
    LuaManager::_stackBalanceEnd();
    return val;
}

//Push value from model to update Lua table/userdata
template<typename TVal, typename TKey>
inline void LuaTableFieldItem<TVal, TKey>::setValue(const TVal val) {
    LuaManager::_stackBalanceBegin();
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
    LuaManager::_stackBalanceEnd();
}

#endif // ESTK_LUA_TABLE_MODEL_HPP
