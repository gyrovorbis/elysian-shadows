#ifndef TK_ASSET_DOCK_WIDGET_H
#define TK_ASSET_DOCK_WIDGET_H

#include <QDockWidget>
#include <QFileSystemModel>
#include <asset/tk_asset_manager.h>
#include <QHash>


#define TK_SPECULARSHEET_ASSET_ICON ":/assetIcons/images/_0000_specularsheet_icon.png"
#define TK_NORMALSHEET_ASSET_ICON   ":/assetIcons/images/_0001_normalsheet_icon.png"
#define TK_TILESHEET_ASSET_ICON     ":/assetIcons/images/_0002_tilesheet_icon.png"
#define TK_PREFAB_ASSET_ICON        ":/assetIcons/images/_0003_prefabs_icon.png"
#define TK_LUA_SCRIPT_ASSET_ICON    ":/assetIcons/images/_0004_LS.png"
#define TK_LUA_BEHAVIOR_ASSET_ICON  ":/assetIcons/images/_0005_LB.png"
#define TK_MAP_ASSET_ICON           ":/assetIcons/images/_0006_BIN.png"
#define TK_LEVEL_ASSET_ICON         ":/assetIcons/images/_0007_levels_icon.png"
#define TK_AREA_ASSET_ICON          ":/assetIcons/images/_0008_areas_icon.png"

namespace Ui {
    class AssetDockWidget;
}



class CustomAssetFileType {
private:
    QString             _typeName;
    QIcon               _icon;
public:
                        CustomAssetFileType(const QString name, const QIcon icon);
    virtual QMenu*      contextMenu(QModelIndex& index);
    virtual void        select(QModelIndex& index);
    virtual QVariant    data(const QModelIndex& index, int role=Qt::DisplayRole);
    virtual void        doubleClick(const QModelIndex& index);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType*;
};

class AreaAssetFileType: public CustomAssetFileType, public AssetManager {
private:
    TkArea*             _area = nullptr;
public:
                        AreaAssetFileType(void);
                        AreaAssetFileType(TkArea*const area);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
};

class LevelAssetFileType: public CustomAssetFileType, public AssetManager {
private:
    TkLevel*            _level = nullptr;
public:
                        LevelAssetFileType(void);
                        LevelAssetFileType(TkLevel*const level);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
};

class SpecularsheetAssetFileType: public CustomAssetFileType {
public:
                        SpecularsheetAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;

};

class NormalsheetAssetFileType: public CustomAssetFileType {
public:
                        NormalsheetAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};

class TilesheetAssetFileType: public CustomAssetFileType {
public:
                        TilesheetAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};

class PrefabAssetFileType: public CustomAssetFileType {
public:
                        PrefabAssetFileType(void);
//    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};

class LuaScriptAssetFileType: public CustomAssetFileType {
public:
                        LuaScriptAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};

class LuaBehaviorAssetFileType: public CustomAssetFileType {
public:
                        LuaBehaviorAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};

class MapAssetFileType: public CustomAssetFileType {
public:
                        MapAssetFileType(void);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
};


class AssetFileSystemModel: public QFileSystemModel {
    Q_OBJECT
private:
    QList<CustomAssetFileType*>             _customDirTypes;
    QList<CustomAssetFileType*>             _customFileTypes;

    QHash<QString, CustomAssetFileType*>    _customData;

private Q_SLOTS:
    void                _onDirectoryLoaded(const QString str);
    void                _onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void                _onDoubleClick(const QModelIndex& index);
public:
                        AssetFileSystemModel(void);
    virtual             ~AssetFileSystemModel(void) override;

    virtual QVariant    data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

    void                addDirectoryType(CustomAssetFileType* type);
    void                addFileType(CustomAssetFileType* type);
};

class AssetDockWidget: public QDockWidget, public AssetManager
{
    Q_OBJECT
public:
    explicit                AssetDockWidget(QWidget *parent = nullptr);
    virtual                 ~AssetDockWidget(void);
    void                    clearTreeView(void);
Q_SIGNALS:
    void                    UncheckMe(int);
protected:
    void                    closeEvent(QCloseEvent *event);
protected Q_SLOTS:
    void                    _onProjectChanged(Project*const project);
    void                    _onAreaChanged(TkArea*const area);
    void                    _onLevelChanged(TkLevel*const level);
    void                    _onSelectionChanged(const Selectable::TYPE type, Selectable *const selection);
private:
    Ui::AssetDockWidget*    _ui         = nullptr;
    AssetFileSystemModel*   _fsModel    = nullptr;
};







// ======= INLINEZ =======



inline QMenu* CustomAssetFileType::contextMenu(QModelIndex& index) { return nullptr; }
inline void CustomAssetFileType::select(QModelIndex& index) { }
inline QVariant CustomAssetFileType::data(const QModelIndex& index, int role) {

    if(role == Qt::DisplayRole && index.column() == 2 && !_typeName.isNull()) return _typeName;
    if(role == Qt::DecorationRole && !_icon.isNull()) return _icon;

    return static_cast<const QFileSystemModel*>(index.model())->QFileSystemModel::data(index, role);

}
inline void CustomAssetFileType::doubleClick(const QModelIndex& index) { }
inline auto CustomAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* { return nullptr; }


inline void AssetFileSystemModel::addFileType(CustomAssetFileType *type) {
    _customFileTypes.push_back(type);
}

inline void AssetFileSystemModel::addDirectoryType(CustomAssetFileType *type) {
    _customDirTypes.push_back(type);
}

inline SpecularsheetAssetFileType::SpecularsheetAssetFileType(void): CustomAssetFileType("Specular Sheet", QIcon(TK_SPECULARSHEET_ASSET_ICON)) {}
inline NormalsheetAssetFileType::NormalsheetAssetFileType(void): CustomAssetFileType("Normal Sheet", QIcon(TK_NORMALSHEET_ASSET_ICON)) {}
inline TilesheetAssetFileType::TilesheetAssetFileType(void): CustomAssetFileType("Tile Sheet", QIcon(TK_TILESHEET_ASSET_ICON)) {}
inline PrefabAssetFileType::PrefabAssetFileType(void): CustomAssetFileType("Prefab", QIcon(TK_PREFAB_ASSET_ICON)) {}
inline LuaScriptAssetFileType::LuaScriptAssetFileType(void): CustomAssetFileType("Lua Script", QIcon(TK_LUA_SCRIPT_ASSET_ICON)) {}
inline LuaBehaviorAssetFileType::LuaBehaviorAssetFileType(void): CustomAssetFileType("Lua Behavior", QIcon(TK_LUA_BEHAVIOR_ASSET_ICON)) {}
inline MapAssetFileType::MapAssetFileType(void): CustomAssetFileType("Map", QIcon(TK_MAP_ASSET_ICON)) {}


#endif // ASSETDOCKWIDGET_H
