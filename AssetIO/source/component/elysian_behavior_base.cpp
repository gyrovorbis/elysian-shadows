#include "elysian_behavior.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_collidable.hpp"
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

constexpr const char* BehaviorBase::triggerStrings[];

//ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Behavior)

//----STATIC----

std::map<const char *, Behavior* (*)(Entity&), ltstr> BehaviorBase::_creators;

void BehaviorBase::registerCreator(const char *const typeName, Behavior* (*const func)(Entity&)) {
	_creators[typeName] = func;
}

void BehaviorBase::staticConstructor(void) {
	//REGISTER C++ BEHAVIORS HERE
}
		
Behavior* BehaviorBase::create(const char* typeName, Entity& parent) {
	std::map<const char *, Behavior* (*)(Entity&), ltstr>::iterator it = _creators.find(typeName);
	if(it == _creators.end()) {
        Debug::printf(Debug::CRITICAL, "Behavior::create(%s) - No registered creator for %s.", typeName);
        return nullptr;
	}
	return _creators[typeName](parent);
}

const char* BehaviorBase::triggerNameFromFlag(const uint64_t flag) {
	static char buff[20];

	switch(flag) {
	case ON_SCRIPT_CREATE:
		return "ON_SCRIPT_CREATE";
	case ON_SCRIPT_DESTROY:
		return "ON_SCRIPT_DESTROY";
	case ON_CREATE:
		return "ON_CREATE";
	case ON_DESTROY:
		return "ON_DESTROY";
	case ON_LOAD:
		return "ON_LOAD";
	case ON_UNLOAD:
		return "ON_UNLOAD";
	case ON_ENTER:
		return "ON_ENTER";
	case ON_EXIT:
		return "ON_EXIT";
	case ON_UPDATE:
		return "ON_UPDATE";
	case ON_RENDER:
		return "ON_RENDER";
	case ON_COLLIDE:
		return "ON_COLLIDE";
	case ON_COLLISION_ENTER:
		return "ON_COLLISION_ENTER";
	case ON_COLLISION_EXIT:
		return "ON_COLLISION_EXIT";
	case ON_EXAMINE:
		return "ON_EXAMINE";
	case ON_DIALOG:
		return "ON_DIALOG";
	case TRIGGER_MAX:
		return "TRIGGER_MAX";
	default:
		sprintf(buff, "INVALID (%llu)", flag);
		return buff;
	}
}

//----NONSTATIC----
	
void BehaviorBase::onCreate(void) { Debug::printf(Debug::CRITICAL, "Behavior::onCreate() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onDestroy(void) { Debug::printf(Debug::CRITICAL, "Behavior::onDestroy() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onLoad(void) { Debug::printf(Debug::CRITICAL, "Behavior::onLoad() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onUnload(void) { Debug::printf(Debug::CRITICAL, "Behavior::onUnload() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onEnter(void) { Debug::printf(Debug::CRITICAL, "Behavior::onEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onExit(void) { Debug::printf(Debug::CRITICAL, "Behavior::onExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onUpdate(void) { Debug::printf(Debug::CRITICAL, "Behavior::onUpdate() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onRender(void) { Debug::printf(Debug::CRITICAL, "Behavior::onRender() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollide(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { Debug::printf(Debug::CRITICAL, "Behavior::onCollide() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onShatter(CollisionContact*const /*contact*/) { Debug::printf(Debug::CRITICAL, "Behavior::onShatter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollisionEnter(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { Debug::printf(Debug::CRITICAL, "Behavior::onCollisionEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollisionExit(Collidable* const /*collidable*/) { Debug::printf(Debug::CRITICAL, "Behavior::onCollisionExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onExamine(Entity* /*entity*/) { Debug::printf(Debug::CRITICAL, "Behavior::onExamine() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onDialog(const unsigned /*screen*/, const int /*option*/) { Debug::printf(Debug::CRITICAL, "Behavior::onDialog() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatEnter(void) { Debug::printf(Debug::CRITICAL, "Behavior::onCombatEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatExit(void) { Debug::printf(Debug::CRITICAL, "Behavior::onCombatExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatDeath(void) { Debug::printf(Debug::CRITICAL, "Behavior::onCombatDeath() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatReceiveStatusModifier(CombatStatus*const /*modifier*/) { Debug::printf(Debug::CRITICAL, "Behavior::onCombatReceiveStatusModifier() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatTargetCollisionEnter(CollisionContact*const /*contact*/, Collidable*const /*collidable*/, CombatStatus*const /*modifier*/) { Debug::printf(Debug::CRITICAL, "Behavior::onCombatTargetCollisionEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onButtonPress(const GameInput::BUTTON /*button*/) { Debug::printf(Debug::CRITICAL, "Behavior::onButtonPress() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onButtonRelease(const GameInput::BUTTON /*button*/) { Debug::printf(Debug::CRITICAL, "Behavior::onButtonRelease() - Unimplemented for '%s'.", getBehavName()); }

void BehaviorBase::onTkSelect(void) { Debug::printf(Debug::CRITICAL, "Behavior::onTkSelect() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onTkUnselect(void) { Debug::printf(Debug::CRITICAL, "Behavior::onTkUnselect() - Unimplemented for '%s'.", getBehavName()); }

BehaviorBase::BehaviorBase(Entity& parent): Component(parent) {
//    dbgprintf(Asset::LOG_CRITICAL, "ENTITY - %s", parent.getName());

}

void BehaviorBase::addTrigger(TRIGGER flag) {
	std::map<const char*, Behavior*, ltstr>& behaviors = getEntity()->getBehaviors();

	//Trigger already added!
	if(_triggerBits & flag) return;

	//If entity already has the trigger enabled, we don't need to set anything up
	for(auto it = behaviors.begin();
		it != behaviors.end();
		++it) 
	{
		switch(flag) {
		default: break; //do we need to check shit before we set it up?

		}
	}

	_triggerBits |= flag;

	switch(flag) {
	case ON_COLLIDE:
	case ON_COLLISION_ENTER:
	case ON_COLLISION_EXIT:
		getEntity()->getCollidable()->setContactEventMonitorMask(
					getEntity()->getCollidable()->getContactEventMonitorMask()
					| ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK);
		break;
	default:
		break;
	}
}


void BehaviorBase::removeTrigger(TRIGGER flag) {
	auto& behaviors = getEntity()->getBehaviors();
	
	_triggerBits &= ~flag;

	//If entity still needs trigger config, don't tear shit down
	for(auto it = behaviors.begin();
		it != behaviors.end();
		++it) 
	{
		switch(flag) {
		case ON_COLLIDE:
		case ON_COLLISION_ENTER:
		case ON_COLLISION_EXIT:
			if(it->second->getTriggers() & (ON_COLLIDE | ON_COLLISION_EXIT | ON_COLLISION_ENTER)) {
				return;
			}
			break;
		default:break;

		}

	}

	switch(flag) {
	case ON_COLLIDE:
	case ON_COLLISION_ENTER:
	case ON_COLLISION_EXIT:
        if(!(_triggerBits & (ON_COLLIDE | ON_COLLISION_ENTER | ON_COLLISION_EXIT)))
		getEntity()->getCollidable()->setContactEventMonitorMask(
					getEntity()->getCollidable()->getContactEventMonitorMask()
					& ~ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK);
		break;
	default:
		break;
	}
}


}
