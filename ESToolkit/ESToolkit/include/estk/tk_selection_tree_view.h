#ifndef TK_SELECTION_TREE_VIEW_H
#define TK_SELECTION_TREE_VIEW_H

#include <QTreeView>

class SelectionTreeView: public QTreeView {
public:

                    SelectionTreeView(void);

    virtual void    keyPressEvent(QKeyEvent* event) override;

};

#endif // TK_SELECTION_TREE_VIEW_H
