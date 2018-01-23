#ifndef TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H
#define TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>

#define TK_COLOR_SWATCH_ICON_PATH ":/misc/images/color_swatch_icon.png"


class ColorSwatchPropertyItemDelegate: public DynamicContentPropertyItemDelegate {

public:

                    ColorSwatchPropertyItemDelegate(void);
virtual void        paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:

        static void onColorSwatchButtonClick(ESTkStandardItem* item);
};

#endif // TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H
