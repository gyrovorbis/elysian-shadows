#ifndef HISTORYDOCKWIDGET_H
#define HISTORYDOCKWIDGET_H

#include <QDockWidget>

class QUndoStack;
class QUndoGroup;
class QUndoView;

namespace Ui {
    class HistoryDockWidget;
}

class HistoryDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit HistoryDockWidget(QWidget *parent = 0);
    ~HistoryDockWidget();
    void SetActiveStack(QUndoStack *stack);
    void SetActiveGroup(QUndoGroup *group);
Q_SIGNALS:
    void UncheckMe(int);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::HistoryDockWidget *ui;
    QUndoView *m_undoView;
};

#endif // HISTORYDOCKWIDGET_H
