#include <algorithm>
#include "elysian_component_base.hpp"
#include "elysian_behavior.hpp"
#include "elysian_entity.hpp"
#include <elysian_renderable.hpp>
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
#include <elysian_generic.hpp>

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
    dbgprintf("Entity::Reested(%s) - Dereesting entity!", _name);
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
    if(_active) return true; //already active
    if(!_area) return false; //not even in an area

    containerEraseOnMatch(static_cast<Entity*>(this), _area->inactiveEntities);
    _area->entities.push_back(static_cast<Entity*>(this));

	if(_renderable && !_componentActive[Component::RENDERABLE]) {
        _area->renderables.push_back(_renderable);
		_componentActive[Component::RENDERABLE] = true;
	}
	if(_animation && !_componentActive[Component::ANIMATION]) {
        _area->animations.push_back(_animation);
		_componentActive[Component::ANIMATION] = true;
	}
	if(_collidable && !_componentActive[Component::COLLIDABLE]) {
        _area->collidables.push_back(_collidable);
		_componentActive[Component::COLLIDABLE] = true;
	}
	if(_rigidBody && !_componentActive[Component::RIGID_BODY]) {
        _area->rigidBodies.push_back(_rigidBody);
		_componentActive[Component::RIGID_BODY] = true;
	}
	if(_character && !_componentActive[Component::CHARACTER]) {
        _area->characters.push_back(_character);
		_componentActive[Component::CHARACTER] = true;
	}
	if(_warp && !_componentActive[Component::WARP]) {
        _area->warps.push_back(_warp);
		_componentActive[Component::WARP] = true;
	}
	if(_examinable && !_componentActive[Component::EXAMINABLE]) {
        _area->examinables.push_back(_examinable);
		_componentActive[Component::EXAMINABLE] = true;
	}
	if(_audioSource && !_componentActive[Component::AUDIO_SOURCE]) {
        _area->audioSources.push_back(_audioSource);
		_componentActive[Component::AUDIO_SOURCE] = true;
	}
	if(_lightSource && !_componentActive[Component::LIGHT_SOURCE]) {
        _area->lightSources.push_back(_lightSource);
		_componentActive[Component::LIGHT_SOURCE] = true;
	}
	if(_particleEmitter && !_componentActive[Component::PARTICLE_EMITTER]) {
        _area->particleEmitters.push_back(_particleEmitter);
		_componentActive[Component::PARTICLE_EMITTER] = true;
	}
	if(_combat && !_componentActive[Component::COMBAT]) {
        _area->combats.push_back(_combat);
		_componentActive[Component::COMBAT] = true;
	}

	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(!it->second->getActive()) {
            _area->behaviors.push_back(it->second);
			it->second->setActive();
		}
	}

    _active = true;
    return true;
}

bool EntityBase::deactivate(void) {
    if(!_active) return true; //already inactive
    if(!_area) return false; //not even in an area

    containerEraseOnMatch(static_cast<Entity*>(this), _area->entities);
    _area->inactiveEntities.push_back(static_cast<Entity*>(this));
	
	if(_renderable && _componentActive[Component::RENDERABLE]) {
        containerEraseOnMatch(_renderable, _area->renderables);
		_componentActive[Component::RENDERABLE] = false;
	}
	if(_animation && _componentActive[Component::ANIMATION]) { 
        containerEraseOnMatch(_animation, _area->animations);
		_componentActive[Component::ANIMATION] = false;
	}
	if(_collidable && _componentActive[Component::COLLIDABLE]) {
        containerEraseOnMatch(_collidable, _area->collidables);
		_componentActive[Component::COLLIDABLE] = false;
	}
	if(_rigidBody && _componentActive[Component::RIGID_BODY]) {
        containerEraseOnMatch(_rigidBody, _area->rigidBodies);
		_componentActive[Component::RIGID_BODY] = false;
	}
	if(_character && _componentActive[Component::CHARACTER]) {
        containerEraseOnMatch(_character, _area->characters);
		_componentActive[Component::CHARACTER] = false;
	}
	if(_warp && _componentActive[Component::WARP]) {
        containerEraseOnMatch(_warp, _area->warps);
		_componentActive[Component::WARP] = false;
	}
	if(_examinable && _componentActive[Component::EXAMINABLE]) {
        containerEraseOnMatch(_examinable, _area->examinables);
		_componentActive[Component::EXAMINABLE] = false;
	}
	if(_audioSource && _componentActive[Component::AUDIO_SOURCE]) {
        containerEraseOnMatch(_audioSource, _area->audioSources);
		_componentActive[Component::AUDIO_SOURCE] = false;
	}
	if(_lightSource && _componentActive[Component::LIGHT_SOURCE]) {
        containerEraseOnMatch(_lightSource, _area->lightSources);
		_componentActive[Component::LIGHT_SOURCE] = false;
	}
	if(_particleEmitter && _componentActive[Component::PARTICLE_EMITTER]) {
        containerEraseOnMatch(_particleEmitter, _area->particleEmitters);
		_componentActive[Component::PARTICLE_EMITTER] = false;
	}
	if(_combat && _componentActive[Component::COMBAT]) {
        containerEraseOnMatch(_combat, _area->combats);
		_componentActive[Component::COMBAT] = false;
	}

	for(auto it = _behavior.begin(); it != _behavior.end(); ++it) {
		if(it->second->getActive()) {
            containerEraseOnMatch(it->second, _area->behaviors);
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
    if(/*behav->canAddTo(*static_cast<Entity*>(this)) ||*/ !behav->getBehavName()) {
        Debug::printf(Debug::CRITICAL, "Could not attach behavior!");
		delete behav;
        return nullptr;
	}
	
	//Attach using its own typeName as a key (so that it never leaves scope), fixing the reast mentioned above.
	_behavior[behav->getBehavName()] = behav;
	
	//add to level
    if(_active) {
        _area->behaviors.push_back(behav);
		behav->setActive();
	}
	
	return behav;
}

LuaBehavior* EntityBase::getLuaBehavior(const char*script) const {
    return static_cast<LuaBehavior*>(getBehavior(script));
}

LuaBehavior* EntityBase::getLuaBehaviorByIndex(const unsigned index) const {
    unsigned count = 0;
    for(auto behav : _behavior) {
        if(count++ == index) return static_cast<LuaBehavior*>(behav.second);
    }
    return nullptr;
}

void EntityBase::setLuaBehaviorByIndex(const unsigned index, const OOLUA::Lua_table_ref valueTable) {
    unsigned count = 0;
    LuaBehavior* behavior;
    for(auto behav : _behavior) {
        if(count++ == index) {
            behavior = static_cast<LuaBehavior*>(behav.second);
            break;
        }
    }

    if(index >= _behavior.size()) {
        Debug::printf(Debug::VERBOSE, "INSERTING");
    } else {
        Debug::printf(Debug::VERBOSE, "UPDATING");
        behavior->fromTable(valueTable);
    }
}

void EntityBase::removeLuaBehavior(const char *script) {
	removeBehavior(script);
}

Behavior* EntityBase::getBehavior(const char *behaviorName) const {
	std::map<const char *, Behavior*, ltstr>::const_iterator it = _behavior.find(behaviorName);
	if(it == _behavior.end()) {
        Debug::printf(Debug::CRITICAL, "Behavior::getBehavior(%s) - Not attached!", behaviorName);
        return nullptr;
	}
	else return it->second;
}

void EntityBase::removeBehavior(const char *behaviorName) {
	std::map<const char *, Behavior*, ltstr>::iterator it = _behavior.find(behaviorName);
	if(it == _behavior.end()) {
        Debug::printf(Debug::CRITICAL, "Behavior::RemoveBehavior(%s) - Not attached!", behaviorName);
		return;
	}
	
	//remove component from area if not null
	if(_active) {
        containerEraseOnMatch(it->second, _area->behaviors);
		it->second->setActive(false);
	}
	delete it->second;
	_behavior.erase(it);
}
    

template<typename T>
T* EntityBase::_addComponent(T*& comp, std::vector<T*>* activeList) {
    if(comp) return comp;
    //if(!T::canAddTo(*static_cast<Entity*>(this))) return nullptr;
    comp = new T(*static_cast<Entity*>(this));
    if(_active) {
        activeList->push_back(comp);
        _componentActive[T::componentType] = true;
    }
    return comp;
}



Renderable* EntityBase::addRenderable(void) {
    return _addComponent<Renderable>(_renderable, _active? &_area->renderables : nullptr);
}

Animation *EntityBase::addAnimation(void) {
    return _addComponent<Animation>(_animation, _active? &_area->animations : nullptr);
}

Collidable *EntityBase::addCollidable(void) {
    return _addComponent<Collidable>(_collidable, _active? &_area->collidables : nullptr);
}

RigidBody *EntityBase::addRigidBody(void) {
    return _addComponent<RigidBody>(_rigidBody, _active? &_area->rigidBodies : nullptr);
}

Character *EntityBase::addCharacter(void) {
    return _addComponent<Character>(_character, _active? &_area->characters : nullptr);
}

Warp *EntityBase::addWarp(void) {
    return _addComponent<Warp>(_warp, _active? &_area->warps : nullptr);
}

Examinable *EntityBase::addExaminable(void) {
    return _addComponent<Examinable>(_examinable, _active? &_area->examinables : nullptr);
}

AudioSource *EntityBase::addAudioSource(void) {
    return _addComponent<AudioSource>(_audioSource, _active? &_area->audioSources : nullptr);
}

LightSource *EntityBase::addLightSource(void) {
    return _addComponent<LightSource>(_lightSource, _active? &_area->lightSources : nullptr);
}

ParticleEmitter *EntityBase::addParticleEmitter(void) {
    return _addComponent<ParticleEmitter>(_particleEmitter, _active? &_area->particleEmitters : nullptr);
}

Combat *EntityBase::addCombat(void) {
    return _addComponent<Combat>(_combat, _active? &_area->combats : nullptr);
}

Skeleton* EntityBase::addSkeleton(void) {
    //return _addComponent<Skeleton>(_renderable);
    return nullptr;
}

template<typename T>
void EntityBase::_removeComponent(T*& comp, std::vector<T*>* container) {
    if(comp && _active) {
        containerEraseOnMatch(comp, *container);
        _componentActive[T::componentType] = false;
    }
    delete comp;
    comp = nullptr;
}


void EntityBase::removeRenderable(void) {
    _removeComponent<Renderable>(_renderable, _active? &_area->renderables : nullptr);
}

void EntityBase::removeAnimation(void) {
    _removeComponent<Animation>(_animation, _active? &_area->animations : nullptr);
}

void EntityBase::removeCollidable(void) {
    _removeComponent<Collidable>(_collidable, _active? &_area->collidables : nullptr);
}

void EntityBase::removeRigidBody(void) {
    _removeComponent<RigidBody>(_rigidBody, _active? &_area->rigidBodies : nullptr);
}

void EntityBase::removeCharacter(void) {
    _removeComponent<Character>(_character, _active? &_area->characters : nullptr);
}

void EntityBase::removeWarp(void) {
    _removeComponent<Warp>(_warp, _active? &_area->warps : nullptr);
}

void EntityBase::removeExaminable(void) {
    _removeComponent<Examinable>(_examinable, _active? &_area->examinables : nullptr);
}

void EntityBase::removeAudioSource(void) {
    _removeComponent<AudioSource>(_audioSource, _active? &_area->audioSources : nullptr);
}

void EntityBase::removeLightSource(void) {
    _removeComponent<LightSource>(_lightSource, _active? &_area->lightSources : nullptr);
}

void EntityBase::removeParticleEmitter(void) {
    _removeComponent<ParticleEmitter>(_particleEmitter, _active? &_area->particleEmitters : nullptr);
}

void EntityBase::removeCombat(void) {
    _removeComponent<Combat>(_combat, _active? &_area->combats : nullptr);
}

void EntityBase::removeSkeleton(void) {

}

template<typename T>
void EntityBase::_setComponent(T*& comp, const OOLUA::Lua_table_ref valTable, std::vector<T*>* container) {
    if(!comp) {
        comp = _addComponent<T>(comp, container);
    }
    comp->fromTable(valTable);
}

void EntityBase::setRenderable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Renderable>(_renderable, valTable, _active? &_area->renderables : nullptr);
}

void EntityBase::setAnimation(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Animation>(_animation, valTable, _active? &_area->animations : nullptr);
}

void EntityBase::setCollidable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Collidable>(_collidable, valTable, _active? &_area->collidables : nullptr);
}

void EntityBase::setRigidBody(const OOLUA::Lua_table_ref valTable) {
    _setComponent<RigidBody>(_rigidBody, valTable, _active? &_area->rigidBodies : nullptr);
}

void EntityBase::setCharacter(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Character>(_character, valTable, _active? &_area->characters : nullptr);
}

void EntityBase::setWarp(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Warp>(_warp, valTable, _active? &_area->warps : nullptr);
}

void EntityBase::setExaminable(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Examinable>(_examinable, valTable, _active? &_area->examinables : nullptr);
}

void EntityBase::setLightSource(const OOLUA::Lua_table_ref valTable) {
    _setComponent<LightSource>(_lightSource, valTable, _active? &_area->lightSources : nullptr);
}

void EntityBase::setAudioSource(const OOLUA::Lua_table_ref valTable) {
    _setComponent<AudioSource>(_audioSource, valTable, _active? &_area->audioSources : nullptr);
}

void EntityBase::setParticleEmitter(const OOLUA::Lua_table_ref valTable) {
    _setComponent<ParticleEmitter>(_particleEmitter, valTable, _active? &_area->particleEmitters : nullptr);
}

void EntityBase::setCombat(const OOLUA::Lua_table_ref valTable) {
    _setComponent<Combat>(_combat, valTable, _active? &_area->combats : nullptr);
}



}
