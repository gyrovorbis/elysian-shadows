#include "tk_entity_selectable.h"
#include "elysian_entity.hpp"
#include "elysian_engine.hpp"
#include "elysian_behavior_system.hpp"
#include "tk_lua_table_stack_model.h"

EntitySelectable::EntitySelectable(TkEntity*const entity): RowsJoinerProxy(nullptr) {
	addEntity(entity);
}

QAbstractItemModel* EntitySelectable::getModel(void) const { return const_cast<EntitySelectable*>(this); }

auto EntitySelectable::getType(void) const -> TYPE { return ENTITY; }

const QVector<TkEntity*>& EntitySelectable::getEntities(void) const {
	return _entities;
}

void EntitySelectable::addEntity(TkEntity * const entity) {
    if(!entity) return;
	_entities.push_back(entity);
    insertSourceModel(entity->getModel());
    entity->getModel()->setLabelItemText("Entity");
}

void EntitySelectable::removeEntity(TkEntity * const entity) {
	for(auto it = _entities.begin(); it != _entities.end(); ++it) {
		if(*it == entity) {
			_entities.erase(it);
            removeSourceModel(entity->getModel());
			break;
		}
	}
}

void EntitySelectable::translate(const float x, const float y) {
    for(auto& it : _entities) it->getTransform().translate(x, y);
}

void EntitySelectable::setCurrentSelection(const bool val, const Selectable* const other) {
    QVector<TkEntity*> exclusiveEntities = _entities;


    if(other && other->getType() == Selectable::ENTITY) {
        const EntitySelectable& otherEntities = *static_cast<const EntitySelectable*>(other);
        for(auto it  = otherEntities.getEntities().begin(); it != otherEntities.getEntities().end(); ++it) {
            for(auto it2 = exclusiveEntities.begin(); it2 != exclusiveEntities.end(); ++it2) {
                if(*it == *it2) {
                    it2 = exclusiveEntities.erase(it2);
                    break;
                }
            }
        }
    }

    for(auto& it : exclusiveEntities) {
        it->setSelectionState(val? TkEntity::SELECTION_SELECTED : TkEntity::SELECTION_NONE);
        getAssets()->getEngine()->getBehavSys()->callTrigger(it, val? elysian::Behavior::ON_TK_SELECT : elysian::Behavior::ON_TK_UNSELECT);
    }
}
