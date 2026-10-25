/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_BEHAVIOR_SYSTEM_HPP
#define ELYSIAN_BEHAVIOR_SYSTEM_HPP

#include <functional>
#include <ESGamma/system/elysian_system.hpp>
#include <AssetIO/crtp/elysian_behavior.hpp>
#include <ESGamma/system/elysian_collision_system.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_area.hpp>

namespace elysian {

	class BehaviorSystem: public System, public CollisionContactEventMonitor {

    protected:

        //Can operate on single entity or defaults to all behaviors in current area
        void            _foreachBehavior(const uint32_t triggers, const std::function<void(Behavior*const)>& lambda, Entity*const entity=nullptr) const;
		
	public:

        virtual         ~BehaviorSystem(void) = default;

        void            initialize(void);

		//reimplemented from CollisionContactEventMonitor
        virtual void    onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
        virtual void    onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
        virtual void    onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;
		
		//Uniform events
        void            callTrigger(Entity*const entity, const Behavior::TRIGGER trigger) const;
        void            callTriggers(const Behavior::TRIGGER trigger) const;

		//Subsystem-specific events
        //void          onCollide(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
        //void          onCollisionEnter(Entity *const entity, CollisionContact*const contact, Collidable *const collidable);
        //void          onCollisionExit(Entity *const entity, Collidable *const collidable);
        void            onExamine(Entity *const entity1, Entity *const entity2) const;
        void            onDialog(Entity *const entity, const unsigned screen, const int option) const;
        void            onCombatReceiveStatusModifier(Entity*const entity, CombatStatus*const modifier) const;
        void            onCombatTargetCollisionEnter(Entity*const entity, CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) const;
        void            onShatter(Entity*const entity, CollisionContact*const contact) const;
        //void          onButtonPress(const GameInput::BUTTON button) const;
        //void          onButtonRelease(const GameInput::BUTTON button) const;

		
	};
	




	
	//====INLINEZ====



    inline void BehaviorSystem::onCollide(Collidable *const bodyA, Collidable*const bodyB, CollisionContact*const contact) {
        _foreachBehavior(Behavior::ON_COLLIDE, [&](Behavior*const behav) {
            behav->onCollide(contact, bodyB);
        }, bodyA->getEntity());
	}

    inline void BehaviorSystem::onCollisionEnter(Collidable *const bodyA, Collidable*const bodyB, CollisionContact*const contact) {
        _foreachBehavior(Behavior::ON_COLLISION_ENTER, [&](Behavior*const behav) {
            behav->onCollide(contact, bodyB);
        }, bodyA->getEntity());
	}

    inline void BehaviorSystem::onCollisionExit(Collidable*const bodyA, Collidable *const bodyB) {
        _foreachBehavior(Behavior::ON_COLLISION_EXIT, [&](Behavior*const behav) {
            behav->onCollisionExit(bodyB);
        }, bodyA->getEntity());
	}

    inline void BehaviorSystem::onExamine(Entity *const entity, Entity *const entity2) const {
        _foreachBehavior(Behavior::ON_EXAMINE, [&](Behavior*const behav) {
            behav->onExamine(entity2);
        }, entity);
	}

    inline void BehaviorSystem::onDialog(Entity *const entity, const unsigned screen, const int option) const {
        _foreachBehavior(Behavior::ON_DIALOG, [&](Behavior*const behav) {
            behav->onDialog(screen, option);
        }, entity);
	}

    inline void BehaviorSystem::callTrigger(Entity * const entity, const Behavior::TRIGGER trigger) const {
        _foreachBehavior(trigger, [&](Behavior*const behav) {
            behav->callTrigger(trigger);
        }, entity);
	}

    inline void BehaviorSystem::callTriggers(const Behavior::TRIGGER trigger) const {
        _foreachBehavior(trigger, [&](Behavior*const behav) {
            behav->callTrigger(trigger);
        });
	}

    inline void BehaviorSystem::onCombatReceiveStatusModifier(Entity*const entity, CombatStatus*const modifier) const {
        _foreachBehavior(Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER, [&](Behavior*const behav) {
            behav->onCombatReceiveStatusModifier(modifier);
        }, entity);
	}

    inline void BehaviorSystem::onShatter(Entity*const entity, CollisionContact* contact) const {
        _foreachBehavior(Behavior::ON_SHATTER, [&](Behavior*const behav) {
            behav->onShatter(contact);
        }, entity);
    }

    /*inline void BehaviorSystem::onButtonPress(const GameInput::BUTTON button) const {
        _foreachBehavior(Behavior::ON_BUTTON_PRESS, [&](Behavior*const behav) {
            behav->onButtonPress(button);
        });
    }*/

    /*
    inline void BehaviorSystem::onButtonRelease(const GameInput::BUTTON button) const {
        _foreachBehavior(Behavior::ON_BUTTON_RELEASE, [&](Behavior*const behav) {
            behav->onButtonRelease(button);
        });
    }*/

    inline void BehaviorSystem::onCombatTargetCollisionEnter(Entity*const entity, CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) const {
        _foreachBehavior(Behavior::ON_COMBAT_TARGET_COLLISION_ENTER, [&](Behavior*const behav) {
            behav->onCombatTargetCollisionEnter(contact, collidable, modifier);
        }, entity);
	}
	
}

#endif
