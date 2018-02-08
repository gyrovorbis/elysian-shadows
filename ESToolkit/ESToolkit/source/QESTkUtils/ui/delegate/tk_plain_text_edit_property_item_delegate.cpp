#include <QESTkUtils/ui/delegate/tk_plain_text_edit_property_item_delegate.h>
#include <QPlainTextEdit>
#include <QFontMetrics>
#include <QScrollBar>
#include <system/elysian_dialog_system.hpp>
#include <gui/elysian_font.hpp>

using namespace elysian;

namespace QESTkUtils {


class DisableScrollFilter: public QObject {
    virtual bool    eventFilter(QObject *object, QEvent *event) override {
        //auto* editor = qobject_cast<QPlainTextEdit*>(object);
        //if(!editor) return false;

        if(event->type() == QEvent::Scroll) {
            event->accept();
            return true;
        }
        return QObject::eventFilter(object, event);
    }
};

static DisableScrollFilter _disableFilter;


QWidget* PlainTextEditPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
    QPlainTextEdit* editor = new QPlainTextEdit(parent);
    editor->setFrameShape(QFrame::NoFrame);
    //editor->setContentsMargins(0, 0, 0, 0);
    editor->setAutoFillBackground(true);
    editor->document()->setDocumentMargin(0);
    editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QFontMetrics m (editor->font());
    int rowHeight = m.lineSpacing();
    editor->setMaximumHeight(rowHeight*3);
    auto rect = option.rect;
    rect.setHeight(rowHeight*3);
    editor->setGeometry(rect);
  //  QCursor cur = editor->setCursorRect
    editor->setCenterOnScroll(false);
    editor->verticalScrollBar()->setEnabled(false);
    editor->installEventFilter(&_disableFilter);
    editor->verticalScrollBar()->installEventFilter(&_disableFilter);
    //editor->setEditable(true);
   // editor->setFocusPolicy(Qt::WheelFocus);
#if 0
    QObject::connect(editor, &QPlainTextEdit::textChanged, [=]() {
        char buff[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE] = { '\0' };

        QTextCursor cursor = editor->textCursor();

        auto&       font = System::getDialogSys()->getTextFont();
        const Vector2&    size = System::getDialogSys()->getTextFrameSize();

        int wordsOnLine = 0;
        int lines = 0;
        QString str = editor->toPlainText();
        str.truncate(ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE-1);
        QStringList words = str.split(" ",);
        QString     accumStr;
        for(int i = 0; i < words.size(); ++i) {
            QString word = words[i];
            accumStr += words[i];
            if(font.getStringPixelWidth(Q_CSTR(accumStr)) <= 600) {
                if(wordsOnLine) word += " ";
                strcat(buff, Q_CSTR(word));
                ++wordsOnLine;
            } else {
                if(++lines == ELYSIAN_DIALOG_SYSTEM_MAX_LINES) break;
                wordsOnLine = 0;
                strcat(buff, "\n");
                strcat(buff, Q_CSTR(words[i]));
                accumStr = QString();
            }
        }

        editor->blockSignals(true);
        editor->setPlainText(buff);
        editor->setTextCursor(cursor);
        editor->blockSignals(false);


    });
#endif
    return editor;
}


void PlainTextEditPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QPlainTextEdit* edit = static_cast<QPlainTextEdit*>(editor);

    auto* item = modelIndexToESTkStandardItem(index);

    edit->setPlainText(item->data(Qt::EditRole).toString());
}


void PlainTextEditPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QPlainTextEdit* edit = static_cast<QPlainTextEdit*>(editor);

    auto* item = modelIndexToESTkStandardItem(index);
    item->setData(edit->toPlainText(), Qt::EditRole);
}

void PlainTextEditPropertyItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect.x(), option.rect.y(), editor->width(), editor->height());
    //DynamicContentPropertyItemDelegate::updateEditorGeometry(editor, option, index);
}

void PlainTextEditPropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
#if 0
    QStyleOptionViewItem itemOption(option);

   initStyleOption(&itemOption, index);
   int strIndex = index.data(Qt::EditRole).toInt();
   if(strIndex < _stringNames.size()) {
        itemOption.text = _stringNames[index.data(Qt::EditRole).toInt()];  // override text
   }

   QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
#else
    DynamicContentPropertyItemDelegate::paint(painter, option, index);
#endif
}
#if 0
QSize PlainTextEditPropertyItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    e

}
#endif



}
