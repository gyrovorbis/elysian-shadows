#ifndef ELYSIAN_ENTITY_PROXY_HPP
#define ELYSIAN_ENTITY_PROXY_HPP

#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/proxy/elysian_instance_transform_proxy.hpp>
#include <AssetIO/proxy/elysian_primitive_proxy.hpp>
#include <AssetIO/proxy/elysian_collidable_proxy.hpp>
#include <ESGamma/proxy/elysian_event_monitor_proxy.hpp>
#include <AssetIO/proxy/elysian_renderable_proxy.hpp>
#include <AssetIO/proxy/elysian_warp_proxy.hpp>
#include <AssetIO/proxy/elysian_rigid_body_proxy.hpp>
#include <AssetIO/proxy/elysian_character_proxy.hpp>
#include <AssetIO/proxy/elysian_audio_source_proxy.hpp>
#include <AssetIO/proxy/elysian_light_source_proxy.hpp>
#include <AssetIO/proxy/elysian_examinable_proxy.hpp>
#include <AssetIO/proxy/elysian_particle_emitter_proxy.hpp>
#include <AssetIO/proxy/elysian_combat_proxy.hpp>
#include <AssetIO/proxy/elysian_skeleton_proxy.hpp>
#include <AssetIO/proxy/elysian_lua_behavior_proxy.hpp>


namespace OOLUA {
    typedef elysian::InstanceTransform  InstanceTransform;
    typedef elysian::Skeleton           Skeleton;
    typedef elysian::Renderable         Renderable;
    typedef elysian::Collidable         Collidable;
    typedef elysian::Warp               Warp;
    typedef elysian::RigidBody          RigidBody;
    typedef elysian::LightSource        LightSource;
    typedef elysian::Character          Character;
    typedef elysian::AudioSource        AudioSource;
    typedef elysian::Examinable         Examinable;
    typedef elysian::Combat             Combat;
    typedef elysian::ParticleEmitter    ParticleEmitter;
    typedef elysian::Behavior           Behavior;
    typedef elysian::LuaBehavior        LuaBehavior;
    typedef elysian::RenderableObject   RenderableObject;
    typedef elysian::Entity             Entity;
    typedef elysian::Component          Component;
    typedef elysian::LuaVariant         LuaVariant;

    template<>                          class Proxy_class<elysian::Entity>;
}


/*
OOLUA_PROXY(LuaVariant)
OOLUA_PROXY_END
*/

OOLUA_PROXY(RenderableObject)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(InstanceTransform&, getTransform)
OOLUA_PROXY_END

namespace OOLUA {
    template<> class Proxy_class<elysian::Renderable>;
    template<> class Proxy_class<elysian::Collidable>;
    template<> class Proxy_class<elysian::RigidBody>;
    template<> class Proxy_class<elysian::Character>;
    template<> class Proxy_class<elysian::Warp>;
    template<> class Proxy_class<elysian::AudioSource>;
    template<> class Proxy_class<elysian::LightSource>;
    template<> class Proxy_class<elysian::Examinable>;
    template<> class Proxy_class<elysian::ParticleEmitter>;
    template<> class Proxy_class<elysian::Combat>;
    template<> class Proxy_class<elysian::Behavior>;
    template<> class Proxy_class<elysian::LuaBehavior>;
    template<> class Proxy_class<elysian::Skeleton>;
}
OOLUA_PROXY(Entity, RenderableObject)
    OOLUA_CTORS(
        OOLUA_CTOR(Entity*)
    )
    OOLUA_MEM_FUNC(void, move, const GYVector2&)
    OOLUA_MEM_FUNC(void, setName, const char*)
    OOLUA_MEM_FUNC_CONST(const char *, getName)
    OOLUA_MEM_FUNC(void, setElev, const unsigned char)
    OOLUA_MEM_FUNC_CONST(unsigned char, getElev)
//	OOLUA_MFUNC_CONST(getNormalType)
//	OOLUA_MFUNC(setNormalType)
    OOLUA_MEM_FUNC(maybe_null<Renderable*>, addRenderable)
    OOLUA_MEM_FUNC(maybe_null<Collidable*>, addCollidable)
    OOLUA_MEM_FUNC(maybe_null<RigidBody*>, addRigidBody)
    OOLUA_MEM_FUNC(maybe_null<Character*>, addCharacter)
    OOLUA_MEM_FUNC(maybe_null<Warp*>, addWarp)
    OOLUA_MEM_FUNC(maybe_null<AudioSource*>, addAudioSource)
    OOLUA_MEM_FUNC(maybe_null<LightSource*>, addLightSource)
    OOLUA_MEM_FUNC(maybe_null<Examinable*>, addExaminable)
    OOLUA_MEM_FUNC(maybe_null<Combat*>, addCombat)
    OOLUA_MEM_FUNC(maybe_null<ParticleEmitter*>, addParticleEmitter)
    OOLUA_MEM_FUNC(maybe_null<Skeleton*>, addSkeleton)
    OOLUA_MEM_FUNC(maybe_null<Behavior*>, addBehavior, Behavior*)
    OOLUA_MEM_FUNC(maybe_null<LuaBehavior*>, addLuaBehavior, const char*, OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC_CONST(maybe_null<Renderable*>, getRenderable)
    OOLUA_MEM_FUNC_CONST(maybe_null<Collidable*>, getCollidable)
    OOLUA_MEM_FUNC_CONST(maybe_null<RigidBody*>, getRigidBody)
    OOLUA_MEM_FUNC_CONST(maybe_null<Character*>, getCharacter)
    OOLUA_MEM_FUNC_CONST(maybe_null<Warp*>, getWarp)
    OOLUA_MEM_FUNC_CONST(maybe_null<AudioSource*>, getAudioSource)
    OOLUA_MEM_FUNC_CONST(maybe_null<LightSource*>, getLightSource)
    OOLUA_MEM_FUNC_CONST(maybe_null<Examinable*>, getExaminable)
    OOLUA_MEM_FUNC_CONST(maybe_null<Combat*>, getCombat)
    OOLUA_MEM_FUNC_CONST(maybe_null<ParticleEmitter*>, getParticleEmitter)
    OOLUA_MEM_FUNC_CONST(maybe_null<Skeleton*>, getSkeleton)
    OOLUA_MEM_FUNC_CONST(maybe_null<Behavior*>, getBehavior, const char*)
    OOLUA_MEM_FUNC_CONST(maybe_null<LuaBehavior*>, getLuaBehavior, const char*)
    OOLUA_MEM_FUNC(void, removeRenderable)
    OOLUA_MEM_FUNC(void, removeCollidable)
    OOLUA_MEM_FUNC(void, removeRigidBody)
    OOLUA_MEM_FUNC(void, removeCharacter)
    OOLUA_MEM_FUNC(void, removeWarp)
    OOLUA_MEM_FUNC(void, removeAudioSource)
    OOLUA_MEM_FUNC(void, removeLightSource)
    OOLUA_MEM_FUNC(void, removeExaminable)
    OOLUA_MEM_FUNC(void, removeCombat)
    OOLUA_MEM_FUNC(void, removeParticleEmitter)
    OOLUA_MEM_FUNC(void, removeSkeleton)
    OOLUA_MEM_FUNC(void, removeBehavior, const char*)
    OOLUA_MEM_FUNC(void, removeLuaBehavior, const char*)
    OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
    OOLUA_MEM_FUNC(void, setSelfTable, Lua_table_ref)
    OOLUA_MEM_FUNC(void, setShadowDepth, float)
    OOLUA_MEM_FUNC(void, setShadowVolume, bool)
    OOLUA_MEM_FUNC(void, setPlaneType, GY_RECT_PLANE_TYPE)
    OOLUA_MEM_FUNC_CONST(GY_RECT_PLANE_TYPE, getPlaneType)
    OOLUA_MEM_FUNC_CONST(float, getShadowDepth)
    OOLUA_MEM_FUNC_CONST(bool, isShadowVolume)
    OOLUA_MEM_FUNC(void, setTransform, LuaVariant)
    OOLUA_MEM_FUNC(void, setRenderable, LuaVariant)
    OOLUA_MEM_FUNC(void, setCollidable, LuaVariant)
    OOLUA_MEM_FUNC(void, setRigidBody, LuaVariant)
    OOLUA_MEM_FUNC(void, setWarp, LuaVariant)
    OOLUA_MEM_FUNC(void, setCharacter, LuaVariant)
    OOLUA_MEM_FUNC(void, setAudioSource, LuaVariant)
    OOLUA_MEM_FUNC(void, setLightSource, LuaVariant)
    OOLUA_MEM_FUNC(void, setExaminable, LuaVariant)
    OOLUA_MEM_FUNC(void, setCombat, LuaVariant)
    OOLUA_MEM_FUNC(void, setParticleEmitter, LuaVariant)
    OOLUA_MEM_FUNC(void, setSkeleton, LuaVariant)
    OOLUA_MEM_FUNC(void, setPersistent, const bool)
    OOLUA_MEM_FUNC_CONST(bool, isPersistent)
    OOLUA_MEM_FUNC_CONST(bool, isActive)
    OOLUA_MEM_FUNC_CONST(const char*, getDispName)
    OOLUA_MEM_FUNC(void, setDispName, const char*)
    OOLUA_MEM_FUNC_CONST(maybe_null<Behavior*>, getBehaviorByIndex, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getBehaviorCount)
    OOLUA_MEM_FUNC(void, setBehaviorByIndex, unsigned, LuaVariant)
OOLUA_PROXY_END


#endif // ELYSIAN_ENTITY_PROXY_HPP

