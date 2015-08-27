#ifndef TK_PARTICLE_EMITTER_H
#define TK_PARTICLE_EMITTER_H

#include <component/elysian_particle_emitter_base.hpp>

class TkParticleEmitter: public elysian::ParticleEmitterBase {
public:

    using       LuaTableProxy<ParticleEmitterAttrib, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::initializeDelegates;

                    TkParticleEmitter(TkEntity& parent);
                    TkParticleEmitter(void) = default;

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;

};


inline TkParticleEmitter::TkParticleEmitter(TkEntity &parent): elysian::ParticleEmitterBase(parent) {}

#endif // TKPARTICLEEMITTER_H

