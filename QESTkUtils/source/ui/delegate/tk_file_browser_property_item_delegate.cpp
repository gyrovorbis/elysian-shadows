#include <QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <ESToolkit/core/tk_file_utilities.h>

FileBrowserPropertyItemDelegate::FileBrowserPropertyItemDelegate(void):
    FileBrowserPropertyItemDelegate(TextCallbackType())
{

}

FileBrowserPropertyItemDelegate::FileBrowserPropertyItemDelegate(TextCallbackType textCallback):
    DynamicContentPropertyItemDelegate(textCallback)
{
    addButton(QIcon(FILE_BROWSER_PROPERTY_ITEM_BROWSE_ICON_PATH), [&](ESTkStandardItem* item) {
        QString str = _dir ? QFileDialog::getExistingDirectory(nullptr, tr("Browse Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) :
                             QFileDialog::getOpenFileName(nullptr, tr("Browse File"), "./");
        if(!str.isNull()) {
             QString relFile = FileUtilities::getAssetRelativePath(str);
             item->setData(relFile, Qt::EditRole);
         }
    }, "Browse For File");
}

#if 0
QWidget* FileBrowserPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& optionz, const QModelIndex &index) const {
    QStyleOptionViewItem option(optionz);
    initStyleOption(&option, index);

    auto* model = static_cast<const PropertyStackModel*>(index.model());
    auto* item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));

    _parentWidget   = new QWidget(parent);
    _layout         = new QHBoxLayout;
    _browseButton   = new QPushButton;
    _browseButton->setIcon(QIcon(FILE_BROWSER_PROPERTY_ITEM_BROWSE_ICON_PATH));
    _pathEdit       = new QLineEdit;


    _pathEdit->setFrame(false);

    //_layout->setStretch(-1, -1);
    //_layout->setSpacing(-1);
    _layout->setMargin(0);
    _layout->setSpacing(0);

    _pathEdit->setFont(QFont(_pathEdit->font().family(), 10));
    _pathEdit->setContentsMargins(0, 0, 0, 0);
    _pathEdit->setFixedHeight(option.rect.height());
    _pathEdit->setAutoFillBackground(true);
    _pathEdit->setTextMargins(0, 0, 0, 0);
    _browseButton->setFixedSize(option.rect.height(), option.rect.height());
    _parentWidget->setAutoFillBackground(true);
    _parentWidget->setGeometry(option.rect);

    _layout->addWidget(_pathEdit);
    _layout->addWidget(_browseButton);
    _parentWidget->setLayout(_layout);

    _parentWidget->setFocusPolicy(Qt::StrongFocus);


    QObject::connect(_browseButton, &QPushButton::pressed, [=](void) {

    });


    return _parentWidget;
}

void FileBrowserPropertyItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  //  editor->setGeometry(option.rect);

}

void FileBrowserPropertyItemDelegate::setEditorData(QWidget* /*editor*/, const QModelIndex &index) const {
    auto* item = modelIndexToESTkStandardItem(index);

    _pathEdit->setText(item? item->data(Qt::EditRole).toString() : "");
}

void FileBrowserPropertyItemDelegate::setModelData(QWidget* /*editor*/, QAbstractItemModel *model, const QModelIndex &index) const {
    auto* item = modelIndexToESTkStandardItem(index);

    item->setData(_pathEdit->text(), Qt::EditRole);

}
#endif
