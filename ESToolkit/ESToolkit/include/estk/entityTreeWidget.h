#ifndef ENTITY_TREEWIDGET_H
#define ENTITY_TREEWIDGET_H

#include <QTreeWidget>
#include "assetManager.h"

class TkArea;
typedef TkArea Area;
class TkEntity;
typedef TkEntity Entity;

class EntityTreeWidget: public QTreeWidget, public AssetManager {
    Q_OBJECT
private:
    Area *_curArea;
    //bool _ignoreLayerChange = true;
public:
    EntityTreeWidget(QWidget *parent=nullptr);
private slots:
    void OnAreaChange(Area * const area);
    void OnAddEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void OnRemoveEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void OnEntitySelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void OnSelectionChanged(const Selectable::TYPE, Selectable *const);
    void onEntityParented(Entity*const child, Entity*const parent);
    //void onLayerChange(const Assets::LAYER_TYPE layer);

signals:
    void entityLayerActivate(void);

};

#endif // ENTITY_TREEWIDGET_H
