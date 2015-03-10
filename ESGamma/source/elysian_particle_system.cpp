#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_area.hpp"

namespace elysian {

ParticleSystem::ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_defaultAttribs[i] = new ParticleEmitterAttrib;
		_Q_EMITters[i].setAttrib(_defaultAttribs[i]);
		_Q_EMITters[i].allocatePool(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
	}
}

ParticleSystem::~ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) delete _defaultAttribs[i];
}

void ParticleSystem::Q_EMITFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
	_Q_EMITters[index].Q_EMITFromPoint(pos, angle, intensity);
}


void ParticleSystem::resizePool(const unsigned index, const unsigned size) {
	_Q_EMITters[index].allocatePool(size);
}

void ParticleSystem::update(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
	CameraState* state = getCameraSys()->getCurrentState();

	//Update internal Q_EMITters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_Q_EMITters[i].update(state, deltaTime);
	}

	//update entity Q_EMITters
	auto Q_EMITters = getCurArea()->particleEmitters;
	for(auto it : Q_EMITters) {
		it->update(state, deltaTime);
	}
}

void ParticleSystem::applyForce(ForceGenerator * const forceGen) {
	const float deltaTime = getTimerSys()->getDeltaTime();

	//Update internal Q_EMITters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		if(forceGen->getForceMask() & _Q_EMITters[i].getAttrib()->getForceMask()) _Q_EMITters[i].applyForce(forceGen, deltaTime);
	}

	//update entity Q_EMITters
	auto Q_EMITters = getCurArea()->particleEmitters;
	for(auto it : Q_EMITters) {
		if(forceGen->getForceMask() & it->getAttrib()->getForceMask()) it->applyForce(forceGen, deltaTime);
	}
}


void ParticleSystem::render(void) {

	//render internal Q_EMITters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_Q_EMITters[i].render();
	}

	//render entity Q_EMITters (no, entities do this)
}


void ParticleSystem::renderAtElev(const unsigned elev) {
	//render internal Q_EMITters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_Q_EMITters[i].renderAtElev(elev);
	}

}


}
