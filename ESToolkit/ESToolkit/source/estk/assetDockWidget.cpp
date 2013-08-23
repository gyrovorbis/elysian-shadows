#include <QCloseEvent>
#include "assetDockWidget.h"
#include "ui_assetdockwidget.h"

AssetDockWidget::AssetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AssetDockWidget)
{
    ui->setupUi(this);
}

AssetDockWidget::~AssetDockWidget()
{
    delete ui;
}

void AssetDockWidget::ClearTreeView()
{
    ui->_assetTreeWidget->clear();
}

void AssetDockWidget::closeEvent(QCloseEvent *event)
{
    emit UncheckMe(0);
    event->accept();
}
