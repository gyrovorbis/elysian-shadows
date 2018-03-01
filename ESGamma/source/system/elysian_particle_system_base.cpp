#include <crtp/elysian_particle_system.hpp>
#include <crtp/elysian_camera_system.hpp>
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_force_generator.hpp"
#include <crtp/elysian_area.hpp>

namespace elysian {

ParticleSystemBase::ParticleSystemBase(unsigned emitterCount):
    _emitterCount(emitterCount)
{
    _emitters       = new ParticleEmitterInternals[emitterCount];
    _defaultAttribs = new std::shared_ptr<ParticleEmitterAttrib>[emitterCount];

    for(unsigned i = 0; i < emitterCount; ++i) {
        _defaultAttribs[i] = std::make_shared<ParticleEmitterAttrib>();
        _emitters[i].setAttrib(_defaultAttribs[i]);
        _emitters[i].resizePool(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
	}
}

ParticleSystemBase::~ParticleSystemBase(void) {
    delete[] _defaultAttribs;
    delete[] _emitters;
}

void ParticleSystemBase::emitFromPoint(const unsigned index, const GYVector3& pos, const float angle, const unsigned intensity) {
    GY_ASSERT(index < _emitterCount);
    _emitters[index].emitFromPoint(pos, angle, intensity);
}


void ParticleSystemBase::resizePool(const unsigned index, const unsigned size) {
    GY_ASSERT(index < _emitterCount);
    _emitters[index].resizePool(size);
}

void ParticleSystemBase::update(void) {
	const float deltaTime = getTimerSys()->getDeltaTime();
    Camera* state = getCameraSys()->getCurrentState();

    //Update internal emitters
    for(unsigned i = 0; i < _emitterCount; ++i) {
        _emitters[i].update(state, deltaTime);
	}

    //update entity emitters
    auto emitters = getCurrentArea()->particleEmitters;
    for(auto it : emitters) {
		it->update(state, deltaTime);
	}
}

void ParticleSystemBase::applyForce(ForceGenerator * const forceGen) {
	const float deltaTime = getTimerSys()->getDeltaTime();

    //Update internal emitters
    for(unsigned i = 0; i < _emitterCount; ++i) {
        if(forceGen->getForceMask().getValue() & _emitters[i].getAttrib()->getForceMask().getValue())
            _emitters[i].applyForce(forceGen, deltaTime);
	}

    //update entity emitters
    auto emitters = getCurrentArea()->particleEmitters;
    for(auto it : emitters) {
        if(forceGen->getForceMask().getValue() & it->getAttrib()->getForceMask().getValue())
            it->applyForce(forceGen, deltaTime);
	}
}


void ParticleSystemBase::render(void) {

    //render internal emitters
    for(unsigned i = 0; i < _emitterCount; ++i) {
        _emitters[i].render();
	}

    //render entity emitters (no, entities do this)
}


void ParticleSystemBase::renderAtElev(const unsigned elev) {
    //render internal emitters
    for(unsigned i = 0; i < _emitterCount; ++i) {
        _emitters[i].renderAtElev(elev);
	}

}


}
