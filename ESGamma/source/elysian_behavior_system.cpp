#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"

namespace elysian {

	void BehaviorSystem::initialize(void) {
		getCollisionSys()->registerContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK, this);
	}



}

