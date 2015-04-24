#include "entityTreeWidget.h"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "assets.h"
#include <TkLevel.h>
#include "entitySelectable.h"
#include <QMenu>
#include <QAction>
#include <elysian_renderable_base.hpp>
#include <project.h>

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

    connect(getAssets(), &Assets::inactiveEntityAdded, [&](Project*const, Level*const, Area*const area, Entity*const entity) {
        if(area == getCurrentArea()) {
            QBrush brush = QBrush(Qt::gray);
            entity->setForeground(0, brush);
            for(int i = 0; i < entity->childCount(); ++i) {
                static_cast<Entity*>(entity->child(0))->setForeground(0, brush);
            }
            _addEntity(entity);
        }
    });

    connect(getAssets(), &Assets::inactiveEntityRemoved, [&](Project*const, Level*const, Area*const area, Entity*const entity) {
        if(area == getCurrentArea()) {
            _removeEntity(entity);
        }
    });
}

void EntityTreeWidget::OnAreaChange(Area *const area) {
    _curArea = area;
    _hiddenItem->takeChildren();
    if(_showHidden) takeTopLevelItem(indexOfTopLevelItem(_hiddenItem));
	//Debug::Log(Debug::VERBOSE, "Area change - %s", area->getName().toStdString().c_str());
    while(topLevelItemCount()) takeTopLevelItem(0);
    if(!area) return;
    auto& entities = area->getEntities();
	for(size_t i = 0; i < entities.size(); ++i) {
	  //  Debug::Log(Debug::VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
        /*if(!entities[i]->getParentEntity())*/ _addEntity(entities[i]);
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
       QBrush brush = QBrush(Qt::black);
       entity->setForeground(0, brush);
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
		for(auto it = entities->getEntities().begin(); it != entities->getEntities().end(); ++it) {
			setCurrentItem(static_cast<TkEntity*>(*it));
		}
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

