#ifndef ENTITY_TREEWIDGET_H
#define ENTITY_TREEWIDGET_H

#include <QTreeWidget>
#include <ESToolkit/core/tk_asset_manager.h>

class TkArea;
typedef TkArea Area;
class TkEntity;
typedef TkEntity Entity;

class EntityTreeWidget: public QTreeWidget, public AssetManager {
    Q_OBJECT
private:
    Area*               _curArea    = nullptr;
    bool                _showHidden = false;
    QTreeWidgetItem*    _hiddenItem = nullptr;
    //bool _ignoreLayerChange = true;

protected:
    void _addEntity(Entity* entity);
    void _removeEntity(Entity* entity);


public:
    EntityTreeWidget(QWidget *parent=nullptr);

    bool getShowHidden(void) const;
    void setShowHidden(const bool val);

private Q_SLOTS:
    void OnAreaChange(Area * const area);
    void OnAddEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void OnRemoveEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);

    void OnEntitySelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void _onItemSelectionChanged(void);

    void OnSelectionChanged(const Selectable::TYPE, Selectable *const);
    void onEntityParented(Entity*const child, Entity*const parent);
    //void onLayerChange(const Assets::LAYER_TYPE layer);
    void _onCustomContextMenuRequested(QPoint location);
    void _onItemDoubleClicked(QTreeWidgetItem* item, int column);

Q_SIGNALS:
    void entityLayerActivate(void);

};


// ======= INLINEZ ========
inline bool EntityTreeWidget::getShowHidden(void) const {
    return _showHidden;
}




#endif // ENTITY_TREEWIDGET_H
