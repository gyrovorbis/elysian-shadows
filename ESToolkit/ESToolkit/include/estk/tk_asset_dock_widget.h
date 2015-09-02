#ifndef TK_ASSET_DOCK_WIDGET_H
#define TK_ASSET_DOCK_WIDGET_H

#include <QDockWidget>
#include <QFileSystemModel>
#include <asset/tk_asset_manager.h>
#include <QHash>

namespace Ui {
    class AssetDockWidget;
}



class CustomAssetFileType {
public:
    virtual QMenu*      contextMenu(QModelIndex& index);
    //virtual QIcon       icon(QModelIndex& index);
    virtual void        select(QModelIndex& index);
    virtual QVariant    data(const QModelIndex& index, int role=Qt::DisplayRole);
    virtual void        doubleClick(const QModelIndex& index);
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType*;
};

class AreaAssetFileType: public CustomAssetFileType, public AssetManager {
private:
    TkArea* _area = nullptr;
public:
                        AreaAssetFileType(void) = default;
                        AreaAssetFileType(TkArea*const area);
    virtual QVariant    data(const QModelIndex &index, int role=Qt::DisplayRole) override;
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
};

class LevelAssetFileType: public CustomAssetFileType, public AssetManager {
private:
    TkLevel* _level = nullptr;
public:
                        LevelAssetFileType(void) = default;
                        LevelAssetFileType(TkLevel*const level);
    virtual QVariant    data(const QModelIndex &index, int role=Qt::DisplayRole) override;
    virtual auto        fromFilePath(const QString path) -> CustomAssetFileType* override;
    virtual void        select(QModelIndex &index) override;
    virtual void        doubleClick(const QModelIndex& index) override;
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






inline QMenu* CustomAssetFileType::contextMenu(QModelIndex& index) { return nullptr; }
inline void CustomAssetFileType::select(QModelIndex& index) { }
inline QVariant CustomAssetFileType::data(const QModelIndex& index, int role) { return index.model()->data(index, role); }
inline void CustomAssetFileType::doubleClick(const QModelIndex& index) { }
inline auto CustomAssetFileType::fromFilePath(const QString path) -> CustomAssetFileType* { return nullptr; }


inline void AssetFileSystemModel::addFileType(CustomAssetFileType *type) {
    _customFileTypes.push_back(type);
}

inline void AssetFileSystemModel::addDirectoryType(CustomAssetFileType *type) {
    _customDirTypes.push_back(type);
}




#endif // ASSETDOCKWIDGET_H
