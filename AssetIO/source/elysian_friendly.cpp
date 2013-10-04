#include "elysian_friendly.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

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
    return containerEraseOnMatch(this, _curArea->friendlies);
}

}
