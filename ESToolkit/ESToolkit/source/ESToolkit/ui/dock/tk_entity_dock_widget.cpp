#include <QCloseEvent>
#include <ESToolkit/ui/dock/tk_entity_dock_widget.h>
#include <ESToolkit/ui/widget/tk_entity_tree_widget.h>
#include "ui_tk_entity_dock_widget.h"

EntityDockWidget::EntityDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EntityDockWidget)
{
    ui->setupUi(this);
}

EntityDockWidget::~EntityDockWidget()
{
    delete ui;
}

void EntityDockWidget::closeEvent(QCloseEvent *event)
{
    Q_EMIT UncheckMe(1);
    event->accept();
}


EntityTreeWidget* EntityDockWidget::getTreeWidget(void) {
    return ui->m_entityTreeWidget;
}
