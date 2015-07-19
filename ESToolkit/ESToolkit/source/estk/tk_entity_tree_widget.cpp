#include "tk_entity_tree_widget.h"
#include <asset/tk_area.h>
#include <asset/tk_entity.h>
#include "elysian_debug.hpp"
#include "tk_assets.h"
#include <tk_level.h>
#include "tk_entity_selectable.h"
#include <QMenu>
#include <QAction>
#include <asset/tk_renderable.h>
#include <tk_project.h>

EntityTreeWidget::EntityTreeWidget(QWidget *parent): QTreeWidget(parent) {
    _hiddenItem = new QTreeWidgetItem();
    QBrush brush = QBrush(Qt::gray);
    _hiddenItem->setForeground(0, brush);
    _hiddenItem->setText(0, "_Hidden");
    _hiddenItem->setFlags(Qt::ItemIsEnabled);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_onCustomContextMenuRequested(QPoint)));

	connect(getAssets(), SIGNAL(areaChanged(Area*const)), this, SLOT(OnAreaChange(Area*const)));
    connect(this,   SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(OnEntitySelected(QTreeWidgetItem *, QTreeWidgetItem*)));
	connect(getAssets(), SIGNAL(selectionChanged(const Selectable::TYPE, Selectable *const)), this, SLOT(OnSelectionChanged(const Selectable::TYPE, Selectable *const)));
	connect(getAssets(), SIGNAL(entityAdded(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnAddEntity(Project*const,Level*const,Area*const,Entity*const)));
	connect(getAssets(), SIGNAL(entityRemoved(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnRemoveEntity(Project*const,Level*const,Area*const,Entity*const)));
    connect(getAssets(), SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));
    //onnect(getAssets(), SIGNAL(layerChanged(Assets::LAYER_TYPE)), this, SLOT(onLayerChange(Assets::LAYER_TYPE)));
}

void EntityTreeWidget::OnAreaChange(Area *const area) {
    _curArea = area;
    _hiddenItem->takeChildren();
    if(_showHidden) takeTopLevelItem(indexOfTopLevelItem(_hiddenItem));
    //Debug::Log(Debug::DBG_VERBOSE, "Area change - %s", area->getName().toStdString().c_str());
    while(topLevelItemCount()) takeTopLevelItem(0);
    if(!area) return;
    auto& entities = area->getEntities();
    auto& inactiveEntities = area->getInactiveEntities();
	for(size_t i = 0; i < entities.size(); ++i) {
      //  Debug::Log(Debug::DBG_VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
        /*if(!entities[i]->getParentEntity())*/ _addEntity(entities[i]);
    }
    for(size_t i = 0; i < inactiveEntities.size(); ++i) {
      //  Debug::Log(Debug::DBG_VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
        /*if(!entities[i]->getParentEntity())*/ _addEntity(inactiveEntities[i]);
    }
    if(_showHidden) addTopLevelItem(_hiddenItem);
}
/*
void EntityTreeWidget::onLayerChange(const Assets::LAYER_TYPE layer) {
    if(_ignoreLayerChange) return;

    if(Assets::LAYER_TYPE == Assets::LAYER_ENTITY) {

    } else {
        setCurrentItem(nullptr);
    }
    getAssets()->setCurrentSelection(new EntitySelectable(static_cast<TkEntity*>(current)));

}*/

void EntityTreeWidget::OnAddEntity(Project *const /*project*/, Level *const /*level*/, Area *const area, Entity *const entity) {
   if(area == getCurrentArea()) {
       _addEntity(entity);
   }
}

void EntityTreeWidget::OnRemoveEntity(Project *const /*project*/, Level *const /*level*/, Area *const area, Entity *const entity) {
    if(area == getCurrentArea()) _removeEntity(entity);//takeTopLevelItem(indexOfTopLevelItem(entity));
}

void EntityTreeWidget::OnEntitySelected(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    if(current == previous) return;
    if(current == _hiddenItem) {
        //xgetAssets()->setCurrentSelection(nullptr);
    } else {
        getAssets()->setCurrentSelection(new EntitySelectable(static_cast<TkEntity*>(current)));
    }
}
void EntityTreeWidget::OnSelectionChanged(const Selectable::TYPE type, Selectable *const selection) {
	EntitySelectable* entities;

	switch(type) {
	default:
		setCurrentItem(nullptr);
		break;
	case Selectable::ENTITY:
		entities = static_cast<EntitySelectable*>(selection);
        const bool blocked = blockSignals(true);
        for(auto it = entities->begin(); it != entities->end(); ++it) {
			setCurrentItem(static_cast<TkEntity*>(*it));
		}
        blockSignals(blocked);
        //_ignoreLayerChange = true;
        if(getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY) Q_EMIT entityLayerActivate();
        //ignoreLayerChange = false;
		break;
	}
}

void EntityTreeWidget::onEntityParented(Entity * const child, Entity * const parent) {
    if(static_cast<QTreeWidgetItem*>(child)->parent()) {
        static_cast<QTreeWidgetItem*>(child)->parent()->removeChild(child);
    }

    if(parent) {
        parent->addChild(child);
    }
}

void EntityTreeWidget::_removeEntity(Entity* entity) {
     char first = entity->getName()[0];

     if(first == '_') {
         QTreeWidgetItem* parent = static_cast<QTreeWidgetItem*>(entity)->parent();
         if(parent) parent->removeChild(entity);
     } else {
        takeTopLevelItem(indexOfTopLevelItem(entity));
     }
}

void EntityTreeWidget::_addEntity(Entity *entity) {
    char first = entity->getName()[0];

    if(first == '_') {
        QBrush brush = QBrush(Qt::gray);
        //entity->setForeground(0, brush);
        _hiddenItem->addChild(entity);
    } else {
        addTopLevelItem((QTreeWidgetItem *)entity);
    }
}

void EntityTreeWidget::setShowHidden(const bool val) {
    _showHidden = val;
    if(val) {
        if(_curArea) addTopLevelItem(_hiddenItem);
    } else {
        if(_curArea) {
            //QList<QTreeWidgetItem*> children = _hiddenItem->takeChildren();
            takeTopLevelItem(indexOfTopLevelItem(_hiddenItem));
            //_hiddenItem->addChildren(children);
        }
    }
}




void EntityTreeWidget::_onCustomContextMenuRequested(QPoint location) {
    QModelIndex modelIndex = indexAt(location);
    Entity *entity = static_cast<Entity*>(itemFromIndex(modelIndex));

    if(!entity || entity == _hiddenItem) return;

    auto* menu = entity->createContextMenu();
    menu->exec(mapToGlobal(location));

    delete menu;

}

