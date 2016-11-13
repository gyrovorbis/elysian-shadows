#include <delegate/tk_dynamic_content_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_lua_table_stack_model.h>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

void DynamicContentPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyle* style = QApplication::style();
    QStyleOptionViewItem itemOption(option);
    initStyleOption(&itemOption, index);

    //Render text
    if(_textCallback) {
        QStyleOptionViewItem itemOption(option);
       initStyleOption(&itemOption, index);

       auto* item = modelIndexToESTkStandardItem(index);
       int buttonsW = _buttons.size()? DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN+DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING : 0;

       for(auto&& it: _buttons)
           if(it.persistent || (option.state & QStyle::State_MouseOver))
                buttonsW += option.rect.height() + DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING;

        _textCallback(item, itemOption);
        itemOption.viewItemPosition = QStyleOptionViewItem::Beginning;
        itemOption.rect.setWidth(itemOption.rect.width()-buttonsW);
        style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
    } else {
        PropertyItemDelegate::paint(painter, option, index);
    }

    if(_buttons.size()) {
        int y               = option.rect.y();
        int h               = option.rect.height();
        int w               = h;
        int x               = option.rect.x()+option.rect.width() - DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN;
        //buttonsW            += DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN;
        QStyleOptionButton button;
        button.iconSize     = QSize(h, h);
        button.state        = itemOption.state;
        button.fontMetrics  = itemOption.fontMetrics;
        button.palette      = itemOption.palette;
        button.styleObject  = itemOption.styleObject;

        for(auto&& it : _buttons) {
            if(it.persistent || (option.state & QStyle::State_MouseOver)) {
                x           -= (w + DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING);
                button.rect = QRect(x, y, w, h);
                button.icon = it.icon;
                style->drawControl(QStyle::CE_PushButton, &button, painter, nullptr);
            }

        }
    }
}

bool DynamicContentPropertyItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

    if(event->type() == QEvent::MouseButtonPress) {
        if(_buttons.size()) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

            if(mouseEvent->button() == Qt::LeftButton) {
                int x = option.rect.x()+option.rect.width() - DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN;
                int y = option.rect.y();
                int h = option.rect.height();
                int w = h;

                for(auto&& it : _buttons) {
                    x -= (w + DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING);
                    QPoint mousePos = mouseEvent->pos();

                    if(mousePos.x() >= x && mousePos.x() <= x+w && mousePos.y() >= y && mousePos.y() <= y+h) {
                        auto* item = modelIndexToESTkStandardItem(index);
                        Q_ASSERT(it.callback);
                        if(it.callback) {
                            it.callback(item);
                            return true;
                        }
                    }
                }
            }
        }
    } else if(event->type() == QEvent::KeyPress) {
        if(_deleteCallback) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Delete) {
                auto* item = modelIndexToESTkStandardItem(index);
                _deleteCallback(item);
                return true;
            }
        }
    }

    return PropertyItemDelegate::editorEvent(event, model, option, index);
}

void DynamicContentPropertyItemDelegate::luaArraySizeTextCallback(ESTkStandardItem *item, QStyleOptionViewItem& option) {
    LuaTableFieldItem* luaItem = static_cast<LuaTableFieldItem*>(item);
    LuaVariant v = luaItem->getVariant();
    option.text = QString::asprintf("[%d items]", v.getLength());
    QPalette palette = option.palette;
    palette.setColor(QPalette::Text, QColor(Qt::gray));
    option.palette = palette;
}
