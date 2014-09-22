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
#include "elysian_area.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_examinable.hpp"
#include "elysian_audio_source.hpp"
#include "elysian_light_source.hpp"
#include "elysian_particle_emitter.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_combat.hpp"

using namespace std;

namespace elysian {

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

void Entity::render(void) {
#ifdef ELYSIAN_ENGINE
	if(_particleEmitter) _particleEmitter->render();
	if(!_renderable) return;
	gyMatPush();
    float oldZ = transform._size.z;
    if(_planeType == GY_RECT_PLANE_FRONT) {
		transform._size.z = transform._size.y;
		//transform._size.y = 400.0f;
	} else {
        //transform._pos.z += _shadowDepth;
	}

    if(_planeType == GY_RECT_PLANE_BOTTOM) {
        transform.apply();
    } else {

        gyMatTranslate(transform._pos.x, transform._pos.y, transform._pos.z);
        gyMatScale(transform._size.x*transform._scale.x, transform._size.y*transform._scale.y, transform._size.z*transform._scale.z);
        gyMatTranslate(0.0f, 0.0f, 0.5f);
        gyMatRotate(transform._orientZ, 0.0f, 1.0f, 0.0f);
        gyMatTranslate(0.0f, 0.0f, -0.5f);


    }


    _renderable->beginRender();
	//if(_depthType == DEPTH_BUFFERED) transform._size.y = transform._size.z;
	_renderable->render();
	_renderable->endRender();
	gyMatPop();
    transform._size.z = oldZ;
#endif
}

#if 0
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
#endif

void Entity::renderShadowVolume(void) {
#ifdef ELYSIAN_ENGINE
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
		transform._size.x*transform._scale.x*0.5f,
		_shadowDepth*0.5f,
		transform._size.y*transform._scale.y*0.5f
	};

    if(_planeType == GY_RECT_PLANE_FRONT) {
		const float shearFactor = halfSize.z/halfSize.y;

		gyMatTranslate(transform._pos.x,
					   transform._pos.y+shearFactor*halfSize.y, //move to bottom then move back up by half the depth
					   transform._pos.z);

		gyMatScale(halfSize.x, halfSize.y, halfSize.z);
		gyMatShearZ(0.0f, -shearFactor);
		//gyMatRotateZ(transform._orientZ);

	} else {

		gyMatTranslate(transform._pos.x,
					   transform._pos.y, //move to bottom then move back up by half the depth
					   transform._pos.z);

		gyMatRotateZ(-transform._orientZ);
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

#endif
}
    
#ifdef ELYSIAN_ENGINE
void Entity::move(const GYVector2& dist) {
    if(_character) _character->move(dist);
    else gyVec2AddAccum((GYVector2*)&transform._pos, &dist);
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

bool Entity::load(FILE* /*file*/) { //Deserialization
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

bool Entity::save(FILE* /*file*/) { //Serialization
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

Behavior* Entity::addBehavior(const char *name) {
	return attachBehavior(Behavior::create(name, *this));
}

#ifdef ELYSIAN_ENGINE
LuaBehavior* Entity::addLuaBehavior(const char *name, const OOLUA::Lua_table_ref paramTable) {
	return static_cast<LuaBehavior*>(attachBehavior(new LuaBehavior(*this, name, paramTable)));
}
#endif
     
Behavior *Entity::attachBehavior(Behavior *behav) {
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

RigidBody *Entity::addRigidBody(void) {
	if(_rigidBody) return nullptr;
	_rigidBody = new RigidBody(*this);
	return _rigidBody = attachComponent(_rigidBody)? _rigidBody : nullptr;
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

LightSource *Entity::addLightSource(void) {
	if(_lightSource) return nullptr;
	_lightSource = new LightSource(*this);
	return _lightSource = attachComponent(_lightSource)? _lightSource : NULL;
}

ParticleEmitter *Entity::addParticleEmitter(void) {
	if(_particleEmitter) return nullptr;
	_particleEmitter = new ParticleEmitter(*this);
	return _particleEmitter = attachComponent(_particleEmitter)? _particleEmitter : NULL;
}

Combat *Entity::addCombat(void) {
	if(_combat) return nullptr;
	_combat = new Combat(*this); //memory leak..... dumbfuck.
	return _combat = attachComponent(_combat)? _combat : nullptr;
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

void Entity::removeRigidBody(void) {
	if(_rigidBody && _active) {
		_rigidBody->removeFromLevel();
		_componentActive[Component::RIGID_BODY] = false;
	}
	delete _rigidBody;
	_rigidBody = nullptr;
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

void Entity::removeLightSource(void) {
	if(_lightSource && _active) {
		_lightSource->removeFromLevel();
		_componentActive[Component::LIGHT_SOURCE] = false;
	}
	delete _lightSource;
	_lightSource = nullptr;
}

void Entity::removeParticleEmitter(void) {
	if(_particleEmitter && _active) {
		_particleEmitter->removeFromLevel();
		_componentActive[Component::PARTICLE_EMITTER] = false;
	}
	delete _particleEmitter;
	_particleEmitter = nullptr;
}

void Entity::removeCombat(void) {
	if(_combat && _active) {
		_combat->removeFromLevel();
		_componentActive[Component::COMBAT] = false;
	}
	delete _combat;
	_combat = nullptr;
}

}
