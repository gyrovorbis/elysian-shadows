#ifndef ENTITY_TREEWIDGET_H
#define ENTITY_TREEWIDGET_H

#include <QTreeWidget>
#include "assetManager.h"

class Area;
class Entity;

class EntityTreeWidget: public QTreeWidget, public AssetManager {
    Q_OBJECT
private:
    Area *_curArea;
public:
    EntityTreeWidget(QWidget *parent=0);
public slots:
    void OnAreaChange(Area * const area);
    void OnAddEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void OnRemoveEntity(Project *const project, Level *const level, Area *const area, Entity *const entity);
    void OnEntitySelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void OnSelectionChanged(const Selectable::TYPE, Selectable *const);

};

#endif // ENTITY_TREEWIDGET_H
