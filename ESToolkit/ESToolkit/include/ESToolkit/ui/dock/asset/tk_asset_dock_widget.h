#ifndef TK_ASSET_DOCK_WIDGET_H
#define TK_ASSET_DOCK_WIDGET_H

#include <QDockWidget>
#include <QFileSystemModel>
#include <ESToolkit/core/tk_asset_manager.h>
#include <crtp/elysian_lua_variant.hpp>
#include <QHash>

#define TK_SPECULARSHEET_ASSET_ICON         ":/assetIcons/images/_0000_specularsheet_icon.png"
#define TK_NORMALSHEET_ASSET_ICON           ":/assetIcons/images/_0001_normalsheet_icon.png"
#define TK_TILESHEET_ASSET_ICON             ":/assetIcons/images/_0002_tilesheet_icon.png"
#define TK_PREFAB_ASSET_ICON                ":/assetIcons/images/_0003_prefabs_icon.png"
#define TK_LUA_SCRIPT_ASSET_ICON            ":/assetIcons/images/_0004_LS.png"
#define TK_LUA_BEHAVIOR_ASSET_ICON          ":/assetIcons/images/_0005_LB.png"
#define TK_MAP_ASSET_ICON                   ":/assetIcons/images/_0006_BIN.png"
#define TK_LEVEL_ASSET_ICON                 ":/assetIcons/images/_0007_levels_icon.png"
#define TK_AREA_ASSET_ICON                  ":/assetIcons/images/_0008_areas_icon.png"
#define TK_VMU_ICON_ASSET_ICON              ":/assetIcons/images/vmu_icon.png"

#define TK_SPECULARSHEET_ASSET_NAME_TOKEN   "specularsheet.png"
#define TK_NORMALSHEET_ASSET_NAME_TOKEN     "normalsheet.png"
#define TK_TILESHEET_ASSET_NAME_TOKEN       "tilesheet.png"
#define TK_PREFAB_ASSET_NAME_TOKEN          "_prefab_data.lua"
#define TK_LUA_BEHAVIOR_ASSET_NAME_TOKEN    "_behav.lua"
#define TK_MAP_ASSET_NAME_TOKEN             "map.bin"
#define TK_VMU_ICON_ASSET_NAME_TOKEN        ".vmuico"

class QMenu;
class QItemSelection;
class QStandardItem;

namespace Ui {
    class AssetDockWidget;
}

class CustomAssetFileType: public QObject {
    Q_OBJECT
public:
    enum FONT_STATE { FONT_INACTIVE, FONT_ACTIVE, FONT_INVALID };
protected:
    QString                 _filePath;
    QString                 _typeName;
    QString                 _toolTip;
    QString                 _whatsThis;
    QIcon                   _icon;
    FONT_STATE              _fontState  = FONT_INACTIVE;
    bool                    _modified   = false;

Q_SIGNALS:
    void                    itemChanged(const QVector<int> &roels=QVector<int>());

public:
                            CustomAssetFileType(QString typeName, QIcon icon=QIcon(), QString filePath=QString(), QString toolTip=QString(), QString whatsThis=QString());
    virtual                 ~CustomAssetFileType(void) = default;

    void                    setFontState(const FONT_STATE state);
    FONT_STATE              getFontState(void) const;
    void                    setModified(const bool val);
    QString                 getFilePath(void) const;
    QString                 getTypeName(void) const;
    QString                 getToolTip(void) const;
    void                    setToolTip(QString str);
    QString                 getWhatsThisText(void) const;
    void                    setWhatsThisText(QString text);

    virtual QMenu*          contextMenu(const QModelIndex& index);
    virtual void            select(QModelIndex& index);
    virtual QVariant        data(const QModelIndex& index, int role=Qt::DisplayRole);
    virtual void            doubleClick(const QModelIndex& index);
    virtual auto            fromFilePath(QString path) -> CustomAssetFileType*;
    virtual QMimeData*      mimeData(void) const;
    virtual Qt::ItemFlags   flags(void) const;
    virtual bool            canDropMimeData(const QMimeData* data, Qt::DropAction action) const;
    virtual bool            dropMimeData(const QMimeData* data, Qt::DropAction action);
    virtual void            fileRemoved(void);
};

class ResourceRefAssetFileType: public CustomAssetFileType {


};

class AreaAssetFileType: public CustomAssetFileType, public AssetManager {
    Q_OBJECT
private:
    TkArea*             _area = nullptr;

private Q_SLOTS:
    void                _onUndoStackCleanChanged(const bool clean);
    void                _onValidChanged(const bool valid);
public:
                        AreaAssetFileType(QString filePath=QString());
                        AreaAssetFileType(QString filePath, TkArea*const area);

    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
    virtual void        fileRemoved(void) override;
};

class LevelAssetFileType: public CustomAssetFileType, public AssetManager {
    Q_OBJECT
private:
    TkLevel*            _level = nullptr;

private Q_SLOTS:
void                    _onValidChanged(const bool clean);

public:
                        LevelAssetFileType(QString filePath=QString());
                        LevelAssetFileType(QString filePath, TkLevel*const level);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
    virtual void        fileRemoved(void) override;
};

class SpecularsheetAssetFileType: public CustomAssetFileType {
public:
                        SpecularsheetAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;

};

class NormalsheetAssetFileType: public CustomAssetFileType {
public:
                        NormalsheetAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
};

class TilesheetAssetFileType: public CustomAssetFileType {
public:
                        TilesheetAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
};

class PrefabAssetFileType: public CustomAssetFileType, public AssetManager {
    Q_OBJECT
private:
    QString             _prefabName;
    LuaVariant          _prefabRef;

private Q_SLOTS:
    void                _onItemChanged(QStandardItem* item);

public:
                        PrefabAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        doubleClick(const QModelIndex& index) override;
    virtual QMenu*      contextMenu(const QModelIndex& index) override;
    virtual QMimeData*  mimeData(void) const override;

    static QString      prefabNameFromFilePath(QString path);
    static QString      prefabifyFilePath(QString path);
};

class LuaScriptAssetFileType: public CustomAssetFileType {
public:
                        LuaScriptAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        doubleClick(const QModelIndex &index) override;
};

class LuaBehaviorAssetFileType: public CustomAssetFileType {
public:
                        LuaBehaviorAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        doubleClick(const QModelIndex &index) override;
};

class MapAssetFileType: public CustomAssetFileType {
public:
                        MapAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
};

class WavAssetFileType: public CustomAssetFileType {
private:
    QString             _assetRelativePath;
public:
                        WavAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        doubleClick(const QModelIndex &index) override;
    virtual QVariant    data(const QModelIndex &index, int role) override;
    virtual QMenu*      contextMenu(const QModelIndex& index) override;
};

class VmuIconAssetFileType: public CustomAssetFileType {
private:
    QString             _assetRelativePath;
public:
                        VmuIconAssetFileType(QString filePath=QString());
    virtual auto        fromFilePath(QString path) -> CustomAssetFileType* override;
    virtual void        doubleClick(const QModelIndex &index) override;
};

class AssetFileSystemModel: public QFileSystemModel {
    Q_OBJECT
private:
    QList<CustomAssetFileType*>             _customDirTypes;
    QList<CustomAssetFileType*>             _customFileTypes;

    QHash<QString, CustomAssetFileType*>    _customData;

    void                    _onFileAdded(const QString filename);
    void                    _onFileRemoved(const QString filename);
private Q_SLOTS:
    void                    _onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void                    _onDoubleClick(const QModelIndex& index);
    void                    _onItemChanged(const QVector<int>& roles);
    void                    _onRowsInserted(const QModelIndex& parent, int first, int last);
    void                    _onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);

public:
                            AssetFileSystemModel(void);
    virtual                 ~AssetFileSystemModel(void) override;

    void                    addDirectoryType(CustomAssetFileType* type);
    void                    addFileType(CustomAssetFileType* type);
    CustomAssetFileType*    getCustomAssetFileType(QString fileName);
    QMenu*                  customContextMenuRequested(const QModelIndex& index);

    //overridden from QFileSystemModel
    virtual QVariant        data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
    virtual Qt::DropActions supportedDragActions(void) const override;
    virtual Qt::DropActions supportedDropActions(void) const override;
    virtual Qt::ItemFlags   flags(const QModelIndex &index) const override;
    virtual QMimeData*      mimeData(const QModelIndexList &indexes) const override;
    virtual bool            canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    virtual bool            dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};

class AssetDockWidget: public QDockWidget, public AssetManager
{
    Q_OBJECT
public:
    explicit                AssetDockWidget(QWidget *parent = nullptr);
    virtual                 ~AssetDockWidget(void);
    void                    clearTreeView(void);
    AssetFileSystemModel*   getAssetFileSystemModel(void) const;
Q_SIGNALS:
    void                    UncheckMe(int);
protected:
    void                    closeEvent(QCloseEvent *event);
protected Q_SLOTS:
    void                    _onProjectAdded(Project*const project);
    void                    _onProjectRemoved(Project*const project);
    void                    _onAreaChanged(TkArea*const area);
    void                    _onLevelChanged(TkLevel*const level);
    void                    _onSelectionChanged(const Selectable::TYPE type, Selectable *const selection);
    void                    _onCustomContextMenuRequested(const QPoint& pos);
private:
    Ui::AssetDockWidget*    _ui         = nullptr;
    AssetFileSystemModel*   _fsModel    = nullptr;
};







// ======= INLINEZ =======

inline AssetFileSystemModel* AssetDockWidget::getAssetFileSystemModel(void) const {
    return _fsModel;
}

inline QMenu* CustomAssetFileType::contextMenu(const QModelIndex& /*index*/) { return nullptr; }
inline void CustomAssetFileType::select(QModelIndex& /*index*/) { }
inline void CustomAssetFileType::doubleClick(const QModelIndex& /*index*/) { }
inline auto CustomAssetFileType::fromFilePath(QString /*path*/) -> CustomAssetFileType* { return nullptr; }
inline void CustomAssetFileType::setFontState(const FONT_STATE state) { _fontState = state; Q_EMIT itemChanged(); }
inline auto CustomAssetFileType::getFontState(void) const -> FONT_STATE { return _fontState; }
inline void CustomAssetFileType::setModified(const bool val) { _modified = val; Q_EMIT itemChanged(); }
inline QString CustomAssetFileType::getFilePath(void) const { return _filePath; }
inline QString CustomAssetFileType::getTypeName(void) const { return _typeName; }
inline void CustomAssetFileType::fileRemoved(void) {}
inline QString CustomAssetFileType::getToolTip(void) const { return _toolTip; }
inline void CustomAssetFileType::setToolTip(QString str) { _toolTip = str; }
inline QString CustomAssetFileType::getWhatsThisText(void) const { return _whatsThis; }
inline void CustomAssetFileType::setWhatsThisText(QString text) { _whatsThis = text; }


inline void AssetFileSystemModel::addFileType(CustomAssetFileType *type) {
    _customFileTypes.push_back(type);
}

inline void AssetFileSystemModel::addDirectoryType(CustomAssetFileType *type) {
    _customDirTypes.push_back(type);
}

inline SpecularsheetAssetFileType::SpecularsheetAssetFileType(QString filePath): CustomAssetFileType("Specular Sheet", QIcon(TK_SPECULARSHEET_ASSET_ICON), std::move(filePath)) {}
inline NormalsheetAssetFileType::NormalsheetAssetFileType(QString filePath): CustomAssetFileType("Normal Sheet", QIcon(TK_NORMALSHEET_ASSET_ICON), std::move(filePath)) {}
inline TilesheetAssetFileType::TilesheetAssetFileType(QString filePath): CustomAssetFileType("Tile Sheet", QIcon(TK_TILESHEET_ASSET_ICON), std::move(filePath)) {}
inline LuaScriptAssetFileType::LuaScriptAssetFileType(QString filePath): CustomAssetFileType("Lua Script", QIcon(TK_LUA_SCRIPT_ASSET_ICON), std::move(filePath)) {}
inline LuaBehaviorAssetFileType::LuaBehaviorAssetFileType(QString filePath): CustomAssetFileType("Lua Behavior", QIcon(TK_LUA_BEHAVIOR_ASSET_ICON), std::move(filePath)) {}
inline MapAssetFileType::MapAssetFileType(QString filePath): CustomAssetFileType("Map", QIcon(TK_MAP_ASSET_ICON), std::move(filePath)) {}
inline VmuIconAssetFileType::VmuIconAssetFileType(QString filePath): CustomAssetFileType("VMU Icon", QIcon(TK_VMU_ICON_ASSET_ICON), std::move(filePath), "Double-click to preview on VMU[0]!") {
    _fontState = FONT_ACTIVE;
}



#endif // ASSETDOCKWIDGET_H
