#ifndef SELECTION_DOCK_WIDGET_H
#define SELECTION_DOCK_WIDGET_H

#include <QDockWidget>

class QTreeView;
class QAbstractItemModel;
class QAbstractItemDelegate;

namespace Ui {
    class SelectionDockWidget;
}

class SelectionDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit                    SelectionDockWidget(QWidget *parent = nullptr);
                                ~SelectionDockWidget(void);

    void                        setSelectionWidget(QWidget *widget);
    void                        setSelectionTreeWidget(void);
    void                        restoreSelectionTreeWidgetDefaultItemDelegate(void);
    void                        setSelectionModel(QAbstractItemModel *model);
    void                        expandSelectionTree(const int index=0);
    QTreeView*                  getSelectionTreeWidget(void);

Q_SIGNALS:
    void                        uncheckMe(int);
protected:

    virtual void                closeEvent(QCloseEvent *event);
    void                        _sizeColumnsToDefault();
private:

    Ui::SelectionDockWidget*    _ui;
    QTreeView*                  _selectionTreeWidget;
    QAbstractItemDelegate*      _defaultItemDelegate;
};




// ==== INLINEZ =====


inline QTreeView *SelectionDockWidget::getSelectionTreeWidget(void) {
	return _selectionTreeWidget;
}

#endif // SELECTIONDOCKWIDGET_H
