#include <delegate/tk_property_item_proxy_delegate.h>
#include <delegate/tk_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_lua_table_stack_model.h>
#include <delegate/tk_file_browser_property_item_delegate.h>

#include <QMouseEvent>



PropertyItemDelegate* PropertyItemProxyDelegate::_defaultTableDelegate = nullptr;


QWidget* PropertyItemProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->createEditor(parent, option, index);
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}


void PropertyItemProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setEditorData(editor, index);
    } else {
        return QStyledItemDelegate::setEditorData(editor, index);
    }

}


void PropertyItemProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setModelData(editor, model, index);
    } else {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
}



bool PropertyItemProxyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item) {

        if(item->getStyledItemDelegate()) {
            return static_cast<PropertyItemDelegate*>(item->getStyledItemDelegate())->editorEvent(event, model, option, index);
        }

        //What the fuck?
        auto* luaItem = (item->type() == (int)ESTkStandardItemType::LuaTable)? static_cast<LuaTableFieldItem*>(item) : nullptr;
        if (luaItem && (luaItem->getValueLuaType() == LUA_TTABLE || luaItem->getParentLuaType() == LUA_TTABLE || luaItem->getValueLuaType()==LUA_TUSERDATA || luaItem->getParentLuaType() == LUA_TUSERDATA)) {
            if(!_defaultTableDelegate) {
                _defaultTableDelegate = new PropertyItemDelegate();
            }
            return _defaultTableDelegate->editorEvent(event, model, option, index);
        }

    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);

}


void PropertyItemProxyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->updateEditorGeometry(editor, option, index);
    } else {
        return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

void PropertyItemProxyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* item = PropertyItemDelegate::modelIndexToESTkStandardItem(index);

    if(item && item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->paint(painter, option, index);
    }

    return QStyledItemDelegate::paint(painter, option, index);
}

bool PropertyItemProxyDelegate::eventFilter(QObject *editor, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
       // if(_deleteCallback) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Delete) {
                //auto* item = modelIndexToESTkStandardItem(index);
                //_deleteCallback(item);
                qDebug() << "DELETE KEY!!!!1";
                return true;
            }
       // }
    }
    return QStyledItemDelegate::eventFilter(editor, event);
}
