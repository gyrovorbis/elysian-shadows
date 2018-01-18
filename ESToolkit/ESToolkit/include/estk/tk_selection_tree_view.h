#ifndef TK_SELECTION_TREE_VIEW_H
#define TK_SELECTION_TREE_VIEW_H

#include <QTreeView>

class SelectionTreeView: public QTreeView {
public:

                    SelectionTreeView(void);

    virtual void    keyPressEvent(QKeyEvent* event) override;

    virtual void    drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const override;

#if 0
    virtual void    dragEnterEvent(QDragEnterEvent *event) override;
    virtual void    dropEvent(QDropEvent *event) override;
#endif
};

#endif // TK_SELECTION_TREE_VIEW_H
