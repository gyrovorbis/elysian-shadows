#include <QESTkUtils/ui/delegate/tk_color_swatch_property_item_delegate.h>
#include <AssetIO/tk_color.h>
#include <QColorDialog>
#include <proxy/elysian_primitive_proxy.hpp>
#include <QPainter>

ColorSwatchPropertyItemDelegate::ColorSwatchPropertyItemDelegate(void) {
    addButton(QIcon(TK_COLOR_SWATCH_ICON_PATH), &ColorSwatchPropertyItemDelegate::onColorSwatchButtonClick);
}


void ColorSwatchPropertyItemDelegate::onColorSwatchButtonClick(ESTkStandardItem* item) {
    LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
    if(luaFieldItem) {
        TkColor4* color = luaFieldItem->getVariant().getValue<TkColor4*>();
        if(color) {
            QColor qcolor = QColorDialog::getColor(*color);
            if(qcolor.isValid()) {
                *color = qcolor;
                color->emitFieldChanged("r");
                color->emitFieldChanged("g");
                color->emitFieldChanged("b");
                color->emitFieldChanged("a");
            }
        }
    }
}



void ColorSwatchPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    DynamicContentPropertyItemDelegate::paint(painter, option, index);

    auto* luaItem = modelIndexToLuaTableFieldItem(index);
    if(luaItem) {
        auto* color = luaItem->getVariant().getValue<TkColor4*>();
        if(color) {
            QStyleOptionViewItem itemOption(option);
            initStyleOption(&itemOption, index);
            int y = option.rect.y();
            int h = option.rect.height() - 2;
            int x = option.rect.x() + 5;
            int w = 64;
            //painter->setPen(*color);
            painter->setBrush(QBrush(*color));
            painter->drawRect(x, y, w, h);
        }
    }
}
