#ifndef SELECTIONDOCKWIDGET_H
#define SELECTIONDOCKWIDGET_H

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
    explicit SelectionDockWidget(QWidget *parent = 0);
    ~SelectionDockWidget();
    void SetSelectionWidget(QWidget *widget);
    void SetSelectionTreeWidget();
	void restoreSelectionTreeWidgetDefaultItemDelegate(void);
    void SetSelectionModel(QAbstractItemModel *model);
    void ExpandSelectionTree();
	QTreeView *getSelectionTreeWidget(void);
signals:
    void UncheckMe(int);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::SelectionDockWidget *ui;
    QTreeView *_selectionTreeWidget;
    void SizeColumnsToDefault();
	QAbstractItemDelegate *_defaultItemDelegate;
};

inline QTreeView *SelectionDockWidget::getSelectionTreeWidget(void) {
	return _selectionTreeWidget;
}

#endif // SELECTIONDOCKWIDGET_H
