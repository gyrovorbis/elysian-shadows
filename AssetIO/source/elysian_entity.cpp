#include <fstream>
#include <algorithm>

#include "elysian_asset.hpp"
#include "elysian_component.hpp"
#include "elysian_behavior.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_animation.hpp"
#include "elysian_collidable.hpp"
#include "elysian_character.hpp"
#include "elysian_enemy.hpp"
#include "elysian_friendly.hpp"
#include "elysian_warp.hpp"
#include "elysian_item.hpp"
#include "elysian_area.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_examinable.hpp"
#include "elysian_audio_source.hpp"

using namespace std;

namespace elysian {

Entity::Entity(void)
{
	for(unsigned int i = 0; i < Component::MAX_TYPE; ++i) _componentActive[i] = false;
	strcpy(_name, "Untitled");
} 

void Entity::reested(void) {
	dbgprintf(LOG_VERBOSE, "Entity::Reested(%s) - Dereesting entity!", _name);
	//SHED COMPONENTS GRACEFULLY
	strcpy(_name, "Reested");
}

Entity::~Entity(void) {
	
	if(_active) deactivate();
	
	//Cleanup Components
	removeRenderable();
	removeAnimation();
	removeCollidable();
	removeCharacter();
	removeWarp();
	removeItem();
	removeFriendly();
	removeEnemy();
	removeExaminable();
	removeAudioSource();
	
	// Cleanup Behaviors
	for(std::map<const char *, Behavior *, ltstr>::iterator it = _behavior.begin(); it != _behavior.end(); ++it) {
		removeBehavior(it->first);
	}
	_behavior.clear();
}

void Entity::render(void) {
#ifdef ELYSIAN_ENGINE
	if(!_renderable) return;
	gyMatPush();
	transform.apply();
	_renderable->beginRender();
	_renderable->render();
	_renderable->endRender();
	gyMatPop();
#endif
}
    
	void Entity::renderAtZ(const float
#ifdef ELYSIAN_ENGINE
						   z
#endif
						   ) {
#ifdef ELYSIAN_ENGINE
        if(!_renderable) return;
        gyMatPush();
        float oldZ = transform._posZ;
        transform._posZ = z;
        transform.apply();
        transform._posZ = oldZ;
        _renderable->beginRender();
        _renderable->render();
        _renderable->endRender();
        gyMatPop();
#endif
    }
    
#ifdef ELYSIAN_ENGINE
void Entity::move(const GYVector2& dist) {
    if(_character) _character->move(dist);
    else gyVec2AddAccum(&transform._pos, &dist);
}
#endif


bool Entity::activate(void) {
	if(_active) return false;
	_active = true;

	_curArea->entities.push_back(this);

	if(_renderable && !_componentActive[Component::RENDERABLE]) {
		_renderable->addToLevel();
		_componentActive[Component::RENDERABLE] = true;
	}
	if(_animation && !_componentActive[Component::ANIMATION]) {
		_animation->addToLevel();
		_componentActive[Component::ANIMATION] = true;
	}
	if(_collidable && !_componentActive[Component::COLLIDABLE]) {
		_collidable->addToLevel();
		_componentActive[Component::COLLIDABLE] = true;
	}
	if(_character && !_componentActive[Component::CHARACTER]) {
		_character->addToLevel();
		_componentActive[Component::CHARACTER] = true;
	}
	if(_item && !_componentActive[Component::ITEM]) {
		_item->addToLevel();
		_componentActive[Component::ITEM] = true;
	}
	if(_warp && !_componentActive[Component::WARP]) {
		_warp->addToLevel();
		_componentActive[Component::WARP] = true;
	}
	if(_friendly && !_componentActive[Component::FRIENDLY]) {
		_friendly->addToLevel();
		_componentActive[Component::FRIENDLY] = true;
	}
	if(_enemy && !_componentActive[Component::ENEMY]) {
		_enemy->addToLevel();
		_componentActive[Component::ENEMY] = true;
	}
	if(_examinable && !_componentActive[Component::EXAMINABLE]) {
		_examinable->addToLevel();
		_componentActive[Component::EXAMINABLE] = true;
	}
	if(_audioSource && !_componentActive[Component::AUDIO_SOURCE]) {
		_audioSource->addToLevel();
		_componentActive[Component::AUDIO_SOURCE] = true;
	}
	for(std::map<const char *, Behavior *, ltstr>::iterator it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(!it->second->getActive()) {
			it->second->addToLevel();
			it->second->setActive();
		}
	}
	return true;
}

bool Entity::deactivate(void) {
	if(!_active) return false;

    std::vector<Entity*>::iterator it;
	if((it = std::find(_curArea->entities.begin(), _curArea->entities.end(), this)) != _curArea->entities.end()) {
		_curArea->entities.erase(it);
	}
	
	if(_renderable && _componentActive[Component::RENDERABLE]) {
		_renderable->removeFromLevel();
		_componentActive[Component::RENDERABLE] = false;
	}
	if(_animation && _componentActive[Component::ANIMATION]) { 
		_animation->removeFromLevel();
		_componentActive[Component::ANIMATION] = false;
	}
	if(_collidable && _componentActive[Component::COLLIDABLE]) {
		_collidable->removeFromLevel();
		_componentActive[Component::COLLIDABLE] = false;
	}
	if(_character && _componentActive[Component::CHARACTER]) {
		_character->removeFromLevel();
		_componentActive[Component::CHARACTER] = false;
	}
	if(_item && _componentActive[Component::ITEM]) {
		_item->removeFromLevel();
		_componentActive[Component::ITEM] = false;
	}
	if(_warp && _componentActive[Component::WARP]) {
		_warp->removeFromLevel();
		_componentActive[Component::WARP] = false;
	}
	if(_friendly && _componentActive[Component::FRIENDLY]) {
		_friendly->removeFromLevel();
		_componentActive[Component::FRIENDLY] = false;
	}
	if(_enemy && _componentActive[Component::ENEMY]) {
		_enemy->removeFromLevel();
		_componentActive[Component::ENEMY] = false;
	}
	if(_examinable && _componentActive[Component::EXAMINABLE]) {
		_examinable->removeFromLevel();
		_componentActive[Component::EXAMINABLE] = false;
	}
	if(_audioSource && _componentActive[Component::AUDIO_SOURCE]) {
		_audioSource->removeFromLevel();
		_componentActive[Component::AUDIO_SOURCE] = false;
	}

	for(std::map<const char *, Behavior *, ltstr>::iterator it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(it->second->getActive()) {
			it->second->removeFromLevel();
			it->second->setActive(false);
		}
	}
	
	_active = false;
	return true;
}

bool Entity::load(ifstream &stream) { //Deserialization
	bool success = true;
	unsigned int amount, compType;
	
	dbgprintf(LOG_VERBOSE, "Entity::Load(ifstream)");
    dbgPush();

	stream >> _name;
	dbgprintf(LOG_VERBOSE, "_name = %s", _name);

	//Transform
	transform.load(stream);

	//Components
	stream >> amount;
	for(unsigned i = 0; i < amount; ++i) {
        Component *comp = NULL;
		char behaviorName[ELYSIAN_BEHAVIOR_NAME_SIZE];
		stream >> compType;
		
		switch((Component::TYPE)compType) {
			case Component::RENDERABLE:
				comp = addRenderable();
				break;
			case Component::ANIMATION:
				comp = addAnimation();
				break;
			case Component::COLLIDABLE:
				comp = addCollidable();
				break;
			case Component::CHARACTER:
				comp = addCharacter();
				break;
			case Component::WARP:
				comp = addWarp();
				break;
			case Component::ITEM:
				comp = addItem();
				break;
			case Component::FRIENDLY:
				comp = addFriendly();
				break;
			case Component::ENEMY:
				comp = addEnemy();
				break;
			case Component::EXAMINABLE:
				comp = addExaminable();
				break;
			case Component::AUDIO_SOURCE:
				comp = addAudioSource();
				break;
			case Component::BEHAVIOR:
				stream >> behaviorName;
				comp = addBehavior(behaviorName);
				break;
			default:
				dbgprintf(LOG_CRITICAL, "Unknown component type %d!", compType);
				comp = NULL;
				break;
		}
		
		if(!comp) { //Deserialization of this entity has reested. Bail out.
			reested();
			success = false;
			break;
		}
		else if(!comp->load(stream)) {
			dbgprintf(LOG_CRITICAL, "Unable to create component %d", compType);
			success = false;
			continue;
		}
	}
    dbgPop();
	return success;
}

bool Entity::save(ofstream &stream) { //Serialization
    Component *comp = NULL;
	bool success = true;
	dbgprintf(LOG_VERBOSE, "Entity::Save(ofstream)");
    dbgPush();

    stream.precision(6);

	stream << _name << endl;

	//Transform
	transform.save(stream);

	//Component Count
	unsigned int compCount = 0;
	if(getRenderable()) ++compCount;
	if(getAnimation()) ++compCount;
	if(getCollidable()) ++compCount;
	if(getCharacter()) ++compCount;
	if(getFriendly()) ++compCount;
	if(getEnemy()) ++compCount;
	if(getItem()) ++compCount;
	if(getWarp()) ++compCount;
	if(getExaminable()) ++compCount;
	if(getAudioSource()) ++compCount;
    for(std::map<const char *, Behavior *, ltstr>::iterator it = _behavior.begin(); it != _behavior.end(); ++it) ++compCount;
	stream << compCount << endl;

	//Built-in Components
	if((comp = getRenderable())) {
		stream << (int)Component::RENDERABLE << endl;
        success &= comp->save(stream);
	}
	if((comp = getAnimation())) {
		stream << (int)Component::ANIMATION << endl;
        success &= comp->save(stream);
	}
    if((comp = getCollidable())) {
		stream << (int)Component::COLLIDABLE << endl;
        success &= comp->save(stream);
	}
    if((comp = getItem())) {
		stream << (int)Component::ITEM << endl;
        success &= comp->save(stream);
    }
    if((comp = getWarp())) {
		stream << (int)Component::WARP << endl;
        success &= comp->save(stream);
    }
    if((comp = getCharacter())) {
		stream << (int)Component::CHARACTER << endl;
		success &= comp->save(stream);
    }
    if((comp = getFriendly())) {
		stream << (int)Component::FRIENDLY << endl;
        success &= comp->save(stream);
    }
    if((comp = getEnemy())) {
		stream << (int)Component::ENEMY << endl;
        success &= comp->save(stream);
    }
    if((comp = getExaminable())) {
		stream << (int)Component::EXAMINABLE << endl;
        success &= comp->save(stream);
    }
	if((comp = getAudioSource())) {
		stream << (int)Component::AUDIO_SOURCE << endl;
		success &= comp->save(stream);
	}
	
	//Behaviors
    for(std::map<const char *, Behavior *, ltstr>::iterator it = _behavior.begin(); it != _behavior.end(); ++it) {
		stream << (int)Component::BEHAVIOR << endl;
        stream << it->second->getBehavName() << endl;
        success &= it->second->save(stream);
    }
    dbgPop();
	return success;
}

//=====BEHAVIORS======

///===========THIS SHIT ALL BEGS TO BE TEMPLATIZED!? GOD, NO!============

Behavior* Entity::addBehavior(const char *name) {
	return attachBehavior(Behavior::create(name, *this));
}

LuaBehavior* Entity::addLuaBehavior(const char *name) {
	return static_cast<LuaBehavior*>(attachBehavior(new LuaBehavior(*this, name)));
}

     
Behavior *Entity::attachBehavior(Behavior *behav) {
	//Check for duplicate behaviors.
	std::map<const char *, Behavior*, ltstr>::iterator it = _behavior.find(behav->getBehavName());
	if(it != _behavior.end()) {
		dbgprintf(LOG_CRITICAL, "Entity::AddBehavior(%s) - Already attached!", behav->getBehavName());
		delete behav;
		return NULL;
	}

	//Check for prereqs
	if(!behav->canAddTo(*this)) {
		dbgprintf(LOG_CRITICAL, "Could not attach behavior!");
		delete behav;
		return NULL;
	}
	
	//Attach using its own typeName as a key (so that it never leaves scope), fixing the reast mentioned above.
	_behavior[behav->getBehavName()] = behav;
	
	//add to level
	if(_active) {
		behav->addToLevel();
		behav->setActive();
	}
	
	return behav;
}

LuaBehavior *Entity::getLuaBehavior(const char*script) const {
	return static_cast<LuaBehavior*>(getBehavior(script));
}

void Entity::removeLuaBehavior(const char *script) {
	removeBehavior(script);
}

Behavior *Entity::getBehavior(const char *behaviorName) const {
	std::map<const char *, Behavior*, ltstr>::const_iterator it = _behavior.find(behaviorName);
	if(it == _behavior.end()) {
		dbgprintf(LOG_CRITICAL, "Behavior::getBehavior(%s) - Not attached!", behaviorName);
		return NULL;
	}
	else return it->second;
}

void Entity::removeBehavior(const char *behaviorName) {
	std::map<const char *, Behavior*, ltstr>::iterator it = _behavior.find(behaviorName);
	if(it == _behavior.end()) {
		dbgprintf(LOG_CRITICAL, "Behavior::RemoveBehavior(%s) - Not attached!", behaviorName);
		return;
	}
	
	//remove component from area if not null
	if(_active) {
		it->second->removeFromLevel();
		it->second->setActive(false);
	}
	delete it->second;
	_behavior.erase(it);
}
    
    
///===========THIS SHIT ALL BEGS TO BE TEMPLATIZED!?============

bool Entity::attachComponent(Component *comp) {
	if(!comp->canAddTo(*this)) {
		dbgprintf(LOG_CRITICAL, "Prereq error. Could not attach component!");
		delete comp;
		return false;
	}
	//add to level
	if(_active) {
		comp->addToLevel();
		_componentActive[comp->getType()] = true;
	}
	return true;
}

Renderable* Entity::addRenderable(void) {
	if(_renderable) return NULL;
	_renderable = new Renderable(*this);
	return _renderable = attachComponent(_renderable)? _renderable : NULL;
}

Animation *Entity::addAnimation(void) {
	if(_animation) return NULL;
	_animation = new Animation(*this);
	return _animation = attachComponent(_animation)? _animation : NULL;
}

Collidable *Entity::addCollidable(void) {
	if(_collidable) return NULL;
	_collidable = new Collidable(*this);
	return _collidable = attachComponent(_collidable)? _collidable : NULL;
}

Character *Entity::addCharacter(void) {
	if(_character) return NULL;
	_character = new Character(*this);
	return _character = attachComponent(_character)? _character : NULL;
}

Warp *Entity::addWarp(void) {
	if(_warp) return NULL;
	_warp = new Warp(*this);
	return _warp = attachComponent(_warp)? _warp : NULL;
}

Item *Entity::addItem(void) {
	if(_item) return NULL;
	_item = new Item(*this);
	return _item = attachComponent(_item)? _item : NULL;
}

Friendly *Entity::addFriendly(void) {
	if(_friendly) return NULL;
	_friendly = new Friendly(*this);
	return _friendly = attachComponent(_friendly)? _friendly : NULL;
}

Enemy *Entity::addEnemy(void) {
	if(_enemy) return NULL;
	_enemy = new Enemy(*this);
	return _enemy = attachComponent(_enemy)? _enemy : NULL;
}

Examinable *Entity::addExaminable(void) {
	if(_examinable) return NULL;
	_examinable = new Examinable(*this);
	return _examinable = attachComponent(_examinable)? _examinable : NULL;
}

AudioSource *Entity::addAudioSource(void) {
	if(_audioSource) return NULL;
	_audioSource = new AudioSource(*this);
	return _audioSource = attachComponent(_audioSource)? _audioSource : NULL;
}


void Entity::removeRenderable(void) {
	if(_renderable && _active) {
		_renderable->removeFromLevel();
		_componentActive[Component::RENDERABLE] = false;
	}
	delete _renderable;
	_renderable = NULL;
}

void Entity::removeAnimation(void) {
	if(_animation && _active) {
		_animation->removeFromLevel();
		_componentActive[Component::ANIMATION] = false;
	}
	delete _animation;
	_animation = NULL;
}

void Entity::removeCollidable(void) {
	if(_collidable && _active) {
		_collidable->removeFromLevel();
		_componentActive[Component::COLLIDABLE] = false;
	}
	delete _collidable;
	_collidable = NULL;
}

void Entity::removeCharacter(void) {
	if(_character && _active) {
		_character->removeFromLevel();
		_componentActive[Component::CHARACTER] = false;
	}
	delete _character;
	_character = NULL;
	
}
void Entity::removeWarp(void) {
	if(_warp && _active) {
		_warp->removeFromLevel();
		_componentActive[Component::WARP] = false;
	}
	delete _warp;
	_warp = NULL;
}

void Entity::removeItem(void) {
	if(_item && _active) {
		_item->removeFromLevel();
		_componentActive[Component::ITEM] = false;
	}
	delete _item;
	_item = NULL;
}

void Entity::removeFriendly(void) {
	if(_friendly && _active) {
		_friendly->removeFromLevel();
		_componentActive[Component::FRIENDLY] = false;
	}
	delete _friendly;
	_friendly = NULL;
}

void Entity::removeEnemy(void) {
	if(_enemy && _active) {
		_enemy->removeFromLevel();
		_componentActive[Component::ENEMY] = false;
	}
	delete _enemy;
	_enemy = NULL;
}

void Entity::removeExaminable(void) {
	if(_examinable && _active) {
		_examinable->removeFromLevel();
		_componentActive[Component::EXAMINABLE] = false;
	}
	delete _examinable;
	_examinable = NULL;
}

void Entity::removeAudioSource(void) {
	if(_audioSource && _active) {
		_audioSource->removeFromLevel();
		_componentActive[Component::AUDIO_SOURCE] = false;
	}
	delete _audioSource;
	_audioSource = NULL;
}

}
