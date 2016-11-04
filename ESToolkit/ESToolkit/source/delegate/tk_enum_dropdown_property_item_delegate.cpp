#include <delegate/tk_enum_dropdown_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <QComboBox>

QWidget* EnumDropdownPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
    auto* editor = new QComboBox(parent);
    //editor->setEditable(true);
   // editor->setFocusPolicy(Qt::WheelFocus);

    for(unsigned i = 0; i < (unsigned)_stringNames.size(); ++i) {
        editor->addItem(_stringNames[i], i);
    }
    return editor;
}


void EnumDropdownPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(combo) combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void EnumDropdownPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    if(combo) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));
        const int i = combo->currentIndex();

        item->setData(_stringNames[i], Qt::DisplayRole);
        item->setData(i, Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}

