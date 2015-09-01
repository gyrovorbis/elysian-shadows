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
    virtual QMenu*  contextMenu(QModelIndex& index);
    virtual QIcon   icon(QModelIndex& index);
    virtual void    select(QModelIndex& index);
    virtual void    doubleClick(QModelIndex& index);
    virtual auto    fromFilePath(const QString path) -> CustomAssetFileType*;
};


class AssetFileSystemModel: public QFileSystemModel {
    Q_OBJECT
private:
    QList<CustomAssetFileType*>             _customDirTypes;
    QList<CustomAssetFileType*>             _customFileTypes;

    QHash<QString, CustomAssetFileType*>    _customData;

private Q_SLOTS:
    void _onDirectoryLoaded(const QString str);
public:
    void addDirectoryType(CustomAssetFileType* type);
    void addFileType(CustomAssetFileType* type);
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
private:
    Ui::AssetDockWidget*    _ui = nullptr;
};




void AssetFileSystemModel::addFileType(CustomAssetFileType *type) {
    _customFileTypes.
}


#endif // ASSETDOCKWIDGET_H
