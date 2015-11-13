#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"

namespace elysian {

	void BehaviorSystem::initialize(void) {
		getCollisionSys()->registerContactEventMonitor(ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK, this);
	}

    void BehaviorSystem::_foreachBehavior(const uint64_t triggers, const std::function<void(Behavior*const )>& lambda, Entity*const entity) const {
        std::vector<Behavior*> behaviors = entity? entity->getBehaviors() : _curArea->behaviors;

       /*
        *Iterating over a COPY of the vector on purpose, since any trigger can potentially remove
        * any number of behaviors, invalidating iteration. Don't know if this will be good enough,
        * since you can still technically "remove" something from the list, but then again,
        * trigger invocation order is nondeterministic, so relying on an order here is semantically
        * incorrect anyway, so I'll blame the scripter for being a dipshit...
        */

        for(size_t i = 0; i < behaviors.size(); ++i) {
            Behavior* behav = behaviors[i];
            //FUCKING REESTED, FIX ME!!!
            //if(!behav->getEntity() || !behav->isActive() || !behav->getEntity()->isActive()) continue;
            if(behav->getTriggers() & triggers) {
                lambda(behav);
            }
        }
    }



}

