#include <delegate/tk_spinbox_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <QSpinbox>


QWidget* SpinboxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
    auto* editor = new QSpinBox(parent);
    editor->setMaximum(_max);
    editor->setMinimum(_min);
    return editor;
}


void SpinboxPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(spinbox) spinbox->setValue(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void SpinboxPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);

    if(spinbox) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

        item->setData(spinbox->value(), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}
