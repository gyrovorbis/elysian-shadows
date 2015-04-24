#ifndef TKPARTICLEEMITTER_H
#define TKPARTICLEEMITTER_H

#include <elysian_particle_emitter_base.hpp>

class TkParticleEmitter: public elysian::ParticleEmitterBase {
public:

    TkParticleEmitter(TkEntity& parent);
    TkParticleEmitter(void) = default;

};


inline TkParticleEmitter::TkParticleEmitter(TkEntity &parent): elysian::ParticleEmitterBase(parent) {}

#endif // TKPARTICLEEMITTER_H

