#ifndef ELYSIAN_PARTICLE_EMITTER_HPP
#define ELYSIAN_PARTICLE_EMITTER_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_particle_emitter_base.hpp>
namespace elysian {
    typedef ParticleEmitterBase ParticleEmitter;
}
#else
#   include <tk_particle_emitter.h>
namespace elysian {
    typedef TkParticleEmitter ParticleEmitter;
}
typedef TkParticleEmitter ParticleEmitter;

#endif

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

