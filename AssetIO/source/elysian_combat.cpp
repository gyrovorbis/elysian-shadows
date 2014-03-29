#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"

namespace elysian {

Combat::Combat(Entity &entity): Component(entity) {}

bool Combat::canAddTo(Entity& entity) { return entity.getCollidable(); }
bool Combat::load(std::ifstream& /*stream*/) { return true; }
bool Combat::save(std::ofstream& /*stream*/) { return true; }

bool Combat::addToLevel(void) {
	_curArea->combats.push_back(this);
}

bool Combat::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->combats);

}

auto Combat::getType(void) const -> TYPE { return COMBAT; }

void Combat::setDmgType(const DMG_TYPE type) {
	_dmgType = type;

	uint32_t mask = getEntity()->getCollidable()->getContactEventMonitorMask();

	switch(type) {
	case DMG_SOURCE:
	case DMG_SOURCE_TARGET:
		mask |= ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK;
		break;
	case DMG_TARGET:
	case DMG_NONE:
	default:
		mask &= ~ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK;
	}

	getEntity()->getCollidable()->setContactEventMonitorMask(mask);

}


}
