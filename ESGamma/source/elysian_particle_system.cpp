#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_force_generator.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")

namespace elysian {

ParticleSystem::ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_defaultAttribs[i] = new ParticleEmitterAttrib;
		_emitters[i].setAttrib(_defaultAttribs[i]);
		_emitters[i].allocatePool(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
	}
}

ParticleSystem::~ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) delete _defaultAttribs[i];
}

void ParticleSystem::emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
	_emitters[index].emitFromPoint(pos, angle, intensity);
}


void ParticleSystem::resizePool(const unsigned index, const unsigned size) {
	_emitters[index].allocatePool(size);
}

void ParticleSystem::update(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	CameraState* state = getCameraSys()->getCurrentState();

	//Update internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].update(state, deltaTime);
	}

	//update entity emitters
	auto emitters = getCurArea()->particleEmitters;
	for(auto it : emitters) {
		it->update(state, deltaTime);
	}
}

void ParticleSystem::applyForce(ForceGenerator * const forceGen) {
	const float deltaTime = getTimerSys()->getDeltaTime();

	//Update internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		if(forceGen->getForceMask() & _emitters[i].getAttrib()->getForceMask()) _emitters[i].applyForce(forceGen, deltaTime);
	}

	//update entity emitters
	auto emitters = getCurArea()->particleEmitters;
	for(auto it : emitters) {
		if(forceGen->getForceMask() & it->getAttrib()->getForceMask()) it->applyForce(forceGen, deltaTime);
	}
}


void ParticleSystem::render(void) {

	//render internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].render();
	}

	//render entity emitters (no, entities do this)
}


void ParticleSystem::renderAtElev(const unsigned elev) {
	//render internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].renderAtElev(elev);
	}

}


}
