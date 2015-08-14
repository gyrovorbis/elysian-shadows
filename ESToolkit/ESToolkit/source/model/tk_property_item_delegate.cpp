#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <core/tk_file_utilities.h>

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
    return QStyledItemDelegate::editorEvent(event, model, option, index);
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
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    if(combo) combo->setCurrentIndex(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void EnumDropdownPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    if(combo) {
        auto* propModel = static_cast<const PropertyStackModel*>(model);
        auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

        item->setData(_stringNames[combo->currentIndex()], Qt::DisplayRole);
        item->setData(combo->currentIndex(), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}




QWidget* SpinboxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
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


FileBrowserPropertyItemDelegate::FileBrowserPropertyItemDelegate(void) {

}

QWidget* FileBrowserPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    _parentWidget   = new QWidget(parent);
    _layout         = new QHBoxLayout;
    _browseButton   = new QPushButton("..");
    _pathEdit       = new QLineEdit;

    //_layout->setStretch(-1, -1);
    //_layout->setSpacing(-1);
    _layout->setMargin(0);
    _layout->addWidget(_pathEdit);
    _layout->addWidget(_browseButton);
    _pathEdit->setFont(QFont(_pathEdit->font().family(), 10));
    _pathEdit->setContentsMargins(0, 0, 0, 0);
    _pathEdit->setFixedHeight(15);
    _browseButton->setFixedSize(15, 15);

    _parentWidget->setLayout(_layout);
    _parentWidget->setFocusPolicy(Qt::StrongFocus);
    _parentWidget->setAutoFillBackground(true);

    QObject::connect(_browseButton, &QPushButton::pressed, [=]() {
       QString str = QFileDialog::getOpenFileName(nullptr, tr("Locate File"), "./");
       if(!str.isNull()) {
            QString relFile = FileUtilities::getAssetRelativePath(str);
            _pathEdit->setText(relFile);
            item->setData(relFile, Qt::EditRole);
        }
    });


    return _parentWidget;
}

void FileBrowserPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    _pathEdit->setText(item? item->data(Qt::EditRole).toString() : "");
}

void FileBrowserPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* propModel = static_cast<const PropertyStackModel*>(model);
    auto* item = static_cast<ESTkStandardItem*>(propModel->itemFromIndex(index));

    item->setData(_pathEdit->text(), Qt::EditRole);

}


