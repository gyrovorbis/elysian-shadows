/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_PARTICLE_EMITTER_H
#define TK_PARTICLE_EMITTER_H

#include <AssetIO/component/elysian_particle_emitter_base.hpp>

class TkParticleEmitterAttrib: public elysian::ParticleEmitterAttribBase {
public:
    using       LuaTableProxy<elysian::ParticleEmitterAttribBase, ParticleEmitterAttrib, elysian::CollidableStaticAttrib, SpriteAttrib>::initializeDelegates;
    using       LuaTableProxy<elysian::ParticleEmitterAttribBase, ParticleEmitterAttrib, elysian::CollidableStaticAttrib, SpriteAttrib>::initializeVirtualTableDelegates;

    static void initializeDelegates(void);


};

class TkParticleEmitter: public elysian::ParticleEmitterBase {
public:
                    TkParticleEmitter(void) = default;
                    TkParticleEmitter(TkEntity& parent, TkParticleEmitter*const prefab=nullptr);

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;

};


inline TkParticleEmitter::TkParticleEmitter(TkEntity &parent, TkParticleEmitter*const prefab): elysian::ParticleEmitterBase(parent, prefab) {}


#endif // TKPARTICLEEMITTER_H

