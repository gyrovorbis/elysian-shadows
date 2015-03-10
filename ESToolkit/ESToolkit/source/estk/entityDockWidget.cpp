#include <QCloseEvent>
#include "entityDockWidget.h"
#include "entityTreeWidget.h"
#include "ui_entitydockwidget.h"

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
