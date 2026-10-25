/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/ui/delegate/tk_plain_text_edit_property_item_delegate.h>
#include <QTextEdit>

#include <QFontMetrics>
#include <QScrollBar>
#include <ESGamma/system/elysian_dialog_system.hpp>
#include <ESGamma/gui/elysian_font.hpp>

using namespace elysian;

namespace QESTkUtils {


class DisableScrollFilter: public QObject {
    virtual bool    eventFilter(QObject *object, QEvent *event) override {
        //auto* editor = qobject_cast<QPlainTextEdit*>(object);
        //if(!editor) return false;
#if 0
        if(event->type() == QEvent::Scroll) {
            event->accept();
            return true;
        }
#endif

        return QObject::eventFilter(object, event);
    }
};

static DisableScrollFilter _disableFilter;


QWidget* PlainTextEditPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
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
    rect.setHeight(rowHeight*3);
    editor->setGeometry(rect);
  //  QCursor cur = editor->setCursorRect
    //editor->setCenterOnScroll(false);
    editor->verticalScrollBar()->setEnabled(false);
    editor->installEventFilter(&_disableFilter);
    //editor->verticalScrollBar()->installEventFilter(&_disableFilter);
    //editor->setEditable(true);
   // editor->setFocusPolicy(Qt::WheelFocus);

#if 1
    QObject::connect(editor, &QTextEdit::textChanged, [=]() {
        char buff[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE] = { '\0' };   //all text
        char tempBuff[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE];          //current line

        QTextCursor cursor = editor->textCursor();
        strncpy(buff, Q_CSTR(editor->toPlainText()), ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE-1);

        auto&           font = System::getDialogSys()->getTextFont();
        unsigned        length     = strlen(buff);
        unsigned        lineIdx    = 0;
        unsigned        wordIdx    = 0;
        unsigned        i          = 0;
        unsigned        curLine    = 0;
        unsigned        curOffset  = 0;

        for(; i < length; ++i) {
            //Only look at the buffer starting at the current line and going until the end of the string
            for(unsigned j = lineIdx; j <= i; ++j) tempBuff[j-lineIdx] = buff[i];
            tempBuff[i-lineIdx+1] = '\0';

            if(font.getStringPixelWidth(tempBuff) > 560) {
                if(wordIdx != lineIdx) {        //This isn't just one big-ass word wrapping around
                    if(curLine != ELYSIAN_DIALOG_SYSTEM_MAX_LINES-1) {
                        lineIdx = wordIdx;      //Make the current word start a new line
                        //wordIdx = 0;
                        buff[wordIdx-1] = '\n'; //Replace space before word with newline character
                        //wordIdx = i+1;
                                                //Next character starts a word? Can't be right.
                    }
                    ++curLine;

                } else {    //Singe big-ass word being wrapped
                    if(curLine != ELYSIAN_DIALOG_SYSTEM_MAX_LINES-1) {
                        //one single fucking word is too long
                        const char temp = buff[i];
                        buff[i] = '\n';
                        buff[i+1] = temp;
                        for(unsigned j = i+3; j < ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE-1; ++j) buff[j-1] = buff[j];
                        if(cursor.position() >= i) ++curOffset;
                        //if(cursor.position() >= (int)strlen(tempBuff)) {
                          //  cursor.setPosition(cursor.position()+1);
                       // }
                        //lineIdx = wordIdx;
                        //wordIdx = 0;
                        ++length;
                    } else ++curLine;
                }
            }

            //Check if we've hit our line limit, and cut the string off here if we have
            if(curLine >= ELYSIAN_DIALOG_SYSTEM_MAX_LINES) {
                buff[i] = '\0';
                break;
            }

            switch(buff[i]) {
            case '\n':
                ++curLine;      //Start next line
                lineIdx = i+1;  //Next index is the beginning of the next line
            case ' ':
                wordIdx = i+1;  //next index is the beginning of the current word
                break;
            default:
                break;
            }

        }

        int newCursorPos = cursor.position() + curOffset;


        const int newLen = strlen(buff);
#if 1
        if(newCursorPos > newLen) {
            cursor.setPosition(newLen);
        }
#endif
#if 0
        if(cursor.position() == oldLen) {
            cursor.setPosition(oldLen + (newLen-oldLen));
         }
#endif

        editor->blockSignals(true);
        editor->setPlainText(buff);
        editor->blockSignals(false);
        cursor.setPosition(newCursorPos);//+curOffset);
        editor->setTextCursor(cursor);
    });

#else
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
    QTextEdit* edit = static_cast<QTextEdit*>(editor);

    auto* item = modelIndexToESTkStandardItem(index);

    edit->setPlainText(item->data(Qt::EditRole).toString());
}


void PlainTextEditPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QTextEdit* edit = static_cast<QTextEdit*>(editor);

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
