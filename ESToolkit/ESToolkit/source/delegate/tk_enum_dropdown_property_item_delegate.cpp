#include <delegate/tk_enum_dropdown_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <QComboBox>
#include <QPainter>
#include <QApplication>

QWidget* EnumDropdownPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
    QComboBox* editor = new QComboBox(parent);
    editor->setFrame(false);
    //editor->setContentsMargins(0, 0, 0, 0);
    editor->setAutoFillBackground(true);
    editor->setGeometry(option.rect);
    //editor->setEditable(true);
   // editor->setFocusPolicy(Qt::WheelFocus);

    for(unsigned i = 0; i < (unsigned)_stringNames.size(); ++i) {
        editor->addItem(_stringNames[i], i);
    }
    return editor;
}


void EnumDropdownPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    auto* item = modelIndexToESTkStandardItem(index);

    if(combo) combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void EnumDropdownPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    if(combo) {
        auto* item = modelIndexToESTkStandardItem(index);
        const int i = combo->currentIndex();

        item->setData(_stringNames[i], Qt::DisplayRole);
        item->setData(i, Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}

void EnumDropdownPropertyItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
    editor->setGeometry(option.rect);
}

void EnumDropdownPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem itemOption(option);

   initStyleOption(&itemOption, index);
   itemOption.text = _stringNames[index.data(Qt::EditRole).toInt()];  // override text

   QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
}
