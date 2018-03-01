#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QToolTip>
#include <QPainter>

void DynamicContentPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyle* style = QApplication::style();
    QStyleOptionViewItem itemOption(option);
    initStyleOption(&itemOption, index);

    auto* item = modelIndexToESTkStandardItem(index);
    unsigned useButtonCount;
    auto useButtons = _getUseButtonData(item, useButtonCount);

    //Render text
    if(_textCallback) {
        QStyleOptionViewItem itemOption(option);
       initStyleOption(&itemOption, index);

       int buttonsW = useButtonCount? DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN+DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING : 0;

        for(int i = 0; i < _buttons.size(); ++i) {
            if(!useButtons[i]) continue;
            auto& it = _buttons[i];
            if(it.persistent || (option.state & QStyle::State_MouseOver))
                buttonsW += option.rect.height() + DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING;
        }

        _textCallback(item, itemOption);
        itemOption.viewItemPosition = QStyleOptionViewItem::Beginning;
        itemOption.rect.setWidth(itemOption.rect.width()-buttonsW);
        style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
        //        PropertyItemDelegate::paint(painter, option, index);
    } else {
        PropertyItemDelegate::paint(painter, option, index);
    }

    if(useButtonCount) {
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

        for(int i = 0; i < _buttons.size(); ++i) {
            if(!useButtons[i]) continue;
            auto& it = _buttons[i];
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

    if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove) {
        bool showedToolTip = false;
        auto* item = modelIndexToESTkStandardItem(index);
        unsigned useButtonCount;
        auto useButtons = _getUseButtonData(item, useButtonCount);
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(useButtonCount) {
            QPoint mousePos = mouseEvent->pos();

            int x = option.rect.x()+option.rect.width() - DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_MARGIN;
            int y = option.rect.y();
            int h = option.rect.height();
            int w = h;

            for(int i = 0; i < _buttons.size(); ++i) {
                if(!useButtons[i]) continue;
                auto& it = _buttons[i];
                x -= (w + DYNAMIC_CONTENT_PROPERTY_ITEM_DELEGATE_BUTTON_SPACING);

                if(mousePos.x() >= x && mousePos.x() <= x+w && mousePos.y() >= y && mousePos.y() <= y+h) {
                    if(!it.toolTip.isEmpty()) {
                        QToolTip::showText(mouseEvent->globalPos(), it.toolTip, nullptr, QRect(), _toolTipTime);
                        showedToolTip = true;
                    }
                    if(mouseEvent->button() == Qt::LeftButton) {
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
        if(!showedToolTip && !_toolTip.isEmpty()) {
            QToolTip::showText(mouseEvent->globalPos(), _toolTip, nullptr, QRect(), _toolTipTime);
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


void DynamicContentPropertyItemDelegate::_luaArraySizeTextCallback(ESTkStandardItem *item, QStyleOptionViewItem& option, bool emptyInvalid) {
    LuaTableFieldItem* luaItem = static_cast<LuaTableFieldItem*>(item);
    LuaVariant v = luaItem->getVariant();
    const int length = v.getLength();
    option.text = QString::asprintf("[%d items]", length);
    QPalette palette = option.palette;
    if(!length && emptyInvalid) {
        palette.setColor(QPalette::Text, QColor(Qt::red));
        luaItem->setToolTip("Must contain at least one item!");
    } else {
        palette.setColor(QPalette::Text, QColor(Qt::gray));
        luaItem->setToolTip("");
    }
    option.palette = palette;
}

QVector<bool> DynamicContentPropertyItemDelegate::_getUseButtonData(ESTkStandardItem* item, unsigned &useButtonCount) const {
    QVector<bool> useButtons = QVector<bool>(_buttons.size());
    useButtonCount = 0;
    for(int i = 0; i < _buttons.size(); ++i) {
        if(!_buttons[i].useButtonCallback || (_buttons[i].useButtonCallback && _buttons[i].useButtonCallback(item))) {
            useButtons[i] = true;
            ++useButtonCount;
        }
        else useButtons[i] = false;
    }

    return useButtons;
}


