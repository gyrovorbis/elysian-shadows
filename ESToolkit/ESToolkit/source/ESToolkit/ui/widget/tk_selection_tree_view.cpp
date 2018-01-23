#include <ESToolkit/ui/widget/tk_selection_tree_view.h>
#include <QKeyEvent>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QFontDatabase>

SelectionTreeView::SelectionTreeView(void) {
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(10);
    setFont(f);

    QPalette p = palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    setPalette(p);
    setIndentation(9);
    setAlternatingRowColors(true);
    setMouseTracking(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    viewport()->setAcceptDrops(true);
}

void SelectionTreeView::keyPressEvent(QKeyEvent *event) {
    QTreeView::keyPressEvent(event);

    auto* delegate = itemDelegate();
    for(auto&& it : selectionModel()->selectedIndexes()) {
        delegate->editorEvent(event, model(), QStyleOptionViewItem(), it);
    }
}

void SelectionTreeView::drawRow(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const {

    QTreeView::drawRow(p, opt, idx);
                p->save();
                QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));

               p->setPen( color );
               p->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
               p->restore();
          //     p->drawLine( 0, py, pw, py );
               //p->drawLine(rect.x()+pw, rect.y(), rect.x()+pw, rect.y()+ph);
}

#if 0
void SelectionTreeView::dragEnterEvent(QDragEnterEvent *event) {

}

void SelectionTreeView::dropEvent(QDropEvent *event) {

}
#endif
