#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>

#include <tk_asset_manager.h>
#include <elysian_lua_manager.hpp>
#include <tk_lua_qt_utils.h>
#include <tk_property_stack_model.h>
#include <tk_property_standard_item.h>

class LuaTableStackModel;
struct LuaTableTableFieldData;


//QStandardItem representing a Lua k,v pair
class LuaTableFieldItem:
    public PropertyStandardItem<QVariant, LuaTableFieldItem, true>,
    public elysian::LuaManager,
    public AssetManager
{
    friend struct LuaTableTableFieldData;
protected:
    mutable LuaTableTableFieldData* _tableData      = nullptr;
    QStandardItem*                  _labelItem      = nullptr;
    const int                       _luaRef         = LUA_NOREF;
    const QVariant                  _key;

public:
                            LuaTableFieldItem(const int ref, const QVariant key, QStandardItem*const labelItem);
    virtual                 ~LuaTableFieldItem(void);
    virtual void            insertingModelItem(void) override;

    QVariant                getValue(void) const;
    void                    setValue(const QVariant val);

    void                    setDirty(const bool val=true);
    LuaTableTableFieldData* getTableData(void) const;

};


//Aggregating "Table" data away, so primitive-type field items don't have all this overhead.
struct LuaTableTableFieldData: public QObject, public LuaManager {
    Q_OBJECT
    friend class LuaTableFieldItem;
public:
    QHash<QVariant, LuaTableFieldItem*> _fieldItems;
    int                                 _luaRef = LUA_NOREF;

                                        LuaTableTableFieldData(const int tableRef);
    virtual                             ~LuaTableTableFieldData(void) override;

    void                                insertTableFieldItems(QStandardItem* parentItem);

public Q_SLOTS:
    void                                onFieldChanged(const QVariant key);
    //void                                onFieldAdded(const QVariant key);

};


class LuaTableStackModel: public PropertyStackModel, public LuaManager {
    friend class LuaTableItem;

    LuaTableTableFieldData*     _tableData      = nullptr;
    QStandardItem*              _parentItem     = nullptr;
    int                         _luaRef         = LUA_NOREF;
    bool                        _populated      = false;

public:
                    LuaTableStackModel(const int ref);

    void            setLuaRef(const int ref);
    virtual void    populateModel(void) override;
    void            setLabelItemText(const QString string);

};






//==== INLINEZ ======

//Pull value from Lua table/userdata to update model
inline QVariant LuaTableFieldItem::getValue(void) const {
    LuaManager::stackBalanceBegin();

    QVariant    val;
    int         valType;
    int         tableRef;

    //retrieve field
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    lua_gettable(*_mainState, -2);

    //check type
    valType = lua_type(*_mainState, -1);

    //Let QVariant handle all primitive types
    if(valType != LUA_TTABLE && valType != LUA_TUSERDATA) {
        OOLUA::pull(*_mainState, val);
    //Table/Userdata fields are out-of-date
    } else if(!_tableData) {
        //val.setValue(QString(lua_typename(*_mainState, valType)));
        tableRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
        _tableData = new LuaTableTableFieldData(tableRef);
        _tableData->insertTableFieldItems(_labelItem);
    } //else Table/userdata fields should be up-to-date
    else {
         //val.setValue(QString(lua_typename(*_mainState, valType)));
         lua_pop(*_mainState, 1); //don't actually give a fuck about the table
    }

    lua_pop(*_mainState, 1);

    LuaManager::stackBalanceEnd();
    return val;
}


//Push value from model to update Lua table/userdata
inline void LuaTableFieldItem::setValue(const QVariant val) {
    LuaManager::stackBalanceBegin();
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _luaRef);
    OOLUA::push(*_mainState, _key);
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
    LuaManager::stackBalanceEnd();
}

inline LuaTableTableFieldData::LuaTableTableFieldData(const int luaRef): _luaRef(luaRef) {}


inline void LuaTableStackModel::setLuaRef(const int ref) {
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    _luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX); //get a fresh reference
}

inline LuaTableStackModel::LuaTableStackModel(const int ref): PropertyStackModel(nullptr) { setLuaRef(ref); }

inline void LuaTableStackModel::setLabelItemText(const QString string) {
    _parentItem->setData(string, Qt::DisplayRole);
}

#endif // ESTK_LUA_TABLE_MODEL_HPP
