#include "elysian_particle_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_area.hpp"

namespace elysian {

ParticleSystem::ParticleSystem(void) {
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].setAttrib(new ParticleEmitterAttrib);
		_emitters[i].allocatePool(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
	}
}

void ParticleSystem::emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
	_emitters[index].emitFromPoint(pos, angle, intensity);
}

ParticleEmitterAttrib* ParticleSystem::getAttrib(const unsigned index) const {
	return _emitters[index].getAttrib();
}

void ParticleSystem::resizePool(const unsigned index, const unsigned size) {
	_emitters[index].allocatePool(size);
}

void ParticleSystem::update(void) {
	CameraState* state = getCameraSys()->getCurrentState();

	//Update internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].update(state);
	}

	//update entity emitters
	auto emitters = getCurArea()->particleEmitters;
	for(auto it : emitters) {
		it->update(state);
	}
}


void ParticleSystem::render(void) {

	//render internal emitters
	for(unsigned i = 0; i < ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_COUNT; ++i) {
		_emitters[i].render();
	}

	//render entity emitters (no, entities do this)

}


}
