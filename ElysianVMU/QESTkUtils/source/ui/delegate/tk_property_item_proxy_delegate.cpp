/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/ui/delegate/tk_property_item_proxy_delegate.h>
#include <QESTkUtils/ui/delegate/tk_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.h>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>


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
    QStyledItemDelegate::paint(painter, option, index);
#if 0

      QModelIndex s = index.sibling( index.row(), 0 );
          if ( s.isValid() )
          {
               QRect rectr = option.rect;
               int py = rectr.y();
               int ph = rectr.height();
               int pw = rectr.width();
               painter->setPen( QColor(Qt::gray) );
               painter->drawLine( rectr.x(), py, pw, py );
               //p->drawLine(rect.x()+pw, rect.y(), rect.x()+pw, rect.y()+ph);
      }
#else
    auto opt = option;
    opt.palette.setCurrentColorGroup(QPalette::Active);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    if (index.column() < index.model()->columnCount()-1) {
        int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->restore();
#endif
}

