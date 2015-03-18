#include <algorithm>

#include "elysian_asset.hpp"
#include "elysian_component.hpp"
#include "elysian_behavior.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_animation.hpp"
#include "elysian_collidable.hpp"
#include "elysian_character.hpp"
#include "elysian_warp.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_examinable.hpp"
#include "elysian_audio_source.hpp"
#include "elysian_light_source.hpp"
#include "elysian_particle_emitter.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_wrappers.hpp"

using namespace std;

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(EntityBase)

float EntityBase::_oldZ;

EntityBase::EntityBase(const OOLUA::Lua_table_ref valTable) { static_cast<Entity*>(this)->fromTable(valTable); }

void EntityBase::setTransform(const OOLUA::Lua_table_ref valTable) { _transform.fromTable(valTable); }

Entity* EntityBase::getTopParentEntity(void) {
    Entity* it = static_cast<Entity*>(this);
    for(; it->_parent; it = it->_parent);
    return it;
}

void EntityBase::reested(void) {
	dbgprintf(LOG_VERBOSE, "Entity::Reested(%s) - Dereesting entity!", _name);
	//SHED COMPONENTS GRACEFULLY
	strcpy(_name, "Reested");
}

EntityBase::~EntityBase(void) {
	
	if(_active) deactivate();
	
	//Cleanup Components
	removeRenderable();
	removeAnimation();
	removeCollidable();
	removeRigidBody();
	removeCharacter();
	removeWarp();
	removeExaminable();
	removeAudioSource();
	removeLightSource();
	removeParticleEmitter();
	removeCombat();
	
	// Cleanup Behaviors
	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		removeBehavior(it->first);
	}
	_behavior.clear();
}

void EntityBase::pushViewTransform(void) {
    gyMatPush();
    _oldZ = _transform._size.z;
    if(_planeType == GY_RECT_PLANE_FRONT) {
        _transform._size.z = _transform._size.y;
        //transform._size.y = 400.0f;
    } else {
        //transform._pos.z += _shadowDepth;
    }

    if(_planeType == GY_RECT_PLANE_BOTTOM) {
        _transform.apply();
    } else {

        gyMatTranslate(_transform._pos.x, _transform._pos.y, _transform._pos.z);
        gyMatScale(_transform._size.x*_transform._scale.x, _transform._size.y*_transform._scale.y, _transform._size.z*_transform._scale.z);
        gyMatTranslate(0.0f, 0.0f, 0.5f);
        gyMatRotate(_transform._orientZ, 0.0f, 1.0f, 0.0f);
        gyMatTranslate(0.0f, 0.0f, -0.5f);


    }

}

void EntityBase::popViewTransform(void) {
    gyMatPop();
    _transform._size.z = _oldZ;
}

void EntityBase::render(void) {
	if(_particleEmitter) _particleEmitter->render();
	if(!_renderable) return;

    pushViewTransform();
    static_cast<Entity*>(this)->_renderRelative();
    popViewTransform();

}

void EntityBase::_renderRelative(void) {
    _renderable->beginRender();
    //if(_depthType == DEPTH_BUFFERED) transform._size.y = transform._size.z;
    _renderable->render();
    _renderable->endRender();
}

#if 0
    void EntityBase::renderAtZ(const float z) {
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
	}
#endif

void EntityBase::renderShadowVolume(void) {
/*
	static const float verts[17*4] = {
		-1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		-1, -1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		-1, -1, -1, 1.0f,
		1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		 1,  1, -1, 1.0f,
		 -1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		 1,  1, -1, 1.0f,
		 1, -1, -1, 1.0f,
		-1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		-1, -1,  1, 1.0f,
		-1,  1,  1, 1.0f
	};*/

	static const float verts[17*4] = {
		-1,  1,  2, 1.0f,
		 1,  1,  2, 1.0f,
		-1, -1,  2, 1.0f,
		 1, -1,  2, 1.0f,
		-1, -1, 0, 1.0f,
		1, -1, 0, 1.0f,
		-1,  1, 0, 1.0f,
		 1,  1, 0, 1.0f,
		 -1,  1,  2, 1.0f,
		 1,  1,  2, 1.0f,
		 1, -1,  2, 1.0f,
		 1,  1, 0, 1.0f,
		 1, -1, 0, 1.0f,
		-1, -1, 0, 1.0f,
		-1,  1, 0, 1.0f,
		-1, -1,  2, 1.0f,
		-1,  1,  2, 1.0f
	};
/*
	static const float verts[17*4] = {
		-1,  1,  2, 1.0f,
		 1,  1,  2, 1.0f,
		-1, -1,  2, 1.0f,
		 1, -1,  2, 1.0f,
		-1, -1, 0, 1.0f,
		1, -1, 0, 1.0f,
		-1,  1, 0, 1.0f,
		 1,  1, 0, 1.0f,
		 -1,  1,  2, 1.0f,
		 1,  1,  2, 1.0f,
		 1, -1,  2, 1.0f,
		 1,  1, 0, 1.0f,
		 1, -1, 0, 1.0f,
		-1, -1, 0, 1.0f,
		-1,  1, 0, 1.0f,
		-1, -1,  2, 1.0f,
		-1,  1,  2, 1.0f
	};*/

	static const GYColor4 colors[17] = {
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 }
	};

	gyMatPush();
	gyMatLoadIdentity();

#if 1

	GYVector3 halfSize = {
        _transform._size.x*_transform._scale.x*0.5f,
		_shadowDepth*0.5f,
        _transform._size.y*_transform._scale.y*0.5f
	};

    if(_planeType == GY_RECT_PLANE_FRONT) {
		const float shearFactor = halfSize.z/halfSize.y;

        gyMatTranslate(_transform._pos.x,
                       _transform._pos.y+shearFactor*halfSize.y, //move to bottom then move back up by half the depth
                       _transform._pos.z);

		gyMatScale(halfSize.x, halfSize.y, halfSize.z);
		gyMatShearZ(0.0f, -shearFactor);
		//gyMatRotateZ(transform._orientZ);

	} else {

        gyMatTranslate(_transform._pos.x,
                       _transform._pos.y, //move to bottom then move back up by half the depth
                       _transform._pos.z);

        gyMatRotateZ(-_transform._orientZ);
		gyMatScale(halfSize.x, halfSize.z, halfSize.y);

	}

#else



	GYVector3 halfSize = {
		transform._size.x*transform._scale.x*0.5f,
		_shadowDepth*0.5f,
		transform._size.y*transform._scale.y*0.5f
	};

	if(_normalType == NORMAL_FRONT) {

		gyMatTranslate(transform._pos.x,
					   transform._pos.y+halfSize.z, //move to bottom then move back up by half the depth
					   ZCoordConstants::mapBase + (_elev)*32.0f);

		gyMatScale(halfSize.x, -halfSize.y, halfSize.z*2.0f);
		gyMatShearZ(0.0f, 1.0f);
		//gyMatRotateZ(transform._orientZ);

	} else {

		gyMatTranslate(transform._pos.x,
					   transform._pos.y, //move to bottom then move back up by half the depth
					   ZCoordConstants::mapBase + (_elev)*32.0f + halfSize.z);

		gyMatScale(halfSize.x, halfSize.z, halfSize.y);
		gyMatRotateZ(transform._orientZ);

	}




#endif

	//transform.apply();
	gyVidColorArray(colors);
	gyVidTriangleStrip((GYVector4*)verts, 0, 17);

	gyMatPop();


}
    
void EntityBase::move(const GYVector2& dist) {
    if(_character) _character->moveByForce(dist);
    else gyVec2AddAccum((GYVector2*)&_transform._pos, &dist);
}


bool EntityBase::activate(void) {
	if(_active) return false;
	_active = true;

    _curArea->entities.push_back(static_cast<Entity*>(this));

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
	if(_rigidBody && !_componentActive[Component::RIGID_BODY]) {
		_rigidBody->addToLevel();
		_componentActive[Component::RIGID_BODY] = true;
	}
	if(_character && !_componentActive[Component::CHARACTER]) {
		_character->addToLevel();
		_componentActive[Component::CHARACTER] = true;
	}
	if(_warp && !_componentActive[Component::WARP]) {
		_warp->addToLevel();
		_componentActive[Component::WARP] = true;
	}
	if(_examinable && !_componentActive[Component::EXAMINABLE]) {
		_examinable->addToLevel();
		_componentActive[Component::EXAMINABLE] = true;
	}
	if(_audioSource && !_componentActive[Component::AUDIO_SOURCE]) {
		_audioSource->addToLevel();
		_componentActive[Component::AUDIO_SOURCE] = true;
	}
	if(_lightSource && !_componentActive[Component::LIGHT_SOURCE]) {
		_lightSource->addToLevel();
		_componentActive[Component::LIGHT_SOURCE] = true;
	}
	if(_particleEmitter && !_componentActive[Component::PARTICLE_EMITTER]) {
		_particleEmitter->addToLevel();
		_componentActive[Component::PARTICLE_EMITTER] = true;
	}
	if(_combat && !_componentActive[Component::COMBAT]) {
		_combat->addToLevel();
		_componentActive[Component::COMBAT] = true;
	}
	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(!it->second->getActive()) {
			it->second->addToLevel();
			it->second->setActive();
		}
	}
	return true;
}

bool EntityBase::deactivate(void) {
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
	if(_rigidBody && _componentActive[Component::RIGID_BODY]) {
		_rigidBody->removeFromLevel();
		_componentActive[Component::RIGID_BODY] = false;
	}
	if(_character && _componentActive[Component::CHARACTER]) {
		_character->removeFromLevel();
		_componentActive[Component::CHARACTER] = false;
	}
	if(_warp && _componentActive[Component::WARP]) {
		_warp->removeFromLevel();
		_componentActive[Component::WARP] = false;
	}
	if(_examinable && _componentActive[Component::EXAMINABLE]) {
		_examinable->removeFromLevel();
		_componentActive[Component::EXAMINABLE] = false;
	}
	if(_audioSource && _componentActive[Component::AUDIO_SOURCE]) {
		_audioSource->removeFromLevel();
		_componentActive[Component::AUDIO_SOURCE] = false;
	}
	if(_lightSource && _componentActive[Component::LIGHT_SOURCE]) {
		_lightSource->removeFromLevel();
		_componentActive[Component::LIGHT_SOURCE] = false;
	}
	if(_particleEmitter && _componentActive[Component::PARTICLE_EMITTER]) {
		_particleEmitter->removeFromLevel();
		_componentActive[Component::PARTICLE_EMITTER] = false;
	}
	if(_combat && _componentActive[Component::COMBAT]) {
		_combat->removeFromLevel();
		_componentActive[Component::COMBAT] = false;
	}

	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(it->second->getActive()) {
			it->second->removeFromLevel();
			it->second->setActive(false);
		}
	}
	
	_active = false;
	return true;
}

bool EntityBase::load(FILE* /*file*/) { //Deserialization
#if 0
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
			case Component::RIGID_BODY:
				comp = addRigidBody();
				break;
			case Component::CHARACTER:
				comp = addCharacter();
				break;
			case Component::WARP:
				comp = addWarp();
				break;
			case Component::EXAMINABLE:
				comp = addExaminable();
				break;
			case Component::AUDIO_SOURCE:
				comp = addAudioSource();
				break;
			case Component::LIGHT_SOURCE:
				comp = addLightSource();
				break;
			case Component::PARTICLE_EMITTER:
				comp = addParticleEmitter();
				break;
			case Component::COMBAT:
				comp = addCombat();
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
#endif
	return true;
}

bool EntityBase::save(FILE* /*file*/) { //Serialization
#if 0
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
	if(getRigidBody()) ++compCount;
	if(getCharacter()) ++compCount;
	if(getWarp()) ++compCount;
	if(getExaminable()) ++compCount;
	if(getAudioSource()) ++compCount;
	if(getLightSource()) ++compCount;
	if(getParticleEmitter()) ++compCount;
	if(getCombat()) ++compCount;
	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) ++compCount;
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
	if((comp = getRigidBody())) {
		stream << (int)Component::RIGID_BODY << endl;
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
    if((comp = getExaminable())) {
		stream << (int)Component::EXAMINABLE << endl;
        success &= comp->save(stream);
    }
	if((comp = getAudioSource())) {
		stream << (int)Component::AUDIO_SOURCE << endl;
		success &= comp->save(stream);
	}
	if((comp = getLightSource())) {
		stream << (int)Component::LIGHT_SOURCE << endl;
		success &= comp->save(stream);
	}
	if((comp = getParticleEmitter())) {
		stream << (int)Component::PARTICLE_EMITTER << endl;
		success &= comp->save(stream);
	}
	if((comp = getCombat())) {
		stream << (int)Component::COMBAT << endl;
		success &= comp->save(stream);
	}
	
	//Behaviors
	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		stream << (int)Component::BEHAVIOR << endl;
        stream << it->second->getBehavName() << endl;
        success &= it->second->save(stream);
    }
    dbgPop();
	return success;
#endif
	return true;
}

//=====BEHAVIORS======

///===========THIS SHIT ALL BEGS TO BE TEMPLATIZED!? GOD, NO! -- YES============

Behavior* EntityBase::addBehavior(const char *name) {
    return attachBehavior(Behavior::create(name, *static_cast<Entity*>(this)));
}

LuaBehavior* EntityBase::addLuaBehavior(const char *name, const OOLUA::Lua_table_ref paramTable) {
    return static_cast<LuaBehavior*>(attachBehavior(new LuaBehavior(*static_cast<Entity*>(this), name, paramTable)));
}
     
Behavior *EntityBase::attachBehavior(Behavior *behav) {
	//Check for duplicate behaviors.
#if 0
	std::map<const char *, Behavior*, ltstr>::iterator it = _behavior.find(behav->getBehavName());
	if(it != _behavior.end()) {
		dbgprintf(LOG_CRITICAL, "Entity::AddBehavior(%s) - Already attached!", behav->getBehavName());
		delete behav;
        return NULL;
	}
#endif

	//Check for prereqs
    if(!behav->canAddTo(*static_cast<Entity*>(this)) || !behav->getBehavName()) {
		dbgprintf(LOG_CRITICAL, "Could not attach behavior!");
		delete behav;
        return nullptr;
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

LuaBehavior* EntityBase::getLuaBehavior(const char*script) const {
    return static_cast<LuaBehavior*>(getBehavior(script));
}

void EntityBase::removeLuaBehavior(const char *script) {
	removeBehavior(script);
}

Behavior* EntityBase::getBehavior(const char *behaviorName) const {
	std::map<const char *, Behavior*, ltstr>::const_iterator it = _behavior.find(behaviorName);
	if(it == _behavior.end()) {
		dbgprintf(LOG_CRITICAL, "Behavior::getBehavior(%s) - Not attached!", behaviorName);
        return nullptr;
	}
	else return it->second;
}

void EntityBase::removeBehavior(const char *behaviorName) {
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
    

bool EntityBase::attachComponent(Component *comp) {
    if(!comp->canAddTo(*static_cast<Entity*>(this))) {
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

template<typename T>
T* EntityBase::_addComponent(T*& comp) {
    if(comp) return nullptr;
    comp = new T(*static_cast<Entity*>(this));
    return comp = attachComponent(comp)? comp : nullptr;
}

Renderable* EntityBase::addRenderable(void) {
    return _addComponent<Renderable>(_renderable);
}

Animation *EntityBase::addAnimation(void) {
    return _addComponent<Animation>(_animation);
}

Collidable *EntityBase::addCollidable(void) {
    return _addComponent<Collidable>(_collidable);
}

RigidBody *EntityBase::addRigidBody(void) {
    return _addComponent<RigidBody>(_rigidBody);
}

Character *EntityBase::addCharacter(void) {
    return _addComponent<Character>(_character);
}

Warp *EntityBase::addWarp(void) {
    return _addComponent<Warp>(_warp);
}

Examinable *EntityBase::addExaminable(void) {
    return _addComponent<Examinable>(_examinable);
}

AudioSource *EntityBase::addAudioSource(void) {
    return _addComponent<AudioSource>(_audioSource);
}

LightSource *EntityBase::addLightSource(void) {
    return _addComponent<LightSource>(_lightSource);
}

ParticleEmitter *EntityBase::addParticleEmitter(void) {
    return _addComponent<ParticleEmitter>(_particleEmitter);
}

Combat *EntityBase::addCombat(void) {
    return _addComponent<Combat>(_combat);
}

Skeleton* EntityBase::addSkeleton(void) {
    //return _addComponent<Skeleton>(_renderable);
    return nullptr;
}

template<typename T>
void EntityBase::_removeComponent(T*& comp) {
    if(comp && _active) {
        comp->removeFromLevel();
        _componentActive[comp->getType()] = false;
    }
    delete comp;
    comp = nullptr;
}


void EntityBase::removeRenderable(void) {
    _removeComponent<Renderable>(_renderable);
}

void EntityBase::removeAnimation(void) {
    _removeComponent<Animation>(_animation);
}

void EntityBase::removeCollidable(void) {
    _removeComponent<Collidable>(_collidable);
}

void EntityBase::removeRigidBody(void) {
    _removeComponent<RigidBody>(_rigidBody);
}

void EntityBase::removeCharacter(void) {
    _removeComponent<Character>(_character);
}

void EntityBase::removeWarp(void) {
    _removeComponent<Warp>(_warp);
}

void EntityBase::removeExaminable(void) {
    _removeComponent<Examinable>(_examinable);
}

void EntityBase::removeAudioSource(void) {
    _removeComponent<AudioSource>(_audioSource);
}

void EntityBase::removeLightSource(void) {
    _removeComponent<LightSource>(_lightSource);
}

void EntityBase::removeParticleEmitter(void) {
    _removeComponent<ParticleEmitter>(_particleEmitter);
}

void EntityBase::removeCombat(void) {
    _removeComponent<Combat>(_combat);
}

void EntityBase::removeSkeleton(void) {

}

template<typename T>
void EntityBase::_setComponent(T*& comp, const OOLUA::Lua_table_ref valTable) {
    if(!comp) {
        comp = _addComponent<T>(comp);
    }
    comp->fromTable(valTable);
}

void EntityBase::setRenderable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Renderable>(_renderable, valTable);
}

void EntityBase::setAnimation(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Animation>(_animation, valTable);
}

void EntityBase::setCollidable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Collidable>(_collidable, valTable);
}

void EntityBase::setRigidBody(const OOLUA::Lua_table_ref valTable) {
    _setComponent<RigidBody>(_rigidBody, valTable);
}

void EntityBase::setCharacter(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Character>(_character, valTable);
}

void EntityBase::setWarp(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Warp>(_warp, valTable);
}

void EntityBase::setExaminable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Examinable>(_examinable, valTable);
}

void EntityBase::setLightSource(const OOLUA::Lua_table_ref valTable) {
    _setComponent<LightSource>(_lightSource, valTable);
}

void EntityBase::setAudioSource(const OOLUA::Lua_table_ref valTable) {
    _setComponent<AudioSource>(_audioSource, valTable);
}

void EntityBase::setParticleEmitter(const OOLUA::Lua_table_ref valTable) {
    _setComponent<ParticleEmitter>(_particleEmitter, valTable);
}

void EntityBase::setCombat(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Combat>(_combat, valTable);
}



}
