#include <AssetIO/crtp/elysian_behavior.hpp>
#include <AssetIO/crtp/elysian_collidable.hpp>
#include <AssetIO/utility/elysian_generic.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_area.hpp>

namespace elysian {

constexpr const char* BehaviorBase::triggerStrings[];

//ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Behavior)

//----STATIC----

std::map<const char *, Behavior* (*)(Entity&), ltstr> BehaviorBase::_creators;

void BehaviorBase::registerCreator(const char *const typeName, Behavior* (*const func)(Entity&)) {
	_creators[typeName] = func;
}
		
Behavior* BehaviorBase::create(const char* typeName, Entity& parent) {
	std::map<const char *, Behavior* (*)(Entity&), ltstr>::iterator it = _creators.find(typeName);
	if(it == _creators.end()) {
        Debug::printf(Debug::LOG_WARNING, "Behavior::create(%s) - No registered creator for %s.", typeName);
        return nullptr;
	}
	return _creators[typeName](parent);
}

const char* BehaviorBase::triggerNameFromFlag(const uint32_t flag) {
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
    case ON_COMBAT_ENTER:
        return "ON_COMBAT_ENTER";
    case ON_COMBAT_EXIT:
        return "ON_COMBAT_EXIT";
    case ON_COMBAT_DEATH:
        return "ON_COMBAT_DEATH";
    case ON_COMBAT_RECEIVE_STATUS_MODIFIER:
        return "ON_COMBAT_STATUS_RECEIVE_MODIFIER";
    case ON_COMBAT_TARGET_COLLISION_ENTER:
        return "ON_COMBAT_TARGET_COLLISION_ENTER";
    case ON_SHATTER:
        return "ON_SHATTER";
    case ON_OBSERVE_EVENT:
        return "ON_OBSERVE_EVENT";
    case ON_RECEIVE_EVENT:
        return "ON_RECEIVE_EVENT";
	case TRIGGER_MAX:
		return "TRIGGER_MAX";
	default:
        sprintf(buff, "INVALID (%u)", flag);
		return buff;
	}
}

//----NONSTATIC----
	
void BehaviorBase::onCreate(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCreate() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onDestroy(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onDestroy() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onLoad(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onLoad() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onUnload(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onUnload() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onEnter(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onExit(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onUpdate(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onUpdate() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onRender(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onRender() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollide(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCollide() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onShatter(CollisionContact*const /*contact*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onShatter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollisionEnter(CollisionContact*const /*contact*/,Collidable* const /*collidable*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCollisionEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCollisionExit(Collidable* const /*collidable*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCollisionExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onExamine(Entity* /*entity*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onExamine() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onDialog(const unsigned /*screen*/, const int /*option*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onDialog() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatEnter(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCombatEnter() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatExit(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCombatExit() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatDeath(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCombatDeath() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatReceiveStatusModifier(CombatStatus*const /*modifier*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCombatReceiveStatusModifier() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onCombatTargetCollisionEnter(CollisionContact*const /*contact*/, Collidable*const /*collidable*/, CombatStatus*const /*modifier*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onCombatTargetCollisionEnter() - Unimplemented for '%s'.", getBehavName()); }
//void BehaviorBase::onButtonPress(const GameInput::BUTTON /*button*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onButtonPress() - Unimplemented for '%s'.", getBehavName()); }
//void BehaviorBase::onButtonRelease(const GameInput::BUTTON /*button*/) { Debug::printf(Debug::LOG_WARNING, "Behavior::onButtonRelease() - Unimplemented for '%s'.", getBehavName()); }

void BehaviorBase::onTkSelect(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onTkSelect() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onTkUnselect(void) { Debug::printf(Debug::LOG_WARNING, "Behavior::onTkUnselect() - Unimplemented for '%s'.", getBehavName()); }

void BehaviorBase::onReceiveEvent(Event*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onReceiveEvent() - Unimplemented for '%s'.", getBehavName()); }
void BehaviorBase::onObserveEvent(Event*, EventTarget*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onObserveEvent() - Unimplemented for '%s'.", getBehavName()); }

BehaviorBase::BehaviorBase(Entity& parent): Component(parent) {
//    dbgprintf(Asset::LOG_LOG_WARNING, "ENTITY - %s", parent.getName());

}

void BehaviorBase::addTrigger(TRIGGER flag) {
    auto&& behaviors = getEntity()->getBehaviors();

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
        GY_ASSERT(getEntity()->getCollidable());
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
    for(auto it : behaviors)
	{
		switch(flag) {
		case ON_COLLIDE:
		case ON_COLLISION_ENTER:
		case ON_COLLISION_EXIT:
            if(it->getTriggers() & (ON_COLLIDE | ON_COLLISION_EXIT | ON_COLLISION_ENTER)) {
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
        if(!(_triggerBits & (ON_COLLIDE | ON_COLLISION_ENTER | ON_COLLISION_EXIT))) {
            GY_ASSERT(getEntity()->getCollidable());
            getEntity()->getCollidable()->setContactEventMonitorMask(
					getEntity()->getCollidable()->getContactEventMonitorMask()
					& ~ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK);
        }
		break;
	default:
		break;
	}
}

void BehaviorBase::onKeyPressEvent(KeyEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onKeyPressEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onKeyReleaseEvent(KeyEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onKeyReleaseEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onMousePressEvent(MouseEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onMousePressEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onMouseReleaseEvent(MouseEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onMouseReleaseEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onMouseMoveEvent(MouseEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onMouseMOveEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onMouseWheelEvent(MouseWheelEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onMouseWheelEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onControllerButtonPressEvent(ControllerButtonEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onControlerButtonPressEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onControllerButtonReleaseEvent(ControllerButtonEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onControllerButtonReleaseEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onControllerAnalogEvent(ControllerAnalogEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onControllerAnalogEvent() - Unimplemented for '%s'.", getBehavName());}
void BehaviorBase::onControllerTriggerEvent(ControllerTriggerEvent*) { Debug::printf(Debug::LOG_WARNING, "Behavior::onControllerTriggerEvent() - Unimplemented for '%s'.", getBehavName());}

}
