#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

namespace elysian {

Combat::Combat(Entity &entity): Component(entity) {}

bool Combat::canAddTo(Entity& /*entity*/) { return true; }
bool Combat::load(std::ifstream& /*stream*/) { return true; }
bool Combat::save(std::ofstream& /*stream*/) { return true; }

bool Combat::addToLevel(void) {
	_curArea->combats.push_back(this);
}

bool Combat::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->combats);

}

auto Combat::getType(void) const -> TYPE { return COMBAT; }


}
