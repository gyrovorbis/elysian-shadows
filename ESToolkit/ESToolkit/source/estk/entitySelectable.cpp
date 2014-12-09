#include "entitySelectable.h"
#include "entity.h"

EntitySelectable::EntitySelectable(TkEntity*const entity): RowsJoinerProxy(nullptr) {
	addEntity(entity);
}

auto EntitySelectable::getType(void) const -> TYPE { return ENTITY; }

QVector<TkEntity*>& EntitySelectable::getEntities(void) {
	return _entities;
}

void EntitySelectable::addEntity(TkEntity * const entity) {
	_entities.push_back(entity);
	insertSourceModel(entity);
}

void EntitySelectable::removeEntity(TkEntity * const entity) {
	for(auto it = _entities.begin(); it != _entities.end(); ++it) {
		if(*it == entity) {
			_entities.erase(it);
			break;
		}
	}
	removeSourceModel(entity);
}

void EntitySelectable::translate(const float x, const float y) {
    for(auto& it : _entities) it->getTransform().translate(x, y);

}
