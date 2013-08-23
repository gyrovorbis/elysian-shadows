#include "elysian_friendly.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

Friendly::Friendly(void): Component() {}
Friendly::Friendly(Entity &parent): Component(parent) {}
Friendly::~Friendly(void) {}

bool Friendly::canAddTo(Entity &parent) {
	//eventually probably should add RIGIDBODY
    return (parent.getCollidable() && parent.getAnimation() && parent.getCharacter());
}
bool Friendly::load(std::ifstream &/*stream*/) {
        dbgprintf(LOG_VERBOSE, "Friendly::Load(ifstream&)");
	return true;
}

bool Friendly::save(std::ofstream &/*stream*/) {
        dbgprintf(LOG_VERBOSE, "Friendly::Save(ofstream&)");
	return true;
}

bool Friendly::addToLevel(void) {
        _curArea->friendlies.push_back(this);
	return true;
}

bool Friendly::removeFromLevel(void) {
    for(unsigned int i = 0; i < _curArea->friendlies.size(); ++i) {
		if(_curArea->friendlies[i] == this) {
			_curArea->friendlies.erase(_curArea->friendlies.begin() + i);
			return true;
		}
	}
	return false;
}

}
