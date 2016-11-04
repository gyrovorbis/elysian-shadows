#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QString>
#include <asset/tk_asset_manager.h>
#include <crtp/elysian_lua_manager.hpp>
#include <core/tk_lua_qt_utils.h>
#include <crtp/elysian_lua_variant.hpp>
//#include <tk_property_stack_model.h>
#include <tk_property_standard_item.h>

#define ELYSIAN_LUA_TABLE_STACK_MODEL_DELEGATE_METAFIELD    "__delegate"
#define ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR     QColor(Qt::blue)

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
    mutable QStandardItem*          _labelItem      = nullptr;
    LuaFieldRef                     _fieldRef;

public:
                                    LuaTableFieldItem(LuaFieldRef ref, QStandardItem*const labelItem);
    virtual                         ~LuaTableFieldItem(void);
    virtual void                    insertingModelItem(void) override;
    virtual QStyledItemDelegate*    getStyledItemDelegate(void) const override;
    virtual int                     type(void) const override;

    virtual QVariant                getValue(void) const;
    virtual void                    setValue(const QVariant val);


    LuaVariant                      getVariant(void) const;
    void                            setVariant(const LuaVariant var);
    int                             getLuaRef(void) const;
    const LuaVariant&               getKey(void) const;


    void                            setDirty(const bool val=true);
    LuaTableTableFieldData*         getTableData(void) const;

    int                             getValueLuaType(void) const;
    int                             getKeyLuaType(void) const;
    int                             getParentLuaType(void) const;

};


//Aggregating "Table" data away, so primitive-type field items don't have all this overhead.
struct LuaTableTableFieldData: public QObject, public LuaManager {
    Q_OBJECT
    friend class LuaTableFieldItem;
public:
    //Lua table/hash maintaining a list of child fieldItems lightuserdata for each key
    LuaVariant                          _fieldItems;
    LuaVariant                          _tableRef;
    LuaVariant                          _parentRef      = false;
    QStandardItem*                      _parentItem     = nullptr;
    bool                                _isPrefab       = false;
    int                                 _prefabDepth    = 0;

                                        LuaTableTableFieldData(LuaVariant tableRef);
    virtual                             ~LuaTableTableFieldData(void) override = default;

    void                                insertTableFieldItem(LuaVariant key);
    void                                insertTableFieldItems(QStandardItem* parentItem);
    const LuaVariant&                   getTableRef(void) const;
    bool                                isPrefab(void) const;
    const LuaVariant&                   getParentRef(void) const;
    int                                 getPrefabDepth(void) const;
    void                                pushPrefab(void);
    void                                popPrefab(void);

public Q_SLOTS:
    void                                onFieldChanged(const LuaVariant& key);
    void                                onFieldAdded(const LuaVariant& key);
  //vod                                 onFieldRemoved(const QVariant key); same shit as onFieldChanged to nil!

Q_SIGNALS:

   // void                                fieldChanged(const LuaFieldRef& ref);

};


class LuaTableStackModel: public PropertyStackModel, public LuaManager {

    friend class LuaTableItem;

    LuaVariant                  _tableRef;
    LuaTableTableFieldData*     _tableData      = nullptr;
    QStandardItem*              _parentItem     = nullptr;
    bool                        _populated      = false;

public:
                                LuaTableStackModel(LuaVariant tableRef);
    void                        setLabelItemText(const QString string);
    const LuaVariant&           getTableRef(void) const;

    virtual void                populateModel(void) override;

};









//==== INLINEZ ======

inline int LuaTableFieldItem::type(void) const { return (int)ESTkStandardItemType::LuaTable; }

inline void LuaTableFieldItem::setVariant(const LuaVariant var) {
    _fieldRef.setVariant(var);
}

inline LuaVariant LuaTableFieldItem::getVariant(void) const {
    return _fieldRef.getVariant();
}

inline int LuaTableFieldItem::getLuaRef(void) const { return _fieldRef.getTableRef().getRef(); }

//Pull value from Lua table/userdata to update model
inline QVariant LuaTableFieldItem::getValue(void) const {
    LuaManager::stackBalanceBegin();

    QVariant    val;
    int         valType;

    //retrieve field
    _fieldRef.getTableRef().push();
    _fieldRef.getKey().push();
    lua_gettable(*_mainState, -2);

    //check type
    valType = lua_type(*_mainState, -1);

    //Let QVariant handle all primitive types
    if(valType != LUA_TTABLE && valType != LUA_TUSERDATA) {
        if(_tableData) {
            //setDirty(true);
            //emitDataChanged();
        }
        OOLUA::pull(*_mainState, val);
    //Table/Userdata fields are out-of-date

    } else if(!_tableData) {
        _tableData = new LuaTableTableFieldData(LuaVariant::fromStack());
        //if(!_tableData->getParentRef().isNil() && (*_tableData->getParentRef()[_fieldRef.getKey()] == ))
        if(_tableData->isPrefab()) {
            _labelItem->setData(QVariant(ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR), Qt::ForegroundRole);
        }

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
    _fieldRef.getTableRef().push();
    _fieldRef.getKey().push();
    OOLUA::push(*_mainState, val);
    lua_settable(*_mainState, -3);
    lua_pop(*_mainState, 1);
}

inline LuaTableTableFieldData::LuaTableTableFieldData(LuaVariant tableRef):
    _fieldItems(LuaVariant::newTable()),
    _tableRef(std::move(tableRef)),
    _parentRef(LuaManager::getParent(_tableRef)),
    _isPrefab((LuaManager::getPrefabName(_tableRef) != nullptr))
{
}

inline LuaTableStackModel::LuaTableStackModel(LuaVariant tableRef):
    PropertyStackModel(nullptr),
    _tableRef(std::move(tableRef))
{
    GY_ASSERT(_tableRef.isIndexable());
}

inline void LuaTableStackModel::setLabelItemText(const QString string) {
    _parentItem->setData(string, Qt::DisplayRole);
}

inline const LuaVariant& LuaTableStackModel::getTableRef(void) const {
    return _tableRef;
}

inline const LuaVariant& LuaTableFieldItem::getKey(void) const {
    return _fieldRef.getKey();
}

inline int LuaTableFieldItem::getKeyLuaType(void) const {
    return _fieldRef.getKey().getType();
}

inline int LuaTableFieldItem::getValueLuaType(void) const {
    _fieldRef.getTableRef().push();
    _fieldRef.getKey().push();
    lua_gettable(*_mainState, -2);
    const int type = lua_type(*_mainState, -1);
    lua_pop(*_mainState, 1);
    return type;
}


inline LuaTableTableFieldData* LuaTableFieldItem::getTableData(void) const { return _tableData; }

inline int LuaTableFieldItem::getParentLuaType(void) const {
    return _fieldRef.getTableRef().getType();
}

inline const LuaVariant& LuaTableTableFieldData::getTableRef(void) const { return _tableRef; }
inline bool LuaTableTableFieldData::isPrefab(void) const { return _isPrefab; }
inline const LuaVariant& LuaTableTableFieldData::getParentRef(void) const { return _parentRef; }

#endif // ESTK_LUA_TABLE_MODEL_HPP
