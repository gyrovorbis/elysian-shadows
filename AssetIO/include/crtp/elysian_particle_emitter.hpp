#ifndef ELYSIAN_PARTICLE_EMITTER_HPP
#define ELYSIAN_PARTICLE_EMITTER_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_particle_emitter_base.hpp>
namespace elysian {
    typedef ParticleEmitterAttribBase   ParticleEmitterAttrib;
    typedef ParticleEmitterBase         ParticleEmitter;
}
#else
#   include <AssetIO/tk_particle_emitter.h>
namespace elysian {
    typedef TkParticleEmitterAttrib ParticleEmitterAttrib;
    typedef TkParticleEmitter       ParticleEmitter;
}
typedef TkParticleEmitterAttrib ParticleEmitterAttrib;
typedef TkParticleEmitter       ParticleEmitter;

#endif

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

