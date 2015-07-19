#include "tk_entity_selectable.h"
#include <asset/tk_entity.h>
#include "elysian_engine.hpp"
#include "elysian_behavior_system.hpp"
#include "tk_lua_table_stack_model.h"

EntitySelectable::EntitySelectable(TkEntity*const entity): RowsJoinerProxy(nullptr) {
	addEntity(entity);
}

EntitySelectable::EntitySelectable(EntitySelectable *selectable): RowsJoinerProxy(nullptr) {
    for(auto it : selectable->_entities) {
        EntityEntry entry;
        entry.wasActive = it.wasActive;
        entry.entity = new TkEntity(it.entity);
        addEntity(entry);
    }
}

QAbstractItemModel* EntitySelectable::getModel(void) const { return const_cast<EntitySelectable*>(this); }

auto EntitySelectable::getType(void) const -> TYPE { return ENTITY; }

//const QVector<TkEntity*>& EntitySelectable::getEntities(void) const {
//	return _entities;
//}

void EntitySelectable::addEntity(const EntityEntry entry) {
    if(!entry.entity) return;
    _entities.push_back(entry);
    insertSourceModel(entry->getModel());
    entry->getModel()->setLabelItemText("Entity");
}

void EntitySelectable::removeEntity(TkEntity * const entity) {
	for(auto it = _entities.begin(); it != _entities.end(); ++it) {
		if(*it == entity) {
            if(*it == _baseEntity) _baseEntity = nullptr;
			_entities.erase(it);
            removeSourceModel(entity->getModel());
			break;
		}
	}
}

void EntitySelectable::translate(const float x, const float y) {
    for(auto& it : _entities) it->getTransform().translate(x, y);
}

void EntitySelectable::setPosition(const float x, const float y) {
    const GYVector3 basePos = getBaseEntity()->_transform._pos;

    auto moveEntity = [&](TkEntity* entity) {
        const GYVector3 prevPos = entity->_transform._pos;
        GYVector3 offset;
        gyVec3Sub(&offset, &prevPos, &basePos);
        entity->_transform.setPos(x+offset.x, y+offset.y, prevPos.z);
    };

    getBaseEntity()->_transform._pos = GYVector3 { x, y+basePos.z, basePos.z };
    for(auto entity : _entities)  {

        if(entity != getBaseEntity()) {
            moveEntity(entity);
        }
    }
}


void EntitySelectable::setCurrentSelection(const bool val, const Selectable* const other) {
    QVector<EntityEntry> exclusiveEntities = _entities;


    if(other && other->getType() == Selectable::ENTITY) {
        const EntitySelectable& otherEntities = *static_cast<const EntitySelectable*>(other);
        for(auto it  = otherEntities._entities.begin(); it != otherEntities._entities.end(); ++it) {
            for(auto it2 = exclusiveEntities.begin(); it2 != exclusiveEntities.end(); ++it2) {
                if(*it == *it2) {
                    it2 = exclusiveEntities.erase(it2);
                    break;
                }
            }
        }
        _baseEntity = otherEntities._baseEntity;
    }

    for(auto& it : exclusiveEntities) {
        it->setSelectionState(val? TkEntity::SELECTION_SELECTED : TkEntity::SELECTION_NONE);
        getAssets()->getEngine()->getBehavSys()->callTrigger(it, val? elysian::Behavior::ON_TK_SELECT : elysian::Behavior::ON_TK_UNSELECT);
    }
}

void EntitySelectable::deactivate(void) {
    for(auto& it : _entities) {
        it.wasActive = it->isActive();
        if(it->getArea()) it->getArea()->removeEntity(it);
    }
}

void EntitySelectable::activate(TkArea* area) {
    for(auto& it : _entities) {
        area->addEntity(it);
        if(it.wasActive) it->activate();
        else it->deactivate();
    }

}
