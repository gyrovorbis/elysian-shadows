#ifndef ELYSIAN_BEHAVIOR_SYSTEM_HPP
#define ELYSIAN_BEHAVIOR_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_behavior.hpp"
#include "elysian_entity.hpp"
#include "elysian_collision_system.hpp"

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
		void callTriggers(const Behavior::TRIGGER trigger);

		//Subsystem-specific events
		//void onCollide(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
		//void onCollisionEnter(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
		//void onCollisionExit(Entity *const entity, Collidable *const collidable);
		void onExamine(Entity *const entity1, Entity *const entity2);
		void onDialog(Entity *const entity, const unsigned screen, const int option);
		
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
	
}

#endif
