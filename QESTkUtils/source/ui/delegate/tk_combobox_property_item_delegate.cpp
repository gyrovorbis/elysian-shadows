#include <QESTkUtils/ui/delegate/tk_combobox_property_item_delegate.h>
#include <QComboBox>
#include <QCompleter>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>


QWidget* ComboBoxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const {
    auto* editor = new QComboBox(parent);
    editor->setEditable(_editable);
   // editor->setFocusPolicy(Qt::WheelFocus);

    auto* item = modelIndexToESTkStandardItem(index);

    QStringList strings = _populator(item);

    for(auto&& it : strings) {
        editor->addItem(it, it);
    }

    if(_editable) {
        QCompleter* completer = new QCompleter(strings);
        editor->setCompleter(completer);
    }

    return editor;
}


void ComboBoxPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);
    auto* item = modelIndexToESTkStandardItem(index);

    if(combo) {
        combo->setCurrentText(item->data(Qt::EditRole).toString());
        //combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    } else QStyledItemDelegate::setEditorData(editor, index);
}


void ComboBoxPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);

    if(combo) {
        auto* item = modelIndexToESTkStandardItem(index);

        item->setData(combo->itemData(combo->currentIndex()), Qt::DisplayRole);
        item->setData(combo->itemData(combo->currentIndex()), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}


