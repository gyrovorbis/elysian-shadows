#ifndef TK_PARTICLE_EMITTER_H
#define TK_PARTICLE_EMITTER_H

#include <component/elysian_particle_emitter_base.hpp>

class TkParticleEmitterAttrib: public elysian::ParticleEmitterAttribBase {
public:
    using       LuaTableProxy<elysian::ParticleEmitterAttribBase, ParticleEmitterAttrib, elysian::CollidableStaticAttrib, SpriteAttrib>::initializeDelegates;


};

class TkParticleEmitter: public elysian::ParticleEmitterBase {
public:

                    TkParticleEmitter(TkEntity& parent);
                    TkParticleEmitter(void) = default;

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;

};


inline TkParticleEmitter::TkParticleEmitter(TkEntity &parent): elysian::ParticleEmitterBase(parent) {}

#endif // TKPARTICLEEMITTER_H

