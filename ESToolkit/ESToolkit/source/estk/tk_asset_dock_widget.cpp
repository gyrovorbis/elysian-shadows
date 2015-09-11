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
        _fsModel->addFileType(new SpecularsheetAssetFileType);
        _fsModel->addFileType(new NormalsheetAssetFileType);
        _fsModel->addFileType(new TilesheetAssetFileType);
        _fsModel->addFileType(new LuaScriptAssetFileType);
        _fsModel->addFileType(new LuaBehaviorAssetFileType);
        _fsModel->addFileType(new MapAssetFileType);
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
    default: break;

    }
}


AssetFileSystemModel::AssetFileSystemModel(void) {
    connect(this, SIGNAL(directoryLoaded(QString)), this, SLOT(_onDirectoryLoaded(QString)));
}

AssetFileSystemModel::~AssetFileSystemModel(void) {
    for(auto it: _customDirTypes)   delete it;
    for(auto it: _customFileTypes)  delete it;
    for(auto it: _customData)       delete it;

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

    //qDebug() << "***** PROCESSING >> " << str;

    auto dirs = thisDir.entryList(QDir::NoDotAndDotDot|QDir::AllDirs);

    for(auto dir: dirs) {
        for(auto  dirType : _customDirTypes) {
            path = thisDir.absoluteFilePath(dir);
            //qDebug() << "ASSESSING PATH" << path;
            auto* dirData = dirType->fromFilePath(path);
            if(dirData) {
                //qDebug() << "REGISTERING" << str;
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
                break;
            }
        }
    }


}

void AssetFileSystemModel::_onSelectionChanged(const QItemSelection &selected, const QItemSelection& /*deselected*/) {
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

CustomAssetFileType::CustomAssetFileType(const QString name, const QIcon icon):
    _typeName(name), _icon(icon) {}


AreaAssetFileType::AreaAssetFileType(void): CustomAssetFileType("Area", QIcon(TK_AREA_ASSET_ICON)) {}
AreaAssetFileType::AreaAssetFileType(TkArea*const area): AreaAssetFileType() {
    _area = area;
}

auto AreaAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists("map.bin")) {
        return new AreaAssetFileType(getAssets()->findArea(path));
    }
    return nullptr;
}

void AreaAssetFileType::select(QModelIndex& /*index*/) {
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_area)));
}

void AreaAssetFileType::doubleClick(const QModelIndex& /*index*/) {
    getAssets()->setCurrentArea(_area);
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_area)));
}





LevelAssetFileType::LevelAssetFileType(void): CustomAssetFileType("Level", QIcon(TK_LEVEL_ASSET_ICON)) {}
LevelAssetFileType::LevelAssetFileType(TkLevel*const level): LevelAssetFileType() {
    _level = level;
}

auto LevelAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists("area") && dir.exists("tile")) {
        return new LevelAssetFileType(getAssets()->findLevel(path));
    }
    return nullptr;
}

void LevelAssetFileType::select(QModelIndex& /*index*/) {
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_level)));
}

void LevelAssetFileType::doubleClick(const QModelIndex& /*index*/) {
    getAssets()->setCurrentLevel(_level);
    getAssets()->setCurrentSelection(static_cast<Selectable*>(static_cast<TreeNode*>(_level)));
}



auto SpecularsheetAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == "specularsheet.png") return new SpecularsheetAssetFileType();
    return nullptr;
}

auto NormalsheetAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == "normalsheet.png") return new NormalsheetAssetFileType();
    return nullptr;
}


auto TilesheetAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == "tilesheet.png") return new TilesheetAssetFileType();
    return nullptr;
}


auto MapAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == "map.bin") return new MapAssetFileType();
    return nullptr;
}

auto LuaScriptAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QFileInfo file(QFile(path).fileName());
    if(file.fileName() == "area_init.lua" || file.fileName() == "level_init.lua") return new LuaScriptAssetFileType();
    return nullptr;
}

auto LuaBehaviorAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* {
    QFile file(path);
    if(file.fileName().split("_behav.lua").size() > 1) return new LuaBehaviorAssetFileType();
    return nullptr;
}

