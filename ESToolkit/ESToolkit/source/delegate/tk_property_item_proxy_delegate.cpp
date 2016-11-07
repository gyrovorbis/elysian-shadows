#include <delegate/tk_property_item_proxy_delegate.h>
#include <delegate/tk_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_lua_table_stack_model.h>
#include <delegate/tk_file_browser_property_item_delegate.h>

#include <QMouseEvent>



PropertyItemDelegate* PropertyItemProxyDelegate::_defaultTableDelegate = nullptr;


QWidget* PropertyItemProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->createEditor(parent, option, index);
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}


void PropertyItemProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setEditorData(editor, index);
    } else {
        return QStyledItemDelegate::setEditorData(editor, index);
    }

}


void PropertyItemProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setModelData(editor, model, index);
    } else {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
}



bool PropertyItemProxyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

    auto* propModel =  qobject_cast<const PropertyStackModel*>(model);
    auto* item = propModel? propModel->itemFromIndex(index) : nullptr;
    auto* propItem = (item && item->type() >= QStandardItem::UserType)? static_cast<ESTkStandardItem*>(item) : nullptr;

    if(propItem) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(propItem->getStyledItemDelegate() && mouseEvent->button() != Qt::RightButton) {
            return static_cast<PropertyItemDelegate*>(propItem->getStyledItemDelegate())->editorEvent(event, model, option, index);
        }

        //What the fuck?
        auto* luaItem = (propItem->type() == (int)ESTkStandardItemType::LuaTable)? static_cast<LuaTableFieldItem*>(propItem) : nullptr;
        if (luaItem && (luaItem->getValueLuaType() == LUA_TTABLE || luaItem->getParentLuaType() == LUA_TTABLE || luaItem->getValueLuaType()==LUA_TUSERDATA || luaItem->getParentLuaType() == LUA_TUSERDATA)) {
            if(!_defaultTableDelegate) {
                _defaultTableDelegate = new FileBrowserPropertyItemDelegate;
            }
            return _defaultTableDelegate->editorEvent(event, model, option, index);
        }

    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);

}


void PropertyItemProxyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->updateEditorGeometry(editor, option, index);
    } else {
        return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

void PropertyItemProxyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(!index.isValid()) return;

    auto* propModel = static_cast<const PropertyStackModel*>(index.model());
    if(propModel) {
        QStandardItem* stdItem = propModel->itemFromIndex(index);
        if(stdItem && stdItem->type() >= (int)ESTkStandardItemType::Generic) {
            ESTkStandardItem* item = static_cast<ESTkStandardItem*>(stdItem);
            if(item->getStyledItemDelegate()) {
                return item->getStyledItemDelegate()->paint(painter, option, index);
            }
        }
    }

    return QStyledItemDelegate::paint(painter, option, index);
}
