#include "elysian_behavior.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_collidable.hpp"
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")

namespace elysian {

//----STATIC----

std::map<const char *, Behavior* (*)(Entity&), ltstr> Behavior::_creators;

void Behavior::registerCreator(const char *const typeName, Behavior* (*const func)(Entity&)) {
	_creators[typeName] = func;
}

void Behavior::staticConstructor(void) {
	//REGISTER C++ BEHAVIORS HERE
}
		
Behavior* Behavior::create(const char* typeName, Entity& parent) {
	std::map<const char *, Behavior* (*)(Entity&), ltstr>::iterator it = _creators.find(typeName);
	if(it == _creators.end()) {
		dbgprintf(LOG_CRITICAL, "Behavior::create(%s) - No registered creator for %s.", typeName);
		return NULL;
	}
	return _creators[typeName](parent);
}

const char* Behavior::triggerNameFromFlag(const uint64_t flag) {
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
	
void Behavior::onCreate(void) { dbgprintf(LOG_CRITICAL, "Behavior::onCreate() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onDestroy(void) { dbgprintf(LOG_CRITICAL, "Behavior::onDestroy() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onLoad(void) { dbgprintf(LOG_CRITICAL, "Behavior::onLoad() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onUnload(void) { dbgprintf(LOG_CRITICAL, "Behavior::onUnload() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onEnter(void) { dbgprintf(LOG_CRITICAL, "Behavior::onEnter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onExit(void) { dbgprintf(LOG_CRITICAL, "Behavior::onExit() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onUpdate(void) { dbgprintf(LOG_CRITICAL, "Behavior::onUpdate() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onRender(void) { dbgprintf(LOG_CRITICAL, "Behavior::onRender() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollide(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { dbgprintf(LOG_CRITICAL, "Behavior::onCollide() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onShatter(CollisionContact*const /*contact*/) { dbgprintf(LOG_CRITICAL, "Behavior::onShatter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollisionEnter(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { dbgprintf(LOG_CRITICAL, "Behavior::onCollisionEnter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollisionExit(Collidable* const /*collidable*/) { dbgprintf(LOG_CRITICAL, "Behavior::onCollisionExit() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onExamine(Entity* /*entity*/) { dbgprintf(LOG_CRITICAL, "Behavior::onExamine() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onDialog(const unsigned /*screen*/, const int /*option*/) { dbgprintf(LOG_CRITICAL, "Behavior::onDialog() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCombatEnter(void) { dbgprintf(LOG_CRITICAL, "Behavior::onCombatEnter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCombatExit(void) { dbgprintf(LOG_CRITICAL, "Behavior::onCombatExit() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCombatDeath(void) { dbgprintf(LOG_CRITICAL, "Behavior::onCombatDeath() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCombatReceiveStatusModifier(CombatStatus*const /*modifier*/) { dbgprintf(LOG_CRITICAL, "Behavior::onCombatReceiveStatusModifier() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCombatTargetCollisionEnter(CollisionContact*const /*contact*/, Collidable*const /*collidable*/, CombatStatus*const /*modifier*/) { dbgprintf(LOG_CRITICAL, "Behavior::onCombatTargetCollisionEnter() - Unimplemented for '%s'.\n", getBehavName()); }

void Behavior::addTrigger(TRIGGER flag) {
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


void Behavior::removeTrigger(TRIGGER flag) {
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

bool Behavior::load(FILE* /*file*/) {
	//stream >> _triggerBits;
	return true;
}

bool Behavior::save(FILE* /*file*/) {
	//stream << _triggerBits << std::endl;
	return false;
}

bool Behavior::addToLevel(void) {
	_curArea->behaviors.push_back(this);
	return true;
}

bool Behavior::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->behaviors);
}


}
