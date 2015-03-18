#ifndef ELYSIAN_BEHAVIOR_HPP
#define ELYSIAN_BEHAVIOR_HPP

#include <map>
#include <stdint.h>
#include <cassert>
//#include "elysian_lua_table_proxy.hpp"
#include "elysian_component.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_game_input.hpp"

#define ELYSIAN_BEHAVIOR_NAME_SIZE 100

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {
	
	struct ltstr;
	class Collidable;
	class CollisionContact;
	class CombatStatus;
	
	class Behavior: public Component {
        friend class BaseEntity;
	private:
		static const Component::TYPE _type = Component::BEHAVIOR;
		static std::map<const char*, Behavior* (*)(Entity&), ltstr> _creators;
		bool _active = false;
        
	protected:
		static void registerCreator(const char*typeName, Behavior* (*const func)(Entity&));
		
		Behavior(Entity& parent);
		
		uint64_t _triggerBits = 0;
		
	public:

       /* ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(triggers, getTriggers)
        ELYSIAN_LUA_PROXY_FIELDS_END()*/

        virtual ~Behavior(void) override = default;

        enum TRIGGER_INDEX {
            ON_SCRIPT_CREATE_INDEX,
            ON_SCRIPT_DESTROY_INDEX,
            ON_CREATE_INDEX,
            ON_DESTROY_INDEX,
            ON_LOAD_INDEX,
            ON_UNLOAD_INDEX,
            ON_ENTER_INDEX,
            ON_EXIT_INDEX,
            ON_UPDATE_INDEX,
            ON_RENDER_INDEX,
            ON_COLLIDE_INDEX,
            ON_COLLISION_ENTER_INDEX,
            ON_COLLISION_EXIT_INDEX,
            ON_EXAMINE_INDEX,
            ON_DIALOG_INDEX,
            ON_COMBAT_ENTER_INDEX,
            ON_COMBAT_EXIT_INDEX,
            ON_COMBAT_DEATH_INDEX,
            ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX,
            ON_COMBAT_TARGET_COLLISION_ENTER_INDEX,
            ON_SHATTER_INDEX,
            ON_BUTTON_PRESS_INDEX,
            ON_BUTTON_RELEASE_INDEX,
            TRIGGER_MAX_INDEX,
            //ESTk-only trigers
            ON_TK_SELECT_INDEX=62,
            ON_TK_UNSELECT_INDEX=63
        };

		//64 triggers max, currently
        enum TRIGGER {
            ON_SCRIPT_CREATE					= 1ul<<ON_SCRIPT_CREATE_INDEX,
            ON_SCRIPT_DESTROY					= 1ul<<ON_SCRIPT_DESTROY_INDEX,
            ON_CREATE							= 1ul<<ON_CREATE_INDEX,
            ON_DESTROY							= 1ul<<ON_DESTROY_INDEX,
            ON_LOAD								= 1ul<<ON_LOAD_INDEX,
            ON_UNLOAD							= 1ul<<ON_UNLOAD_INDEX,
            ON_ENTER							= 1ul<<ON_ENTER_INDEX,
            ON_EXIT								= 1ul<<ON_EXIT_INDEX,
            ON_UPDATE							= 1ul<<ON_UPDATE_INDEX,
            ON_RENDER							= 1ul<<ON_RENDER_INDEX,
            ON_COLLIDE							= 1ul<<ON_COLLIDE_INDEX,
            ON_COLLISION_ENTER					= 1ul<<ON_COLLISION_ENTER_INDEX,
            ON_COLLISION_EXIT					= 1ul<<ON_COLLISION_EXIT_INDEX,
            ON_EXAMINE							= 1ul<<ON_EXAMINE_INDEX,
            ON_DIALOG							= 1ul<<ON_DIALOG_INDEX,
            ON_COMBAT_ENTER						= 1ul<<ON_COMBAT_ENTER_INDEX,
            ON_COMBAT_EXIT						= 1ul<<ON_COMBAT_EXIT_INDEX,
            ON_COMBAT_DEATH						= 1ul<<ON_COMBAT_DEATH_INDEX,
            ON_COMBAT_RECEIVE_STATUS_MODIFIER	= 1ul<<ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX,
            ON_COMBAT_TARGET_COLLISION_ENTER	= 1ul<<ON_COMBAT_TARGET_COLLISION_ENTER_INDEX,
            ON_SHATTER                          = 1ul<<ON_SHATTER_INDEX,
            ON_BUTTON_PRESS                     = 1ul<<ON_BUTTON_PRESS_INDEX,
            ON_BUTTON_RELEASE                   = 1ul<<ON_BUTTON_RELEASE_INDEX,
            TRIGGER_MAX							= 1ul<<TRIGGER_MAX_INDEX,//1<<23,
            //ESTk-only trigers
            ON_TK_SELECT                        = 1ull<<ON_TK_SELECT_INDEX,
            ON_TK_UNSELECT                      = 1ull<<ON_TK_UNSELECT_INDEX
        };

        constexpr static const char* triggerStrings[] = {
            "onScriptCreate",
            "onScriptDestroy",
            "onCreate",
            "onDestroy",
            "onLoad",
            "onUnload",
            "onEnter",
            "onExit",
            "onUpdate",
            "onRender",
            "onCollide",
            "onCollisionEnter",
            "onCollisionExit",
            "onExamine",
            "onDialog",
            "onCombatEnter",
            "onCombatExit",
            "onCombatDeath",
            "onCombatReceiveStatusModifier",
            "onCombatTargetCollisionEnter",
            "onShatter",
            "onButtonPress",
            "onButtonRelease"
        };

		static const char* triggerNameFromFlag(const uint64_t flag);

        virtual const char* getBehavName(void) const = 0;
		
		static Behavior* create(const char* typeName, Entity& parent);
        
		static void staticConstructor(void);
		
		//Default trigger implementations (should never be called, since anything with the trigger should override)
		virtual void onCreate(void);
        virtual void onDestroy(void);
		virtual void onLoad(void);
		virtual void onUnload(void);
		virtual void onEnter(void);
		virtual void onExit(void);
        virtual void onUpdate(void);
        virtual void onRender(void);
		virtual void onCollide(CollisionContact*const contact, Collidable* const collidable);
		virtual void onCollisionEnter(CollisionContact*const contact, Collidable* const collidable);
        virtual void onShatter(CollisionContact*const contact);
		virtual void onCollisionExit(Collidable* const collidable);
		virtual void onExamine(Entity* entity);
		virtual void onDialog(const unsigned screen, const int option);
		virtual void onCombatEnter(void);
		virtual void onCombatExit(void);
		virtual void onCombatDeath(void);
        virtual void onButtonPress(const GameInput::BUTTON button);
        virtual void onButtonRelease(const GameInput::BUTTON button);
		virtual void onCombatReceiveStatusModifier(CombatStatus*const modifier);
		virtual void onCombatTargetCollisionEnter(CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier);

        //ESTk Triggers
        virtual void onTkSelect(void);
        virtual void onTkUnselect(void);

		void		callTrigger(const TRIGGER trigger);

		uint64_t	getTriggers(void) const;
		void		addTrigger(const TRIGGER flag);
		void		addTriggers(const uint64_t flags);
		void		removeTrigger(const TRIGGER flag);
		inline bool getActive(void) const { return _active; }
		inline void setActive(const bool val=true) { _active = val; }
		
		//Reimplemented from Component
        virtual bool canAddTo(Entity& entity) const override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override; 
		virtual Component::TYPE getType(void) const override;
	};
	
	
	//========= INLINEZ =========
	
	inline uint64_t Behavior::getTriggers(void) const { return _triggerBits; }
	//inline void Behavior::addTrigger(const uint64_t flags) { _triggerBits |= flags; }
	//inline void Behavior::setTriggers(const uint64_t flags) { _triggerBits = flags; }
	inline Component::TYPE Behavior::getType(void) const { return Component::BEHAVIOR; }
    inline bool Behavior::canAddTo(Entity& /*entity*/) const { return true; /*for each bit, make sure entity has required components*/ }

	inline void Behavior::addTriggers(const uint64_t flags) {
		for(unsigned i = 0; i < 64; ++i) {
			addTrigger((TRIGGER)(flags & (0x1<<i)));
		}
	}
	
	inline void Behavior::callTrigger(const TRIGGER trigger) {
		if(!_triggerBits & trigger) return;

		switch(trigger) {
			case ON_LOAD:
				return onLoad();
			case ON_UNLOAD:
				return onUnload();
			case ON_ENTER:
				return onEnter();
			case ON_EXIT:
				return onExit();
			case ON_UPDATE:
				return onUpdate();
			case ON_RENDER:
				return onRender();
			case ON_COMBAT_ENTER:
				return onCombatEnter();
			case ON_COMBAT_EXIT:
				return onCombatExit();
			case ON_COMBAT_DEATH:
				return onCombatDeath();
            case ON_TK_SELECT:
                return onTkSelect();
            case ON_TK_UNSELECT:
                return onTkUnselect();
			default: //these have no business being called here
				assert(false);
				return;
		}
	}


}


#endif
