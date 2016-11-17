#include <estk/tk_selection_tree_view.h>
#include <QKeyEvent>
#include <QStyledItemDelegate>
#include <QDebug>

SelectionTreeView::SelectionTreeView(void) {
    QFont f = font();
    f.setPointSize(11);
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

#if 0
void SelectionTreeView::dragEnterEvent(QDragEnterEvent *event) {

}

void SelectionTreeView::dropEvent(QDropEvent *event) {

}
#endif
