#ifndef ELYSIAN_BEHAVIOR_SYSTEM_HPP
#define ELYSIAN_BEHAVIOR_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_behavior.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")

namespace elysian {

	class BehaviorSystem: public System, public CollisionContactEventMonitor {
		
	public:

		virtual ~BehaviorSystem(void) = default;

		void initialize(void);

		//reimplemented from CollisionContactEventMonitor
		virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;
		
		//Uniform events
		void callTrigger(Entity*const entity, const Behavior::TRIGGER trigger);
		void callTriggers(const Behavior::TRIGGER trigger);

		//Subsystem-specific events
		//void onCollide(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
		//void onCollisionEnter(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
		//void onCollisionExit(Entity *const entity, Collidable *const collidable);
		void onExamine(Entity *const entity1, Entity *const entity2);
		void onDialog(Entity *const entity, const unsigned screen, const int option);
		void onCombatReceiveStatusModifier(Entity*const entity, CombatStatus*const modifier);
		void onCombatTargetCollisionEnter(Entity*const entity, CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier);
        void onShatter(Entity*const entity, CollisionContact*const contact);

		
	};
	
	
	//====INLINEZ====

	inline void BehaviorSystem::onCollide(Collidable *const bodyA, Collidable*const bodyB, CollisionContact*const contact) {
		auto& behaviors = bodyA->getEntity()->getBehaviors();
		for(auto it = behaviors.begin();
			it != behaviors.end();
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_COLLIDE) it->second->onCollide(contact, bodyB);
		}
	}

	inline void BehaviorSystem::onCollisionEnter(Collidable *const bodyA, Collidable*const bodyB, CollisionContact*const contact) {
		auto& behaviors = bodyA->getEntity()->getBehaviors();
		for(auto it = behaviors.begin();
			it != behaviors.end();
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_COLLISION_ENTER) it->second->onCollisionEnter(contact, bodyB);
		}
	}

	inline void BehaviorSystem::onCollisionExit(Collidable*const bodyA, Collidable *const bodyB) {
		auto& behaviors = bodyA->getEntity()->getBehaviors();
		for(auto it = behaviors.begin();
			it != behaviors.end();
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_COLLISION_EXIT) it->second->onCollisionExit(bodyB);
		}
	}

	inline void BehaviorSystem::onExamine(Entity *const entity, Entity *const entity2) {
		auto& behaviors = entity->getBehaviors();
		for(auto it = behaviors.begin();
			it != behaviors.end();
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_EXAMINE) it->second->onExamine(entity2);
		}
	}

	inline void BehaviorSystem::onDialog(Entity *const entity, const unsigned screen, const int option) {
		auto& behaviors = entity->getBehaviors();
		for(auto it = behaviors.begin();
			it != behaviors.end();
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_DIALOG) it->second->onDialog(screen, option);
		}
	}

	inline void BehaviorSystem::callTrigger(Entity * const entity, const Behavior::TRIGGER trigger) {
		auto end = entity->getBehaviors().end();
		for(auto it = entity->getBehaviors().begin();
			it != end;
			++it)
		{
			if(it->second->getTriggers() & trigger) {
				it->second->callTrigger(trigger);
			}
		}
	}

	inline void BehaviorSystem::callTriggers(const Behavior::TRIGGER trigger) {
		auto end = _curArea->behaviors.end();
		for(auto it = _curArea->behaviors.begin();
			it != end; 
			++it) 
		{
			if((*it)->getTriggers() & trigger) {
				(*it)->callTrigger(trigger);
			}
		}
	}

	inline void BehaviorSystem::onCombatReceiveStatusModifier(Entity*const entity, CombatStatus*const modifier) {
		auto end = entity->getBehaviors().end();
		for(auto it = entity->getBehaviors().begin();
			it != end;
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER) {
				it->second->onCombatReceiveStatusModifier(modifier);
			}
		}
	}

    inline void BehaviorSystem::onShatter(Entity*const entity, CollisionContact* contact) {
        auto end = entity->getBehaviors().end();
        for(auto it = entity->getBehaviors().begin();
            it != end;
            ++it)
        {
            if(it->second->getTriggers() & Behavior::ON_SHATTER) {
                it->second->onShatter(contact);
            }
        }
    }

	inline void BehaviorSystem::onCombatTargetCollisionEnter(Entity*const entity, CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) {
		auto end = entity->getBehaviors().end();
		for(auto it = entity->getBehaviors().begin();
			it != end;
			++it)
		{
			if(it->second->getTriggers() & Behavior::ON_COMBAT_TARGET_COLLISION_ENTER) {
				it->second->onCombatTargetCollisionEnter(contact, collidable, modifier);
			}
		}
	}
	
}

#endif
