#include <QCloseEvent>
#include <QFileSystemModel>
#include <estk/tk_asset_dock_widget.h>
#include <asset/tk_area.h>
#include <asset/tk_level.h>
#include <core/tk_file_utilities.h>
#include <asset/tk_project.h>
#include "ui_tk_asset_dock_widget.h"
#include <core/tk_lua_table_selectable.h>
#include <QDebug>
#include <core/tk_menu_stack.h>
#include <QMenu>
#include <core/tk_mime_data.h>

AssetDockWidget::AssetDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::AssetDockWidget)
{
    _ui->setupUi(this);

    //enable drag n drop
    _ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    _ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->treeView->setDragEnabled(true);
    //_ui->treeView->setAcceptDrops(true);
    //_ui->treeView->setDropIndicatorShown(true);

    connect(getAssets(), SIGNAL(projectAdded(Project*const)), this, SLOT(_onProjectAdded(Project*const)));
    connect(getAssets(), SIGNAL(projectRemoved(Project*const)), this, SLOT(_onProjectRemoved(Project*const)));
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

void AssetDockWidget::_onProjectAdded(Project * const project) {
    if(project) {
        _fsModel = new AssetFileSystemModel;
        _ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        _ui->treeView->	setContextMenuPolicy(Qt::CustomContextMenu);
        //connect(_ui->treeView, SIGNAL(itemChanged))
        _fsModel->addDirectoryType(new AreaAssetFileType);
        _fsModel->addDirectoryType(new LevelAssetFileType);
        _fsModel->addFileType(new SpecularsheetAssetFileType);
        _fsModel->addFileType(new NormalsheetAssetFileType);
        _fsModel->addFileType(new TilesheetAssetFileType);
        _fsModel->addFileType(new LuaScriptAssetFileType);
        _fsModel->addFileType(new LuaBehaviorAssetFileType);
        _fsModel->addFileType(new MapAssetFileType);
        _fsModel->addFileType(new PrefabAssetFileType);
        _fsModel->setRootPath(project->getNodePath());
        //qDebug() << "LA NODE PATH" << project->getNodePath();
        //model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        _fsModel->setResolveSymlinks(true);
        _ui->treeView->setModel(_fsModel);
        _ui->treeView->hideColumn(1);
        _ui->treeView->hideColumn(3);
        _ui->treeView->setRootIndex(_fsModel->index(project->getNodePath()));
        connect(_ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), _fsModel, SLOT(_onSelectionChanged(const QItemSelection&,const QItemSelection&)));
        connect(_ui->treeView, SIGNAL(doubleClicked(const QModelIndex&)), _fsModel, SLOT(_onDoubleClick(const QModelIndex&)));
        connect(_ui->treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(_onCustomContextMenuRequested(const QPoint&)));
        _onLevelChanged(getAssets()->getCurrentLevel());
        _onAreaChanged(getAssets()->getCurrentArea());
    }
}

void AssetDockWidget::_onProjectRemoved(Project * const/* project*/) {
    _ui->treeView->setModel(nullptr);
    delete _fsModel;
    _fsModel = nullptr;
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

void AssetDockWidget::_onCustomContextMenuRequested(const QPoint& pos) {
    QMenu* menu = _fsModel->customContextMenuRequested(_ui->treeView->indexAt(pos));
    if(menu) menu->exec(_ui->treeView->mapToGlobal(pos));
    delete menu;
}


AssetFileSystemModel::AssetFileSystemModel(void) {
    connect(this, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(_onRowsInserted(const QModelIndex&,int,int)));
    connect(this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(_onRowsAboutToBeRemoved(const QModelIndex&,int,int)));
}

AssetFileSystemModel::~AssetFileSystemModel(void) {
    for(auto it: _customDirTypes)   delete it;
    for(auto it: _customFileTypes)  delete it;
    for(auto it: _customData)       delete it;
}

Qt::DropActions AssetFileSystemModel::supportedDragActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
}

Qt::DropActions AssetFileSystemModel::supportedDropActions(void) const {
    return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction | Qt::IgnoreAction | Qt::TargetMoveAction);
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

void AssetFileSystemModel::_onFileAdded(const QString filename) {
    QFileInfo info(filename);
    QString path = info.absoluteFilePath();

    if(info.isDir()) {

        for(auto  dirType : _customDirTypes) {
            auto* dirData = dirType->fromFilePath(path);
            if(dirData) {
                connect(dirData, SIGNAL(itemChanged()), this, SLOT(_onItemChanged()));
                _customData[path] = dirData;
                break;
            }
        }

    } else if(info.isFile()) {

        for(auto  fileType : _customFileTypes) {
            auto* fileData = fileType->fromFilePath(path);
            if(fileData) {
                connect(fileData, SIGNAL(itemChanged()), this, SLOT(_onItemChanged()));
                _customData[path] = fileData;
                break;
            }
        }

    } else {
        warnprintf("AssetFileSystem[%s] - Indexing neither file nor directory, ignoring.");
    }
}

void AssetFileSystemModel::_onFileRemoved(const QString filename) {
    QFileInfo info(filename);
    auto data = _customData.find(info.absoluteFilePath());
    if(data != _customData.end()) {
        delete data.value();
        _customData.erase(data);
    }
}

void AssetFileSystemModel::_onItemChanged(void) {
    CustomAssetFileType* sender = static_cast<CustomAssetFileType*>(QObject::sender());
    auto modelIndex = index(sender->getFilePath());
    if(modelIndex.isValid()) {
        Q_EMIT dataChanged(modelIndex, modelIndex);
    }
}

void AssetFileSystemModel::_onRowsInserted(const QModelIndex& parent, int first, int last) {
    for(int i = first; i <= last; ++i) {
        QString path = filePath(index(i, 0, parent));
        _onFileAdded(path);
    }
}

void AssetFileSystemModel::_onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) {
    for(int i = first; i <= last; ++i) {
        QString path = filePath(index(i, 0, parent));
        _onFileRemoved(path);
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

QMenu* AssetFileSystemModel::customContextMenuRequested(const QModelIndex &index) {
    auto data = _customData.find(filePath(index));
    return (data != _customData.end())? data.value()->contextMenu(index) : nullptr;
}


Qt::ItemFlags AssetFileSystemModel::flags(const QModelIndex &index) const {
    auto data = _customData.find(filePath(index));
    return (data != _customData.end())? data.value()->flags() : QFileSystemModel::flags(index);
}

QMimeData* AssetFileSystemModel::mimeData(const QModelIndexList &indexes) const {
    QList<QMimeData*>           mimeDataList;
    QModelIndexList             defaultModelIndexList;
    QList<CustomAssetFileType*> customDataCache;


    /* Iterate over QModelIndexList immediately adding MimeData for custom asset types (and preventing duplication),
     * and creating a new list containing only default QFileSystemModel QModelIndex types, so we can pass them all at once
     * to the parent class and be efficient, let Qt group them, and be consistent with how the parent and default mime types work.
     */
    for(auto&& modelIndex : indexes) {
        auto data = _customData.find(filePath(modelIndex));
        if(data != _customData.end()) {
            if(!customDataCache.contains(data.value())) { //only add to mimeDataList if we haven't already added the custom type (prevent duplicates for multiple cell selection)
                customDataCache.append(data.value());
                mimeDataList.append(data.value()->mimeData());
            }
        } else {
            defaultModelIndexList.append(modelIndex);
        }
    }

    if(defaultModelIndexList.size()) {
        mimeDataList.append(QFileSystemModel::mimeData(defaultModelIndexList));
    }

    //For multiple MIME types, create a Lua Array of QMimeData
    if(mimeDataList.size() > 1) {
        //only use custom mime type if ALL indices are custom supported types!
        int arraySize = 0;
        LuaVariant mimeDataArray = LuaVariant::newTable();

        //First pass is appending only custom MimeData objects separately
        for(auto&& mimeData: mimeDataList) {
            mimeDataArray[++arraySize] = (void*)mimeData;
        }

        return new TkMimeData(TkMimeData::TYPE_MIME_DATA_ARRAY, std::move(mimeDataArray));

    } else {
        return mimeDataList[0]; //just return the only element
    }
}

bool AssetFileSystemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const {

    if(row == -1 && column == -1) { //dropped directly on parent
        auto customData = _customData.find(filePath(parent));
        if(customData != _customData.end()) return customData.value()->canDropMimeData(data, action);
    }

    return QFileSystemModel::canDropMimeData(data, action, row, column, parent);

}

bool AssetFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if(row == -1 && column == -1) {
        auto customData = _customData.find(filePath(parent));
        if(customData != _customData.end()) {
            if(customData.value()->canDropMimeData(data, action)) {
                return customData.value()->dropMimeData(data, action);
            }
            else return false;
        }
    }
    return QFileSystemModel::dropMimeData(data, action, row, column, parent);
}


CustomAssetFileType::CustomAssetFileType(QString typeName, QIcon icon, QString filePath):
    _filePath(std::move(filePath)),
    _typeName(std::move(typeName)),
    _icon(std::move(icon))
{}

QMimeData* CustomAssetFileType::mimeData(void) const {
    QMimeData* mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl> { QUrl::fromLocalFile(_filePath) });
    return mimeData;
}

bool CustomAssetFileType::canDropMimeData(const QMimeData* /*data*/, Qt::DropAction action) const {
    return action == Qt::IgnoreAction? true : false;
}

bool CustomAssetFileType::dropMimeData(const QMimeData* data, Qt::DropAction action) {
    if (!canDropMimeData(data, action))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    return false;
}

Qt::ItemFlags CustomAssetFileType::flags(void) const {
    return (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
}

QVariant CustomAssetFileType::data(const QModelIndex& index, int role) {
    if(role == Qt::ForegroundRole) {
        switch(_fontState) {
        case FONT_INACTIVE:
            return QVariant(QColor(Qt::gray));
        case FONT_ACTIVE:
            return QVariant(QColor(Qt::black));
        case FONT_INVALID:
            return QVariant(QColor(Qt::darkRed));
        }
    }
    else if(role == Qt::DisplayRole && index.column() == 2 && !_typeName.isNull()) return _typeName;
    else if(role == Qt::DecorationRole && !_icon.isNull()) return _icon;
    else {
        QVariant value = static_cast<const QFileSystemModel*>(index.model())->QFileSystemModel::data(index, role);
        if(role == Qt::DisplayRole && index.column() == 0 && _modified)
            return QString("*")+value.toString();
        return value;
    }
}


AreaAssetFileType::AreaAssetFileType(const QString filePath): CustomAssetFileType("Area", QIcon(TK_AREA_ASSET_ICON), filePath) {}
AreaAssetFileType::AreaAssetFileType(const QString filePath, TkArea*const area):
    AreaAssetFileType(filePath)
{
    _area = area;
    connect(&area->getUndoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(_onUndoStackCleanChanged(bool)));
    connect(area, SIGNAL(validChanged(bool)), this, SLOT(_onValidChanged(bool)));

   // setFontState(area->isValid()? CustomAssetFileType::FONT_ACTIVE : )
}

void AreaAssetFileType::_onUndoStackCleanChanged(const bool clean) {
    setModified(!clean);
}

void AreaAssetFileType::_onValidChanged(const bool valid) {
    setFontState(valid? CustomAssetFileType::FONT_ACTIVE : CustomAssetFileType::FONT_INVALID);
}

auto AreaAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists(TK_MAP_ASSET_NAME_TOKEN)) {
        return new AreaAssetFileType(path, getAssets()->findArea(path));
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

LevelAssetFileType::LevelAssetFileType(const QString filePath): CustomAssetFileType("Level", QIcon(TK_LEVEL_ASSET_ICON), filePath) {}

LevelAssetFileType::LevelAssetFileType(const QString filePath, TkLevel*const level):
    LevelAssetFileType(filePath)
{
    _level = level;
    connect(level, SIGNAL(validChanged(bool)), this, SLOT(_onValidChanged(bool)));
}

void LevelAssetFileType::_onValidChanged(const bool valid) {
    setFontState(valid? CustomAssetFileType::FONT_ACTIVE : CustomAssetFileType::FONT_INVALID);
}

auto LevelAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    QDir dir(path);

    if(dir.exists("area") && dir.exists("tile")) {
        return new LevelAssetFileType(path, getAssets()->findLevel(std::move(path)));
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

auto SpecularsheetAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == TK_SPECULARSHEET_ASSET_NAME_TOKEN) return new SpecularsheetAssetFileType(std::move(path));
    return nullptr;
}

auto NormalsheetAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == TK_NORMALSHEET_ASSET_NAME_TOKEN) return new NormalsheetAssetFileType(std::move(path));
    return nullptr;
}


auto TilesheetAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == TK_TILESHEET_ASSET_NAME_TOKEN) return new TilesheetAssetFileType(std::move(path));
    return nullptr;
}


auto MapAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    if(QFileInfo(QFile(path).fileName()).fileName() == TK_MAP_ASSET_NAME_TOKEN) return new MapAssetFileType(std::move(path));
    return nullptr;
}

auto LuaScriptAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    QFileInfo file(QFile(path).fileName());
    if(file.fileName() == "area_init.lua" || file.fileName() == "level_init.lua") return new LuaScriptAssetFileType(std::move(path));
    return nullptr;
}

auto LuaBehaviorAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    QFile file(path);
    if(file.fileName().split(TK_LUA_BEHAVIOR_ASSET_NAME_TOKEN).size() > 1) return new LuaBehaviorAssetFileType(std::move(path));
    return nullptr;
}

PrefabAssetFileType::PrefabAssetFileType(QString filePath):
    CustomAssetFileType("Prefab", QIcon(TK_PREFAB_ASSET_ICON), filePath)
{
    if(filePath.isEmpty()) return;

    QDir assetDir(FileUtilities::getAbsoluteAssetPath(ELYSIAN_LUA_PREFAB_PATH));
    GY_ASSERT(assetDir.exists());

    _prefabName = prefabNameFromFilePath(_filePath);
    qDebug() << "****** PREFAB RELATIVE FILE PATH " << _prefabName;

    _prefabRef = LuaManager::getPrefab(Q_CSTR(_prefabName));

    if(_prefabRef.getType() != LUA_TTABLE && _prefabRef.getType() != LUA_TUSERDATA) {
        errprintf("Prefab[%s] is not USERDATA or TABLE type! (%s)", Q_CSTR(_prefabName), _prefabRef.getTypeString());
        setFontState(CustomAssetFileType::FONT_INVALID);
    }
}

void PrefabAssetFileType::_onItemChanged(QStandardItem */*item*/) {
    setModified(true);
}

auto PrefabAssetFileType::fromFilePath(QString path) -> CustomAssetFileType* {
    QFile file(path);
    if(file.fileName().split(TK_PREFAB_ASSET_NAME_TOKEN).size() > 1) return new PrefabAssetFileType(std::move(path));
    return nullptr;
}

void PrefabAssetFileType::doubleClick(const QModelIndex& /*index*/) {
    LuaTableSelectable* selectable = new LuaTableSelectable(_prefabRef);
    connect(selectable, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(_onItemChanged(QStandardItem*)));
    getAssets()->setCurrentSelection(selectable);

    if(getFontState() == CustomAssetFileType::FONT_INACTIVE) setFontState(CustomAssetFileType::FONT_ACTIVE);
}

QMenu* PrefabAssetFileType::contextMenu(const QModelIndex& /*index*/) {
    MenuStack menuStack("Prefab");

    menuStack.insertAction("Save",
         [&](bool) {
            LuaManager::saveTable(_prefabRef.getRef(), Q_CSTR(getFilePath()));
            setModified(false);
    });

    menuStack.insertAction("Create Child",
         [&](bool) {
        LuaVariant v = _prefabRef["new"].getVariant()(_prefabRef);
        getAssets()->setCurrentSelection(new LuaTableSelectable(std::move(v)));

    });

    return menuStack.first();
}

QString PrefabAssetFileType::prefabNameFromFilePath(const QString path) {
    QDir assetDir(FileUtilities::getAbsoluteAssetPath(ELYSIAN_LUA_PREFAB_PATH));
    return assetDir.relativeFilePath(path).split(TK_PREFAB_ASSET_NAME_TOKEN)[0];
}

QString PrefabAssetFileType::prefabifyFilePath(const QString path) {
    QString newFilePath = path;
    QDir assetDir(FileUtilities::getAbsoluteAssetPath(ELYSIAN_LUA_PREFAB_PATH));

    //Convert if invalid
    if(assetDir.relativeFilePath(path).split(TK_PREFAB_ASSET_NAME_TOKEN).size() <= 1) {
        newFilePath = newFilePath.split(".")[0];
        newFilePath += QString(TK_PREFAB_ASSET_NAME_TOKEN);
    }

    return newFilePath;
}

QMimeData* PrefabAssetFileType::mimeData(void) const {
    return new TkMimeData(strcmp(_prefabRef.getTypeString(), "Entity") == 0?
                              TkMimeData::TYPE_LUA_ENTITY_PREFAB : TkMimeData::TYPE_LUA_PREFAB, _prefabRef);
}

