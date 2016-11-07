#include <delegate/tk_combobox_property_item_delegate.h>
#include <QComboBox>
#include <model/tk_lua_table_stack_model.h>


QWidget* ComboBoxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const {
    auto* editor = new QComboBox(parent);
    editor->setEditable(_editable);
   // editor->setFocusPolicy(Qt::WheelFocus);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
    LuaTableFieldItem* fieldItem =
            (item->type() == (int)ESTkStandardItemType::LuaTable)?
                static_cast<LuaTableFieldItem*>(item) : nullptr;

    QStringList strings = _populator(fieldItem);

    for(auto&& it : strings) {
        editor->addItem(it, it);
    }

    return editor;
}


void ComboBoxPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(combo) {
        combo->setCurrentText(item->data(Qt::EditRole).toString());
        //combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    } else QStyledItemDelegate::setEditorData(editor, index);
}


void ComboBoxPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    if(combo) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

        item->setData(combo->itemData(combo->currentIndex()), Qt::DisplayRole);
        item->setData(combo->itemData(combo->currentIndex()), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}


