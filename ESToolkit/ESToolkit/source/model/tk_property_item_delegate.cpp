#include <QComboBox>

#include <model/tk_property_item_delegate.h>
#include <model/tk_property_standard_item.h>
#include <model/tk_property_stack_model.h>

QWidget* PropertyItemProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->createEditor(parent, option, index);
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }


}


void PropertyItemProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setEditorData(editor, index);
    } else {
        return QStyledItemDelegate::setEditorData(editor, index);
    }

}


void PropertyItemProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->setModelData(editor, model, index);
    } else {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }

}


bool PropertyItemProxyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
#if 0

    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));


    if(item->getStyledItemDelegate()) {
        return item->getStyledItemDelegate()->editorEvent(event, model, option, index);
    } else {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }


#endif
}



QWidget* EnumDropdownPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto* editor = new QComboBox(parent);
    editor->setFocusPolicy(Qt::WheelFocus);

    for(unsigned i = 0; i < _stringNames.size(); ++i) {
        editor->addItem(_stringNames[i], i);
    }

    return editor;

}


void EnumDropdownPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);
    combo->setCurrentIndex(index.data().toInt());
}


void EnumDropdownPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = static_cast<QComboBox*>(editor);
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

    item->setData(combo->currentIndex());
}

