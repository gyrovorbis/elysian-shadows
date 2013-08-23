#include "elysian_item.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

Item::Item(void): Component() {}
Item::Item(Entity& parent): Component(parent) {}
Item::~Item(void) {}

bool Item::canAddTo(Entity &parent) {
	//eventually probably should add RIGIDBODY
	return (parent.getCollidable() && parent.getAnimation());
}

bool Item::load(std::ifstream& /*stream*/) {
    dbgprintf(LOG_VERBOSE, "Item::Load(ifstream&)");
	return true;
}

bool Item::save(std::ofstream& /*stream*/) {
    dbgprintf(LOG_VERBOSE, "Item::Save(ofstream&)");
	return true;
}

bool Item::addToLevel() {
	_curArea->items.push_back(this);
	return true;
}

bool Item::removeFromLevel() {
	for(unsigned int i = 0; i < _curArea->items.size(); ++i) {
		if(_curArea->items[i] == this) {
			_curArea->items.erase(_curArea->items.begin() + i);
			return true;
		}
	}
	return false;
}

}
