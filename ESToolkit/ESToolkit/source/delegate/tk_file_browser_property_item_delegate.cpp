#include <delegate/tk_file_browser_property_item_delegate.h>
#include <model/tk_property_stack_model.h>
#include <model/tk_property_standard_item.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <core/tk_file_utilities.h>


FileBrowserPropertyItemDelegate::FileBrowserPropertyItemDelegate(void) {}

QWidget* FileBrowserPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex &index) const {

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

    QObject::connect(_browseButton, &QPushButton::pressed, [=](void) {
       QString str = _dir ? QFileDialog::getExistingDirectory(nullptr, tr("Locate Project Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) :
                            QFileDialog::getOpenFileName(nullptr, tr("Locate File"), "./");
       if(!str.isNull()) {
            QString relFile = FileUtilities::getAssetRelativePath(str);
            _pathEdit->setText(relFile);
            item->setData(relFile, Qt::EditRole);
        }
    });


    return _parentWidget;
}

void FileBrowserPropertyItemDelegate::setEditorData(QWidget* /*editor*/, const QModelIndex &index) const {
    auto* item = modelIndexToESTkStandardItem(index);

    _pathEdit->setText(item? item->data(Qt::EditRole).toString() : "");
}

void FileBrowserPropertyItemDelegate::setModelData(QWidget* /*editor*/, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* item = modelIndexToESTkStandardItem(index);

    item->setData(_pathEdit->text(), Qt::EditRole);

}
