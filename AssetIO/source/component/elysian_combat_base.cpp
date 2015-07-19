#include <crtp/elysian_combat.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_area.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CombatBase)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CombatAttrib)

CombatBase::CombatBase(const CombatBase& other) {
    *this = other;
}

void CombatBase::setDmgType(const DMG_TYPE type) {
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
