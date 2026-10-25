/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP
#define ESTK_LUA_TABLE_STANDARD_ITEM_MODEL_HPP

#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QString>
#include <ESToolkit/core/tk_asset_manager.h>
#include <ESGamma/crtp/elysian_lua_manager.hpp>
#include <QESTkUtils/tk_lua_qt_utils.h>
#include <AssetIO/crtp/elysian_lua_variant.hpp>


#define ELYSIAN_LUA_TABLE_STACK_MODEL_PREFAB_FONT_COLOR     QColor(Qt::blue)

class LuaTableStackModel;
struct LuaTableTableFieldData;


//QStandardItem representing a Lua k,v pair
class LuaTableFieldItem:
    public PropertyStandardItem<QVariant, LuaTableFieldItem, false>, //temporarily removing undo/redo until ownership reest is fixed.
    public elysian::LuaManager,
    public AssetManager
{
    friend struct LuaTableTableFieldData;
protected:
    mutable LuaTableTableFieldData* _tableData      = nullptr;
    mutable QStandardItem*          _labelItem      = nullptr;
    LuaFieldRef                     _fieldRef;
    mutable bool                    _ownsDelegate   = false;
    mutable bool                    _triedDelegate  = false;

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
    int                             getTableRefInt(void) const;
    LuaVariant                      getTableRef(void) const;
    const LuaVariant&               getKey(void) const;


    void                            setDirty(const bool val=true);
    LuaTableTableFieldData*         getTableData(void) const;

    int                             getValueLuaType(void) const;
    int                             getKeyLuaType(void) const;
    int                             getParentLuaType(void) const;

    QStandardItem*                  getParentItem(void);
    LuaTableFieldItem*              getParentLuaItem(void);
    QStandardItem*                  getAncestorItem(QStringList keyNames);
    LuaTableFieldItem*              getAncestorLuaItem(QStringList keyNames);
    QStandardItem*                  getAncestorSiblingItem(QStringList keyNames, int col);
    LuaTableFieldItem*              getAncestorSiblingLuaItem(QStringList keyNames, int col);

    static LuaTableFieldItem*       fromStandardItem(QStandardItem* item);
    static const LuaTableFieldItem* fromStandardItem(const QStandardItem* item);
    static LuaTableFieldItem*       fromModelIndex(const QModelIndex& index);
};


//Aggregating "Table" data away, so primitive-type field items don't have all this overhead.
struct LuaTableTableFieldData: public QObject, public LuaManager {
    Q_OBJECT
    friend class LuaTableFieldItem;
public:
    LuaVariant                          _fieldItems; //Lua table/hash maintaining a list of child fieldItems lightuserdata for each key
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
    QStandardItem*                      getParentItem(void) const;

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
    virtual bool                canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
    virtual bool                dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    virtual Qt::DropActions     supportedDropActions(void) const override;
    virtual Qt::DropActions     supportedDragActions(void) const override;
    virtual QStringList         mimeTypes(void) const override;


};











//==== INLINEZ - LuaTableFieldItem ======

inline int LuaTableFieldItem::type(void) const { return (int)ESTkStandardItemType::LuaTable; }

inline void LuaTableFieldItem::setVariant(const LuaVariant var) {
    _fieldRef.setVariant(var);
}

inline LuaVariant LuaTableFieldItem::getVariant(void) const {
    return _fieldRef.getVariant();
}

inline LuaVariant LuaTableFieldItem::getTableRef(void) const { return _fieldRef.getTableRef(); }

inline int LuaTableFieldItem::getTableRefInt(void) const { return _fieldRef.getTableRef().getRef(); }


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


inline LuaTableFieldItem* LuaTableFieldItem::getAncestorLuaItem(QStringList keyNames) {
    return fromStandardItem(getAncestorItem(keyNames));
}

inline LuaTableFieldItem* LuaTableFieldItem::getAncestorSiblingLuaItem(QStringList keyNames, int col) {
    return fromStandardItem(getAncestorSiblingItem(keyNames, col));
}

inline LuaTableFieldItem* LuaTableFieldItem::fromStandardItem(QStandardItem* item) {
    if(item && item->type() == ESTkStandardItemType::LuaTable) {
        return static_cast<LuaTableFieldItem*>(item);
    }
    return nullptr;
}

inline const LuaTableFieldItem* LuaTableFieldItem::fromStandardItem(const QStandardItem* item) {
    if(item && item->type() == ESTkStandardItemType::LuaTable) {
        return static_cast<const LuaTableFieldItem*>(item);
    }
    return nullptr;
}

inline LuaTableFieldItem* LuaTableFieldItem::fromModelIndex(const QModelIndex& index) {
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(index.model());
    if(model) {
        return fromStandardItem(model->itemFromIndex(index));
    }
    return nullptr;
}

inline QStandardItem* LuaTableFieldItem::getParentItem(void) {
    return getAncestorSiblingItem({"*"}, 1);
}
inline LuaTableFieldItem* LuaTableFieldItem::getParentLuaItem(void) {
    return getAncestorSiblingLuaItem({"*"}, 1);
}


//==== LuaTableFieldData ======

inline LuaTableTableFieldData::LuaTableTableFieldData(LuaVariant tableRef):
    _fieldItems(LuaVariant::newTable()),
    _tableRef(std::move(tableRef)),
    _parentRef(LuaManager::getParent(_tableRef)),
    _isPrefab((LuaManager::getPrefabName(_tableRef) != nullptr))
{}

inline QStandardItem* LuaTableTableFieldData::getParentItem(void) const {
    return _parentItem;
}

inline const LuaVariant& LuaTableTableFieldData::getTableRef(void) const { return _tableRef; }
inline bool LuaTableTableFieldData::isPrefab(void) const { return _isPrefab; }
inline const LuaVariant& LuaTableTableFieldData::getParentRef(void) const { return _parentRef; }


//==== LuaStackModel ======

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


#endif // ESTK_LUA_TABLE_MODEL_HPP
