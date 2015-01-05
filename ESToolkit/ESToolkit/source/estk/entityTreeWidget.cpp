#include "entityTreeWidget.h"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "assets.h"
#include "entitySelectable.h"

EntityTreeWidget::EntityTreeWidget(QWidget *parent): QTreeWidget(parent) {
	connect(getAssets(), SIGNAL(areaChanged(Area*const)), this, SLOT(OnAreaChange(Area*const)));
    connect(this,   SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(OnEntitySelected(QTreeWidgetItem *, QTreeWidgetItem*)));
	connect(getAssets(), SIGNAL(selectionChanged(const Selectable::TYPE, Selectable *const)), this, SLOT(OnSelectionChanged(const Selectable::TYPE, Selectable *const)));
	connect(getAssets(), SIGNAL(entityAdded(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnAddEntity(Project*const,Level*const,Area*const,Entity*const)));
	connect(getAssets(), SIGNAL(entityRemoved(Project*const,Level*const,Area*const,Entity*const)), this, SLOT(OnRemoveEntity(Project*const,Level*const,Area*const,Entity*const)));
    connect(getAssets(), SIGNAL(entityParented(Entity*const,Entity*const)), this, SLOT(onEntityParented(Entity*const,Entity*const)));
    //onnect(getAssets(), SIGNAL(layerChanged(Assets::LAYER_TYPE)), this, SLOT(onLayerChange(Assets::LAYER_TYPE)));
}

void EntityTreeWidget::OnAreaChange(Area *const area) {
	//Debug::Log(Debug::VERBOSE, "Area change - %s", area->getName().toStdString().c_str());
    while(topLevelItemCount()) takeTopLevelItem(0);
    if(!area) return;
    auto& entities = area->getEntities();
	for(size_t i = 0; i < entities.size(); ++i) {
	  //  Debug::Log(Debug::VERBOSE, "Trying to add - %s", area->entity[i]->getName().toStdString().c_str());
		addTopLevelItem((QTreeWidgetItem *)entities[i]);
    }
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
    if(area == getCurrentArea()) addTopLevelItem((QTreeWidgetItem *)entity);
}

void EntityTreeWidget::OnRemoveEntity(Project *const /*project*/, Level *const /*level*/, Area *const area, Entity *const entity) {
    if(area == getCurrentArea()) takeTopLevelItem(indexOfTopLevelItem(entity));
}

void EntityTreeWidget::OnEntitySelected(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    if(current == previous) return;
	getAssets()->setCurrentSelection(new EntitySelectable(static_cast<TkEntity*>(current)));
  //  Debug::Log(Debug::VERBOSE, "Trying to add fucking - %s", ((Entity *)current)->getName().toStdString().c_str());
 //DO IN MAIN WINDOW   setWidget((QWidget *)((Entity *)current));
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
        if(getAssets()->getCurrentLayer() != Assets::LAYER_ENTITY) emit entityLayerActivate();
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
