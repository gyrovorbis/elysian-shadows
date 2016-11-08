#include <delegate/tk_dynamic_text_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_lua_table_stack_model.h>
#include <QApplication>
#include <QPainter>

void DynamicTextPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem itemOption(option);

   initStyleOption(&itemOption, index);

   auto* model = static_cast<const PropertyStackModel*>(index.model());
   auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
   LuaTableFieldItem* fieldItem =
           (item->type() == (int)ESTkStandardItemType::LuaTable)?
               static_cast<LuaTableFieldItem*>(item) : nullptr;

   _callback(fieldItem, itemOption);

   QStyle* style = QApplication::style();

   //itemOption.icon = style->standardIcon(QStyle::SP_MediaPlay);
   itemOption.viewItemPosition = QStyleOptionViewItem::Beginning;
   //painter->drawImage(itemOption.rect, itemOption.icon.pixmap(itemOption.icon.availableSizes().first()).toImage());

   style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);

   QStyleOptionButton button;
   int x = option.rect.x()+option.rect.width()-20;
   int y = option.rect.y();
   int w = option.rect.height();
   int h = option.rect.height();
   button.rect = QRect(x, y, w, h);
   button.iconSize = QSize(h, h);
   button.state = itemOption.state;
   button.fontMetrics = itemOption.fontMetrics;
   button.palette = itemOption.palette;
   button.styleObject = itemOption.styleObject;
   button.icon = style->standardIcon(QStyle::SP_MediaPlay);

   style->drawControl(QStyle::CE_PushButton, &button, painter, nullptr);

}
