#include <QCloseEvent>
#include "tk_asset_dock_widget.h"
#include "ui_tk_asset_dock_widget.h"

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

void AssetDockWidget::clearTreeView()
{
    ui->_assetTreeWidget->clear();
}

void AssetDockWidget::closeEvent(QCloseEvent *event)
{
    Q_EMIT UncheckMe(0);
    event->accept();
}
