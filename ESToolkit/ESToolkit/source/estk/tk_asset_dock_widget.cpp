#include <QCloseEvent>
#include <QFileSystemModel>
#include <estk/tk_asset_dock_widget.h>
#include <asset/tk_area.h>
#include <asset/tk_level.h>
#include <core/tk_file_utilities.h>
#include <asset/tk_project.h>
#include "ui_tk_asset_dock_widget.h"

AssetDockWidget::AssetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::AssetDockWidget)
{
    _ui->setupUi(this);

    connect(getAssets(), SIGNAL(projectChanged(Project*const)), this, SLOT(_onProjectChanged(Project*const)));
    connect(getAssets(), &Assets::areaChanged, this, &AssetDockWidget::_onAreaChanged);
    connect(getAssets(), &Assets::levelChanged, this, &AssetDockWidget::_onLevelChanged);
    connect(getAssets(), &Assets::selectionChanged, this, &AssetDockWidget::_onSelectionChanged);

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
        _fsModel = new AssetFileSystemModel;
        _ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        //connect(_ui->treeView, SIGNAL(itemChanged))
        _fsModel->addDirectoryType(new AreaAssetFileType);
        _fsModel->addDirectoryType(new LevelAssetFileType);
        _fsModel->setRootPath(project->getNodePath());
        qDebug() << "LA NODE PATH" << project->getNodePath();
        //model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        _fsModel->setResolveSymlinks(true);
        _ui->treeView->setModel(_fsModel);
        _ui->treeView->hideColumn(1);
        _ui->treeView->hideColumn(3);
        _ui->treeView->setRootIndex(_fsModel->index(project->getNodePath()));
        connect(_ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), _fsModel, SLOT(_onSelectionChanged(const QItemSelection&,const QItemSelection&)));
        connect(_ui->treeView, SIGNAL(doubleClicked(const QModelIndex&)), _fsModel, SLOT(_onDoubleClick(const QModelIndex&)));
        _onLevelChanged(getAssets()->getCurrentLevel());
        _onAreaChanged(getAssets()->getCurrentArea());
    }
}

void AssetDockWidget::_onAreaChanged(TkArea*const area) {
    if(area && _fsModel) {
        const auto absPath = FileUtilities::getAbsoluteAssetPath(area->getNodePath());
        const auto index = _fsModel->index(absPath);
        if(index.isValid()) {
            _ui->treeView->setCurrentIndex(index);
        }
    }
}

void AssetDockWidget::_onLevelChanged(TkLevel*const level) {
    if(level && _fsModel) _ui->treeView->setCurrentIndex(_fsModel->index(FileUtilities::getAbsoluteAssetPath(level->getNodePath())));
}

void AssetDockWidget::_onSelectionChanged(const Selectable::TYPE type, Selectable *const selection) {
    switch(type) {
    case Selectable::AREA:
        _onAreaChanged(static_cast<TkArea*>(selection));
        break;
    case Selectable::LEVEL:
        _onLevelChanged(static_cast<TkLevel*>(selection));
        break;

    }
}


AssetFileSystemModel::AssetFileSystemModel(void) {
    connect(this, SIGNAL(directoryLoaded(QString)), this, SLOT(_onDirectoryLoaded(QString)));
}

QVariant AssetFileSystemModel::data(const QModelIndex &index, int role) const {
    QString path = filePath(index);

    //qDebug() << "ASSET LOOKUP" << path;

    if(!path.isNull()) {
        auto data = _customData.find(path);
        if(data != _customData.end()) return data.value()->data(index, role);
    }


    return QFileSystemModel::data(index, role);
}

void AssetFileSystemModel::_onDirectoryLoaded(const QString str) {
    QDir thisDir(str);
    QString path;

    qDebug() << "***** PROCESSING >> " << str;

    auto dirs = thisDir.entryList(QDir::NoDotAndDotDot|QDir::AllDirs);

    for(auto dir: dirs) {
        for(auto  dirType : _customDirTypes) {
            path = thisDir.absoluteFilePath(dir);
            qDebug() << "ASSESSING PATH" << path;
            auto* dirData = dirType->fromFilePath(path);
            if(dirData) {
                qDebug() << "REGISTERING" << str;
                _customData[path] = dirData;
                break;
            }
        }
    }

    auto files = thisDir.entryList(QDir::Files);

    for(auto file: files) {
        for(auto  fileType : _customFileTypes) {
            path = thisDir.absoluteFilePath(file);
            qDebug() << "ASSESSING PATH" << path;
            auto* fileData = fileType->fromFilePath(path);
            if(fileData) {
                qDebug() << "REGISTERING" << str;
                _customData[path] = fileData;
            }
        }
    }


}

void AssetFileSystemModel::_onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    auto indeces = selected.indexes();

    for(auto it : indeces) {
        auto data = _customData.find(filePath(it));
        if(data != _customData.end()) data.value()->select(it);
    }

}

void AssetFileSystemModel::_onDoubleClick(const QModelIndex &index) {
    auto data = _customData.find(filePath(index));
    if(data != _customData.end()) data.value()->doubleClick(index);
}


AreaAssetFileType::AreaAssetFileType(TkArea*const area): CustomAssetFileType(), _area(area) {}
QVariant AreaAssetFileType::data(const QModelIndex &index, int role) {

    if(role == Qt::DisplayRole && index.column() == 2) return QVariant("Area");

    return static_cast<const QFileSystemModel*>(index.model())->QFileSystemModel::data(index, role);


}
auto AreaAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists("map.bin")) {
        return new AreaAssetFileType(getAssets()->findArea(path));
    }
    return nullptr;
}

void AreaAssetFileType::select(QModelIndex &index) {
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_area)));
}

void AreaAssetFileType::doubleClick(const QModelIndex &index) {
    getAssets()->setCurrentArea(_area);
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_area)));
}






LevelAssetFileType::LevelAssetFileType(TkLevel*const level): CustomAssetFileType(), _level(level) {}
QVariant LevelAssetFileType::data(const QModelIndex &index, int role) {

    if(role == Qt::DisplayRole && index.column() == 2) return QVariant("Level");

    return static_cast<const QFileSystemModel*>(index.model())->QFileSystemModel::data(index, role);


}
auto LevelAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists("area") && dir.exists("tile")) {
        return new LevelAssetFileType(getAssets()->findLevel(path));
    }
    return nullptr;
}

void LevelAssetFileType::select(QModelIndex &index) {
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_level)));
}

void LevelAssetFileType::doubleClick(const QModelIndex &index) {
    getAssets()->setCurrentLevel(_level);
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_level)));
}



