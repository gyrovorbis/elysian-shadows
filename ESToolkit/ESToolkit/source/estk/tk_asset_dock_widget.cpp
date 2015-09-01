#include <QCloseEvent>
#include <QFileSystemModel>
#include <estk/tk_asset_dock_widget.h>
#include <core/tk_file_utilities.h>
#include <asset/tk_project.h>
#include "ui_tk_asset_dock_widget.h"

AssetDockWidget::AssetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::AssetDockWidget)
{
    _ui->setupUi(this);

    connect(getAssets(), SIGNAL(projectChanged(Project*const)), this, SLOT(_onProjectChanged(Project*const)));
}

AssetDockWidget::~AssetDockWidget()
{
    delete _ui;
}

void AssetDockWidget::clearTreeView()
{
   // _ui->_assetTreeWidget->clear();
}

void AssetDockWidget::closeEvent(QCloseEvent *event)
{
    Q_EMIT UncheckMe(0);
    event->accept();
}

void AssetDockWidget::_onProjectChanged(Project * const project) {
    if(project) {
        auto* model = new AssetFileSystemModel;

        connect(model, &QFileSystemModel::directoryLoaded, [](QString path) {
            qDebug() << "DIRECTORY LOADED" << path;

        });

        model->setRootPath(project->getNodePath());
        qDebug() << "LA NODE PATH" << project->getNodePath();
        //model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        model->setResolveSymlinks(true);
        _ui->treeView->setModel(model);
        _ui->treeView->hideColumn(1);
        _ui->treeView->hideColumn(3);
        _ui->treeView->setRootIndex(model->index(project->getNodePath()));

    }
}
