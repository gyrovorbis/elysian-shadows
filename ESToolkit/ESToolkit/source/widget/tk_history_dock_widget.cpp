#include <QUndoView>
#include <QUndoGroup>
#include <QUndoStack>
#include <QDockWidget>
#include <QCloseEvent>
#include "tk_history_dock_widget.h"
#include "ui_tk_history_dock_widget.h"

HistoryDockWidget::HistoryDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::HistoryDockWidget)
{
    ui->setupUi(this);

    m_undoView = new QUndoView(parent);
    m_undoView->setEmptyLabel("<Undo All>");

    QPalette p = m_undoView->palette();
    p.setColor(QPalette::Highlight, QColor(225,202,251));
    p.setColor(QPalette::HighlightedText, Qt::black);
    m_undoView->setPalette(p);

    setWidget(m_undoView);
}

HistoryDockWidget::~HistoryDockWidget()
{
    delete ui;
}

void HistoryDockWidget::SetActiveStack(QUndoStack *stack)
{
    m_undoView->setStack(stack);
}

void HistoryDockWidget::SetActiveGroup(QUndoGroup *group)
{
    m_undoView->setGroup(group);
}

void HistoryDockWidget::closeEvent(QCloseEvent *event)
{
    Q_EMIT UncheckMe(4);
    event->accept();
}
