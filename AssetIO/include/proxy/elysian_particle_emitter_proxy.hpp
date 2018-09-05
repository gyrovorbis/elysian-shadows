#ifndef ELYSIAN_PARTICLE_EMITTER_PROXY_HPP
#define ELYSIAN_PARTICLE_EMITTER_PROXY_HPP

#include <AssetIO/crtp/elysian_particle_emitter.hpp>
#include <AssetIO/crtp/elysian_sprite_attrib.hpp>
#include <AssetIO/crtp/elysian_collidable.hpp>
#include <AssetIO/primitive/elysian_color_base.hpp>

namespace OOLUA {
    typedef elysian::ParticleEmitterAttrib      ParticleEmitterAttrib;
    typedef elysian::ParticleEmitter            ParticleEmitter;
    typedef elysian::SpriteAttrib               SpriteAttrib;
    typedef elysian::CollidableStaticAttrib     CollidableStaticAttrib;
    typedef elysian::Color4                     Color4;

    //template<>                          class Proxy_class<elysian::SpriteAttrib>;
    //template<>                          class Proxy_class<elysian::CollidableStaticAttrib>;
    //template<>                          class Proxy_class<elysian::ParticleEmitterAttrib>;
    template<>                          class Proxy_class<elysian::ParticleEmitter>;

}

OOLUA_PROXY(ParticleEmitterAttrib, SpriteAttrib, CollidableStaticAttrib)
    OOLUA_TAGS(Shared)
    OOLUA_MFUNC(setAcceleration)
    OOLUA_MFUNC(setMaxLifeTime)
    OOLUA_MEM_FUNC(void, setForceMask, BitMask32)
    OOLUA_MEM_FUNC(void, setIntensityFactor, float)
    OOLUA_MEM_FUNC(void, setStartSize, unsigned)
    OOLUA_MEM_FUNC(void, setEndSize, unsigned)
    OOLUA_MEM_FUNC(void, setEndColor, const Color4&)
    OOLUA_MEM_FUNC(void, setAnimGridSize, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, setAnimGridIndex, unsigned, unsigned, int)
    OOLUA_MEM_FUNC(void, setAnimFrameDuration, float)
    OOLUA_MEM_FUNC(void, setStick, bool)
    OOLUA_MEM_FUNC(void, setRandomStartFrame, bool)
    OOLUA_MEM_FUNC(void, setShadowVolume, bool)
    OOLUA_MEM_FUNC(Color4, getEndColor)
    OOLUA_MEM_FUNC_CONST(bool, getShadowVolume)
    OOLUA_MEM_FUNC_CONST(bool, getRandomStartFrame)
    OOLUA_MEM_FUNC_CONST(bool, getStick)
    OOLUA_MEM_FUNC_CONST(float, getAnimFrameDuration)
    OOLUA_MEM_FUNC_CONST(unsigned, getEndSize)
    OOLUA_MEM_FUNC_CONST(unsigned, getStartSize)
    OOLUA_MEM_FUNC_CONST(float, getIntensityFactor)
    OOLUA_MEM_FUNC(BitMask32&, getForceMask)
    OOLUA_MEM_FUNC_CONST(float, getMaxLifeTime)
OOLUA_PROXY_END

OOLUA_PROXY(ParticleEmitter, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, resizePool, unsigned)
    OOLUA_MEM_FUNC(void, setAttrib, std::shared_ptr<ParticleEmitterAttrib>)
    OOLUA_MEM_FUNC_CONST(maybe_null<std::shared_ptr<ParticleEmitterAttrib>>, getAttrib)
    OOLUA_MEM_FUNC(void, emitFromPoint, const GYVector3&, float, unsigned)
    OOLUA_MEM_FUNC(void, emitFromEntity, const GYVector3&, float, unsigned)
    OOLUA_MEM_FUNC(void, emitFromPointInDir, const GYVector3&, const GYVector3&, unsigned, float)
OOLUA_PROXY_END

#endif // ELYSIAN_PARTICLE_EMITTER_PROXY_HPP

