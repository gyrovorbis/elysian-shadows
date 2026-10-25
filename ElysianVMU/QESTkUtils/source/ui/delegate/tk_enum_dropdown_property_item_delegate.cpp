#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QComboBox>
#include <QPainter>
#include <QLineEdit>
#include <QApplication>
#include <QFontDatabase>
#include <QListView>

#if 0
QTextEdit* editor = new QTextEdit(parent);
editor->setFrameShape(QFrame::NoFrame);
//editor->setContentsMargins(0, 0, 0, 0);
editor->setAutoFillBackground(true);
editor->document()->setDocumentMargin(0);
editor->setWordWrapMode(QTextOption::NoWrap);
editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
QFontMetrics m (editor->font());
int rowHeight = m.lineSpacing();
editor->setMaximumHeight(rowHeight*3);
auto rect = option.rect;
#endif
QWidget* EnumDropdownPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
    QComboBox* editor = new QComboBox(parent);
    editor->setFrame(false);
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(10);
    editor->setFont(f);
    QListView* listView = static_cast<QListView*>(editor->view());
    listView->setContentsMargins(0, 0, 0, 0);
    listView->setSpacing(0);
    listView->setFrameShape(QFrame::NoFrame);
    listView->setLineWidth(0);
    //listView->setViewportMargins(0, 0, 0, 0);
    editor->setContentsMargins(0, 0, 0, 0);
    //editor->setContentsMargins(0, 0, 0, 0);
    editor->setAutoFillBackground(true);
    editor->setGeometry(option.rect);
    editor->setFixedSize(option.rect.width(), option.rect.height());
    //editor->lineEdit()->setTextMargins(0, 0, 0, 0);
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

        if(i < _stringNames.size()) {
            item->setData(_stringNames[i], Qt::DisplayRole);
            item->setData(i, Qt::EditRole);
        }
    } else QStyledItemDelegate::setModelData(editor, model, index);
}

void EnumDropdownPropertyItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
    editor->setGeometry(option.rect);
}

void EnumDropdownPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem itemOption(option);

   initStyleOption(&itemOption, index);
   int strIndex = index.data(Qt::EditRole).toInt();
   if(strIndex < _stringNames.size()) {
        itemOption.text = _stringNames[index.data(Qt::EditRole).toInt()];  // override text
   }

   QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
}
