#include "elysian_behavior.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_entity.hpp"
#include "elysian_collidable.hpp"

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

//----NONSTATIC----
	
void Behavior::onCreate(void) { dbgprintf(LOG_CRITICAL, "Behavior::onCreate() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onDestroy(void) { dbgprintf(LOG_CRITICAL, "Behavior::onDestroy() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onLoad(void) { dbgprintf(LOG_CRITICAL, "Behavior::onLoad() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onUnload(void) { dbgprintf(LOG_CRITICAL, "Behavior::onUnload() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onEnter(void) { dbgprintf(LOG_CRITICAL, "Behavior::onEnter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onExit(void) { dbgprintf(LOG_CRITICAL, "Behavior::onExit() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onUpdate(void) { dbgprintf(LOG_CRITICAL, "Behavior::onUpdate() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onRender(void) { dbgprintf(LOG_CRITICAL, "Behavior::onRender() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollide(Collidable* const collidable) { dbgprintf(LOG_CRITICAL, "Behavior::onCollide() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollisionEnter(Collidable* const collidable) { dbgprintf(LOG_CRITICAL, "Behavior::onCollisionEnter() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onCollisionExit(Collidable* const collidable) { dbgprintf(LOG_CRITICAL, "Behavior::onCollisionExit() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onExamine(Entity* entity) { dbgprintf(LOG_CRITICAL, "Behavior::onExamine() - Unimplemented for '%s'.\n", getBehavName()); }
void Behavior::onDialog(const unsigned screen, const int option) { dbgprintf(LOG_CRITICAL, "Behavior::onDialog() - Unimplemented for '%s'.\n", getBehavName()); }

void Behavior::addTrigger(TRIGGER flag) {
	std::map<const char*, Behavior*, ltstr>& behaviors = getEntity()->getBehaviors();

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
		getEntity()->getCollidable()->setTrigger(true);
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
		getEntity()->getCollidable()->setTrigger(false);
		break;
	default:
		break;
	}
}

bool Behavior::load(std::ifstream &stream) {
	stream >> _triggerBits;
	return true;
}

bool Behavior::save(std::ofstream &stream) {
	stream << _triggerBits << std::endl;
	return false;
}

bool Behavior::addToLevel(void) {
	_curArea->behaviors.push_back(this);
	return true;
}

bool Behavior::removeFromLevel(void) {
	for(unsigned int i = 0; i < _curArea->behaviors.size(); ++i) {
		if(_curArea->behaviors[i] == this) {
			_curArea->behaviors.erase(_curArea->behaviors.begin() + i);
			return true;
		}
	}
	return false;
}


}
