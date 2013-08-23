#ifndef ELYSIAN_BEHAVIOR_HPP
#define ELYSIAN_BEHAVIOR_HPP

#include <map>
#include <stdint.h>
#include <cassert>
#include "elysian_component.hpp"
#include "elysian_stl_algorithm.hpp"

#define ELYSIAN_BEHAVIOR_NAME_SIZE 100

namespace elysian {
	
	struct ltstr;
	class Entity;
	class Collidable;
	
	class Behavior: public Component {
		friend class Entity;
	private:
		static const Component::TYPE _type = Component::BEHAVIOR;
		static std::map<const char*, Behavior* (*)(Entity&), ltstr> _creators;
		bool _active;
        
	protected:
		static void registerCreator(const char*typeName, Behavior* (*const func)(Entity&));
		
		Behavior(Entity& parent);
		virtual ~Behavior(void);
		
		uint64_t _triggerBits;
		
	public:
		//64 triggers max, currently
		enum TRIGGER {
			ON_CREATE			= 1,
			ON_DESTROY			= 1<<1,
			ON_LOAD				= 1<<2,
			ON_UNLOAD			= 1<<3,
			ON_ENTER			= 1<<4,
			ON_EXIT				= 1<<5,
			ON_UPDATE			= 1<<6,
			ON_RENDER			= 1<<7,
			ON_COLLIDE			= 1<<8,
			ON_COLLISION_ENTER	= 1<<9,
			ON_COLLISION_EXIT	= 1<<10,
			ON_EXAMINE			= 1<<11,
			ON_DIALOG			= 1<<12,
			TRIGGER_MAX			= 1<<13
		};

		virtual const char *getBehavName(void) const = 0;
		
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
		virtual void onCollide(Collidable* const collidable);
		virtual void onCollisionEnter(Collidable* const collidable);
		virtual void onCollisionExit(Collidable* const collidable);
		virtual void onExamine(Entity* entity);
		virtual void onDialog(const unsigned screen, const int option);

		void		callTrigger(const TRIGGER trigger);

		uint64_t	getTriggers(void) const;
		void		addTrigger(const TRIGGER flag);
		void		removeTrigger(const TRIGGER flag);
		inline bool getActive(void) const { return _active; }
		inline void setActive(const bool val=true) { _active = val; }
		
		//Reimplemented from Component
        virtual bool canAddTo(Entity& entity) override;
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override; 
		virtual Component::TYPE getType(void) const override;
	};
	
	
	//========= INLINEZ =========
	
	inline Behavior::Behavior(Entity& parent): Component(parent), _triggerBits(0), _active(false) {}
	inline Behavior::~Behavior(void) { }
	inline uint64_t Behavior::getTriggers(void) const { return _triggerBits; }
	//inline void Behavior::addTrigger(const uint64_t flags) { _triggerBits |= flags; }
	//inline void Behavior::setTriggers(const uint64_t flags) { _triggerBits = flags; }
	inline Component::TYPE Behavior::getType(void) const { return Component::BEHAVIOR; }
	inline bool Behavior::canAddTo(Entity& /*entity*/) { return true; /*for each bit, make sure entity has required components*/ }
	
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
			default: //these have no business being called here
				assert(false);
				return;
		}
	}


}


#endif
