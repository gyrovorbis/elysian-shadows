#include <delegate/tk_dynamic_text_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_lua_table_stack_model.h>
#include <QApplication>

void DynamicTextPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem itemOption(option);

   initStyleOption(&itemOption, index);

   auto* model = static_cast<const PropertyStackModel*>(index.model());
   auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
   LuaTableFieldItem* fieldItem =
           (item->type() == (int)ESTkStandardItemType::LuaTable)?
               static_cast<LuaTableFieldItem*>(item) : nullptr;

   _callback(fieldItem, itemOption);

   QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
}
