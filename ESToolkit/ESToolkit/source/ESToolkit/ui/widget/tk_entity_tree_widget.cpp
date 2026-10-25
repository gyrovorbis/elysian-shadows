/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ui/widget/tk_entity_tree_widget.h>
#include <ESToolkit/AssetIO/tk_area.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESToolkit/core/tk_assets.h>
#include <ESToolkit/AssetIO/tk_level.h>
#include <ESToolkit/core/tk_entity_selectable.h>
#include <QMenu>
#include <QAction>
#include <ESToolkit/AssetIO/tk_renderable.h>
#include <ESToolkit/core/tk_project.h>

EntityTreeWidget::EntityTreeWidget(QWidget *parent): QTreeWidget(parent) {
    _hiddenItem = new QTreeWidgetItem();
    QBrush brush = QBrush(Qt::gray);
    _hiddenItem->setForeground(0, brush);
    _hiddenItem->setText(0, "_Hidden");
    _hiddenItem->setFlags(Qt::ItemIsEnabled);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_onCustomContextMenuRequested(QPoint)));

	connect(getAssets(), SIGNAL(areaChanged(Area*const)), this, SLOT(OnAreaChange(Area*const)));
    //connect(this,   SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(OnEntitySelected(QTreeWidgetItem *, QTreeWidgetItem*)));
    connect(this, SIGNAL(itemSelectionChanged(void)), this, SLOT(_onItemSelectionChanged(void)));
    connect(getAssets(), SIGNAL(selectionChanged(const Selectable::TYPE, Selectable *const)), this, SLOT(OnSelectionChanged(const Selectable::TYPE, Selectable *const)));
	connect(getAssets(), SIGNAL(entityAdded(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnAddEntity(Project*const,Level*const,Area*const,Entity*const)));
	connect(getAssets(), SIGNAL(entityRemoved(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnRemoveEntity(Project*const,Level*const,Area*const,Entity*const)));
    connect(getAssets(), SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(_onItemDoubleClicked(QTreeWidgetItem*,int)));
    //onnect(getAssets(), SIGNAL(layerChanged(Assets::LAYER_TYPE)), this, SLOT(onLayerChange(Assets::LAYER_TYPE)));
}

void EntityTreeWidget::OnAreaChange(Area *const area) {
    _curArea = area;
    _hiddenItem->takeChildren();
    if(_showHidden) takeTopLevelItem(indexOfTopLevelItem(_hiddenItem));
    //Debug::Log(Debug::LOG_VERBOSE, "Area change - %s", area->getName().toStdString().c_str());
    while(topLevelItemCount()) takeTopLevelItem(0);
    if(!area) return;
    auto& entities = area->getEntities();
    auto& inactiveEntities = area->getInactiveEntities();
	for(size_t i = 0; i < entities.size(); ++i) {
      //  Debug::Log(Debug::LOG_VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
        /*if(!entities[i]->getParentEntity())*/ _addEntity(entities[i]);
    }
    for(size_t i = 0; i < inactiveEntities.size(); ++i) {
      //  Debug::Log(Debug::LOG_VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
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
    auto* selModel = selectionModel();
    const bool blocked = blockSignals(true);
    selModel->clear();

	switch(type) {
	default:
        //setCurrentItem(nullptr);

        break;
	case Selectable::ENTITY:
		entities = static_cast<EntitySelectable*>(selection);

        for(auto it = entities->begin(); it != entities->end(); ++it) {
            //setCurrentItem(static_cast<TkEntity*>(*it));
            auto index = indexFromItem(static_cast<QTreeWidgetItem*>(*it));
            selModel->select(index, QItemSelectionModel::Select);
            scrollTo(index);
        }

        //_ignoreLayerChange = true;
        if(getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY) Q_EMIT entityLayerActivate();
        //ignoreLayerChange = false;
		break;
	}

    blockSignals(blocked);
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


void EntityTreeWidget::_onItemSelectionChanged(void) {
    auto selection = selectedItems();
    EntitySelectable* selectable = nullptr;

    if(selection.size()) {

        selectable = new EntitySelectable;

        for(auto it = selection.begin();
            it != selection.end();
            ++it)
        {
            selectable->addEntity(static_cast<Entity*>(*it));
        }

    }

    setCurrentSelection(selectable);
}


void EntityTreeWidget::_onCustomContextMenuRequested(QPoint location) {
    QModelIndex modelIndex = indexAt(location);
    Entity *entity = static_cast<Entity*>(itemFromIndex(modelIndex));

    if(!entity || entity == _hiddenItem) return;

    auto* menu = entity->createContextMenu();
    menu->exec(mapToGlobal(location));

    delete menu;

}

void EntityTreeWidget::_onItemDoubleClicked(QTreeWidgetItem *item, int) {
    TreeNode *node= static_cast<TreeNode*>(item);
    node->onDoubleClick();
}
