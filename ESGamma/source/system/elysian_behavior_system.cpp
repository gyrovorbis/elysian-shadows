#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"

namespace elysian {

	void BehaviorSystem::initialize(void) {
		getCollisionSys()->registerContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK, this);
	}

    void BehaviorSystem::_foreachBehavior(const uint64_t triggers, const std::function<void(Behavior*const )>& lambda, Entity*const entity) const {
        const auto& behaviors = entity? entity->getBehaviors() : _curArea->behaviors;

        for(auto behav : behaviors) {
            if(!behav->getEntity()) continue;
            assert(behav->getEntity()->isActive());
            if(behav->getTriggers() & triggers) {
                lambda(behav);
            }
        }
    }



}

