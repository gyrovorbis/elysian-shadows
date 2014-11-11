#ifndef ELYSIAN_LUA_WRAPPERS_HPP
#define ELYSIAN_LUA_WRAPPERS_HPP

#include <oolua.h>

#include <gyro.h>
#include "elysian_renderable_object.hpp"
#include "elysian_entity.hpp"
#include "elysian_debug.hpp"
#include "elysian_renderable.hpp"
#include "elysian_animation.hpp"
#include "elysian_collidable.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_character.hpp"
#include "elysian_warp.hpp"
#include "elysian_examinable.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_lua_singleton.hpp"
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"
#include "elysian_party_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_audio_source.hpp"
#include "elysian_light_source.hpp"
#include "elysian_level_system.hpp"
#include "elysian_light_system.hpp"
#include "elysian_rigid_body_system.hpp"
#include "elysian_particle_system.hpp"
#include "elysian_lua_script.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_quad_sprite.hpp"
#include "elysian_day_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_material.hpp"
#include "elysian_force_generator.hpp"
#include "elysian_combat.hpp"
#include "elysian_combat_system.hpp"
#include "elysian_lua_wrappers.hpp"
#include "elysian_animation_system.hpp"
#include "elysian_character_system.hpp"
#include "elysian_camera_system.hpp"

/* IT'S NOT SAFE TO GO ALONE! TAKE THIS HEADER FILE. IT WILL HELP
 * YOU ON YOUR JOURNEY!
 *
 * I have taken special care to create a SINGLE header file serving
 * as a registry for all Gamma/libGyro functionality exposed to Lua.
 * I have tried to keep this as clean as possible so that it can be
 * used as an index without scaring scripters away. This is everything
 * you need. It is organized as follows:
 * 1) C and C++ Class and Struct Wrappers
 * 2) C and C++ Constant and Enumeration Wrappers
 * 3) C Global and C++ Static Member Function Wrappers
 *
 * Now go forth unto the world with the power of Lua, my children.
 *				--Falco
 */


// ====== C/++ CLASS/STRUCT WRAPPERS ======

namespace OOLUA
{
	typedef elysian::InstanceTransform			InstanceTransform;
	typedef elysian::RenderableObject			RenderableObject;
	typedef elysian::Component					Component;
	typedef elysian::Entity						Entity;
	typedef elysian::PrimAttrib					PrimAttrib;
	typedef elysian::TexturedAttrib				TexturedAttrib;
	typedef elysian::SpriteAttrib				SpriteAttrib;
	typedef elysian::QuadSpriteAttrib			QuadSpriteAttrib;
	typedef elysian::Material					Material;
	typedef elysian::Renderable					Renderable;
	typedef elysian::AnimationSequence			AnimationSequence;
	typedef elysian::AnimationAttrib			AnimationAttrib;
	typedef elysian::ChildAnimationSequence		ChildAnimationSequence;
	typedef elysian::ChildAnimationAttrib		ChildAnimationAttrib;
	typedef elysian::ChildAnimation				ChildAnimation;
	typedef elysian::Animation					Animation;
	typedef elysian::CollisionFlags				CollisionFlags;
	typedef elysian::CollidableAttrib			CollidableAttrib;
    typedef elysian::CollidableStaticAttrib     CollidableStaticAttrib;
	typedef elysian::CollisionContact			CollisionContact;
	typedef elysian::Collidable					Collidable;
	typedef elysian::RigidBodyAttrib			RigidBodyAttrib;
	typedef elysian::ForceTarget				ForceTarget;
	typedef elysian::RigidBody					RigidBody;
	typedef elysian::ForceGenerator				ForceGenerator;
	typedef elysian::GravityForceGenerator		GravityForceGenerator;
	typedef elysian::ImplosionForceGenerator	ImplosionForceGenerator;
	typedef elysian::ExplosionForceGenerator	ExplosionForceGenerator;
	typedef elysian::Character					Character;
	typedef elysian::Warp						Warp;
	typedef elysian::Examinable					Examinable;
	typedef elysian::Dialog						Dialog;
	typedef elysian::DialogScreen				DialogScreen;
	typedef elysian::DialogText					DialogText;
	typedef elysian::DialogPrompt				DialogPrompt;
	typedef elysian::MenuWidget					MenuWidget;
	typedef elysian::MenuText					MenuText;
	typedef elysian::MenuEntity					MenuEntity;
	typedef elysian::CombatAttrib				CombatAttrib;
	typedef elysian::CombatStatus				CombatStatus;
	typedef elysian::CombatReceipt				CombatReceipt;
	typedef elysian::Combat						Combat;
	typedef elysian::ParticleEmitterAttrib		ParticleEmitterAttrib;
	typedef elysian::ParticleEmitter			ParticleEmitter;
	typedef elysian::Behavior					Behavior;
	typedef elysian::LuaBehavior				LuaBehavior;
	typedef elysian::Tile						Tile;
	typedef elysian::Level						Level;
	typedef elysian::MapEntry					MapEntry;
	typedef elysian::TileEntry					TileEntry;
	typedef elysian::Area						Area;
	typedef elysian::GameInput					GameInput;
	typedef elysian::Singletons					Singletons;
	typedef elysian::AnimationSystem			AnimationSystem;
	typedef elysian::LightAttrib				LightAttrib;
	typedef elysian::PartySystem				PartySystem;
	typedef elysian::TimerSystem				TimerSystem;
	typedef elysian::RigidBodySystem			RigidBodySystem;
	typedef elysian::CharacterSystem			CharacterSystem;
	typedef elysian::AudioSource				AudioSource;
	typedef elysian::LightSource				LightSource;
	typedef elysian::AudioSystem				AudioSystem;
	typedef elysian::LevelSystem				LevelSystem;
	typedef elysian::LightSystem				LightSystem;
	typedef elysian::ParticleSystem				ParticleSystem;
	typedef elysian::DaySystem					DaySystem;
	typedef elysian::GuiSystem					GuiSystem;
	typedef elysian::DialogSystem				DialogSystem;
	typedef elysian::CombatSystem				CombatSystem;
	typedef elysian::CameraSystem				CameraSystem;
	typedef elysian::LuaBehaviorScriptAttrib	LuaBehaviorScriptAttrib;
	typedef elysian::Direction					Direction;
	typedef elysian::EventMonitor				EventMonitor;
	typedef elysian::LuaEventMonitor			LuaEventMonitor;
	typedef elysian::LuaEventCallback			LuaEventCallback;
	typedef elysian::CameraKeyFrame				CameraKeyFrame;
}

struct Lua32BitMask {
	uint32_t mask = 0;

	uint32_t toInt(void) const { return mask; }
	void fromInt(const uint32_t val) { mask = val; }

	void setBit(const unsigned char index, const unsigned char val) {
		if(val) mask |= (1<<index);
		else mask &= ~(1<<index);
	}
};

OOLUA_PROXY(Lua32BitMask)
	OOLUA_MEM_FUNC_CONST(uint32_t, toInt)
	OOLUA_MEM_FUNC(void, fromInt, uint32_t)
	OOLUA_MEM_FUNC(void, setBit, unsigned char, unsigned char)
OOLUA_PROXY_END

OOLUA_PROXY(LuaBehaviorScriptAttrib)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC(inheritFromBehavior)
	OOLUA_MFUNC(addSupportedTrigger)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector2)
    OOLUA_MGET_MSET(x, getX, setX)
    OOLUA_MGET_MSET(y, getY, setY)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector3)
	OOLUA_MGET_MSET(x, getX, setX)
	OOLUA_MGET_MSET(y, getY, setY)
	OOLUA_MGET_MSET(z, getZ, setZ)
OOLUA_PROXY_END

OOLUA_PROXY(GYVector4)
	OOLUA_MGET_MSET(x, getX, setX)
	OOLUA_MGET_MSET(y, getY, setY)
	OOLUA_MGET_MSET(z, getZ, setZ)
	OOLUA_MGET_MSET(w, getW, setW)
OOLUA_PROXY_END

OOLUA_PROXY(GYColor4)
	OOLUA_MGET_MSET(a, getA, setA)
	OOLUA_MGET_MSET(r, getR, setR)
	OOLUA_MGET_MSET(g, getG, setG)
	OOLUA_MGET_MSET(b, getB, setB)
OOLUA_PROXY_END

OOLUA_PROXY(GYWav)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END

OOLUA_PROXY(EventMonitor)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	//OOLUA_SFUNC(null)
OOLUA_PROXY_END

namespace OOLUA {
	template<> class Proxy_class<elysian::LuaEventMonitor>;
}
OOLUA_PROXY(LuaEventMonitor, EventMonitor)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setTable, OOLUA::Lua_table_ref&)
	OOLUA_MEM_FUNC(OOLUA::Lua_table_ref&, getTable)
OOLUA_PROXY_END

OOLUA_PROXY(LuaEventCallback, LuaEventMonitor)
	OOLUA_TAGS(
		No_default_constructor
	)
	OOLUA_CTORS(
		OOLUA_CTOR(OOLUA::Lua_func_ref&)
	)
	OOLUA_MEM_FUNC(void, setCallbackFunction, OOLUA::Lua_func_ref&)
OOLUA_PROXY_END

OOLUA_PROXY(InstanceTransform)
	OOLUA_MEM_FUNC_CONST(void, apply)
	OOLUA_MEM_FUNC_CONST_RENAME(getPos2f, void, getPos, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_CONST_RENAME(getPos3f, void, getPos, out_p<float&>, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_RENAME(setPos2f, void, setPos, float, float)
	OOLUA_MEM_FUNC_RENAME(setPos3f, void, setPos, float, float, float)
	OOLUA_MEM_FUNC_RENAME(translate2f, void, translate, float, float)
	OOLUA_MEM_FUNC_RENAME(translate3f, void, translate, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize3f, void, setSize, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale3f, void, setScale, float, float, float)
    OOLUA_MEM_FUNC_RENAME(setSize2f, void, setSize, float, float)
    OOLUA_MEM_FUNC_RENAME(setScale2f, void, setScale, float, float)
	OOLUA_MEM_FUNC(void, setOrientZ, float)
	OOLUA_MEM_FUNC(void, rotate, float)
	OOLUA_MEM_FUNC_RENAME(scale2f, void, scale, float, float)
	OOLUA_MEM_FUNC_CONST_RENAME(getSize2f, void, getSize, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_CONST_RENAME(getScale2f, void, getScale, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_CONST(float, getOrientZ)
	OOLUA_MEM_FUNC(GYVector2&, getPosVec)
	OOLUA_MEM_FUNC(GYVector2& getSizeVec)
	OOLUA_MEM_FUNC(GYVector2& getScaleVec)
OOLUA_PROXY_END

OOLUA_PROXY(RenderableObject)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(InstanceTransform&, getTransform)
OOLUA_PROXY_END

namespace OOLUA {
	template<> class Proxy_class<elysian::Renderable>;
	template<> class Proxy_class<elysian::Animation>;
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
}
OOLUA_PROXY(Entity, RenderableObject)
	OOLUA_MEM_FUNC(void, move, const GYVector2&)
	OOLUA_MEM_FUNC(void, setName, const char*)
	OOLUA_MEM_FUNC_CONST(const char *, getName)
	OOLUA_MEM_FUNC(void, setElev, const unsigned char)
	OOLUA_MEM_FUNC_CONST(unsigned char, getElev)
//	OOLUA_MFUNC_CONST(getNormalType)
//	OOLUA_MFUNC(setNormalType)
	OOLUA_MEM_FUNC(Renderable*, addRenderable)
	OOLUA_MEM_FUNC(Animation*, addAnimation)
	OOLUA_MEM_FUNC(Collidable*, addCollidable)
	OOLUA_MEM_FUNC(RigidBody*, addRigidBody)
	OOLUA_MEM_FUNC(Character*, addCharacter)
	OOLUA_MEM_FUNC(Warp*, addWarp)
	OOLUA_MEM_FUNC(AudioSource*, addAudioSource)
	OOLUA_MEM_FUNC(LightSource*, addLightSource)
	OOLUA_MEM_FUNC(Examinable*, addExaminable)
	OOLUA_MEM_FUNC(Combat*, addCombat)
	OOLUA_MEM_FUNC(ParticleEmitter*, addParticleEmitter)
	OOLUA_MEM_FUNC(Behavior*, addBehavior, const char*)
    OOLUA_MEM_FUNC(maybe_null<LuaBehavior*>, addLuaBehavior, const char*, OOLUA::Lua_table_ref)
    OOLUA_MEM_FUNC_CONST(maybe_null<Renderable*>, getRenderable)
    OOLUA_MEM_FUNC_CONST(maybe_null<Animation*>, getAnimation)
    OOLUA_MEM_FUNC_CONST(maybe_null<Collidable*>, getCollidable)
    OOLUA_MEM_FUNC_CONST(maybe_null<RigidBody*>, getRigidBody)
    OOLUA_MEM_FUNC_CONST(maybe_null<Character*>, getCharacter)
    OOLUA_MEM_FUNC_CONST(maybe_null<Warp*>, getWarp)
    OOLUA_MEM_FUNC_CONST(maybe_null<AudioSource*>, getAudioSource)
    OOLUA_MEM_FUNC_CONST(maybe_null<LightSource*>, getLightSource)
    OOLUA_MEM_FUNC_CONST(maybe_null<Examinable*>, getExaminable)
    OOLUA_MEM_FUNC_CONST(maybe_null<Combat*>, getCombat)
    OOLUA_MEM_FUNC_CONST(maybe_null<ParticleEmitter*>, getParticleEmitter)
    OOLUA_MEM_FUNC_CONST(maybe_null<Behavior*>, getBehavior, const char*)
    OOLUA_MEM_FUNC_CONST(maybe_null<LuaBehavior*>, getLuaBehavior, const char*)
	OOLUA_MEM_FUNC(void, removeRenderable)
	OOLUA_MEM_FUNC(void, removeAnimation)
	OOLUA_MEM_FUNC(void, removeCollidable)
	OOLUA_MEM_FUNC(void, removeRigidBody)
	OOLUA_MEM_FUNC(void, removeCharacter)
	OOLUA_MEM_FUNC(void, removeWarp)
	OOLUA_MEM_FUNC(void, removeAudioSource)
	OOLUA_MEM_FUNC(void, removeLightSource)
	OOLUA_MEM_FUNC(void, removeExaminable)
	OOLUA_MEM_FUNC(void, removeCombat);
	OOLUA_MEM_FUNC(void, removeParticleEmitter)
	OOLUA_MEM_FUNC(void, removeBehavior, const char*)
	OOLUA_MEM_FUNC(void, removeLuaBehavior, const char*)
	OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
	OOLUA_MEM_FUNC(void, setSelfTable, Lua_table_ref)
	OOLUA_MEM_FUNC(void, setShadowDepth, float)
	OOLUA_MEM_FUNC(void, setShadowVolume, bool)
    OOLUA_MEM_FUNC(void, setPlaneType, GY_RECT_PLANE_TYPE)
	OOLUA_MEM_FUNC_CONST(GY_RECT_PLANE_TYPE, getPlaneType)
	OOLUA_MEM_FUNC_CONST(float, getShadowDepth)
	OOLUA_MEM_FUNC_CONST(bool, shadowVolume)
OOLUA_PROXY_END

OOLUA_PROXY(Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(Entity*, getEntity)
OOLUA_PROXY_END

OOLUA_PROXY(PrimAttrib)
	OOLUA_TAGS (Abstract)
OOLUA_PROXY_END

OOLUA_PROXY(SpriteAttrib, PrimAttrib)
	OOLUA_MEM_FUNC(void, setFrame, unsigned)
	OOLUA_MEM_FUNC(void, setColor, unsigned char, unsigned char, unsigned char, unsigned char)
	OOLUA_MEM_FUNC_CONST(void, getColor, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>)
	OOLUA_MEM_FUNC_CONST(unsigned char, getAlpha)
	OOLUA_MEM_FUNC(void, setAlpha, unsigned char)
	OOLUA_MEM_FUNC(void, loadTexture, const char *, unsigned, unsigned, unsigned, unsigned)
	OOLUA_MEM_FUNC(void, loadSheet, const char *, unsigned, unsigned)
	OOLUA_MEM_FUNC(void, setTexCoord, unsigned, float, float)
OOLUA_PROXY_END

OOLUA_PROXY(QuadSpriteAttrib, SpriteAttrib)
	OOLUA_MEM_FUNC(void, setVertexColor, const unsigned short, const unsigned char, const unsigned char, const unsigned char, const unsigned char)
	OOLUA_MEM_FUNC_CONST(void, getVertexColor, unsigned short, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>)
	OOLUA_MEM_FUNC(void, setVerticesAlpha, const unsigned char, const unsigned char, const unsigned char, const unsigned char)
OOLUA_PROXY_END

OOLUA_PROXY(Material)
	OOLUA_MEM_FUNC(void, setAmbientColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setDiffuseColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setSpecularColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setEmissiveColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setShininess, float)
OOLUA_PROXY_END

OOLUA_PROXY(Renderable, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(void, getOffset, GYVector2&)
	OOLUA_MEM_FUNC(void, setOffset, const GYVector2&)
	OOLUA_MEM_FUNC_CONST(unsigned, getFrame)
	OOLUA_MEM_FUNC(void, setFrame, unsigned)
	OOLUA_MEM_FUNC_CONST(SpriteAttrib*, getSpriteAttrib)
	OOLUA_MEM_FUNC_CONST(QuadSpriteAttrib*, getQuadSpriteAttrib)
	OOLUA_MEM_FUNC(void, setSpriteAttrib, cpp_in_p<SpriteAttrib*>)
	OOLUA_MEM_FUNC(void, setMaterial, cpp_in_p<Material*>)
	OOLUA_MFUNC_CONST(getMaterial)
	OOLUA_MFUNC(loadNormalMap)
	OOLUA_MEM_FUNC(void, loadSpecularMap, const char*, unsigned, unsigned)
OOLUA_PROXY_END

OOLUA_PROXY(AnimationSequence)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addFrame, float, float, float, unsigned, GY_RECT_TEX_ORIENT)
    OOLUA_MEM_FUNC(AnimationSequence*, addSequence, const char*)
    OOLUA_MEM_FUNC(void, setLinearInterpolation, bool)
OOLUA_PROXY_END

OOLUA_PROXY(AnimationAttrib)
	OOLUA_MEM_FUNC(AnimationSequence*, addSequence, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimationSequence)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addFrame, float, float, float, float)
    OOLUA_MEM_FUNC(ChildAnimationSequence*, addSequence, const char*)
    OOLUA_MEM_FUNC(void, setLinearInterpolation, bool)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimation)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(ChildAnimationSequence*, addSequence, const char*)
	OOLUA_MEM_FUNC(void, setGlobalOffset, float, float, float)
    OOLUA_MEM_FUNC(void, setInheritSpeedMultiplier, bool)
	OOLUA_MEM_FUNC(void, setInheritScale, bool)
OOLUA_PROXY_END

OOLUA_PROXY(ChildAnimationAttrib)
	OOLUA_MEM_FUNC(ChildAnimation*, addAnimation, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(Animation, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(AnimationAttrib*, getAttrib)
	OOLUA_MEM_FUNC_CONST(ChildAnimationAttrib*, getChildAttrib)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<AnimationAttrib *>)
	OOLUA_MEM_FUNC(void, setChildAttrib, cpp_in_p<ChildAnimationAttrib*>)
    OOLUA_MEM_FUNC(void, playAnimation, const char*, EventMonitor*)
	OOLUA_MEM_FUNC(void, stopAnimation)
	OOLUA_MEM_FUNC(void, addChildEntity, Entity*, const char*)
	OOLUA_MEM_FUNC(void, removeChildEntity, const char*)
	OOLUA_MEM_FUNC_CONST(bool, isLooping)
	OOLUA_MEM_FUNC(void, setLooping, bool)
	OOLUA_MEM_FUNC_CONST(unsigned, getCurrentFrame)
	OOLUA_MEM_FUNC(void, setCurrentFrame, const unsigned)
	OOLUA_MEM_FUNC(void, setChildPivot, float, float)
	OOLUA_MEM_FUNC(void, setSpeedMultiplier, float)
	OOLUA_MEM_FUNC(void, addState, const char*)
	OOLUA_MEM_FUNC(void, removeState, const char*)
	OOLUA_MEM_FUNC(void, replaceState, const char*, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(AnimationSystem)
OOLUA_TAGS (
	No_public_constructors,
	No_public_destructor
)
OOLUA_MEM_FUNC(void, linearInterpTransform, InstanceTransform*, const InstanceTransform&, float, EventMonitor*)
OOLUA_PROXY_END

OOLUA_PROXY(CollisionFlags)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, resetType)
	OOLUA_MEM_FUNC(void, resetCheckAgainst)
	OOLUA_MEM_FUNC(void, addType, uint32_t)
	OOLUA_MEM_FUNC(void, addCheckAgainst, uint32_t)
OOLUA_PROXY_END

OOLUA_PROXY(GYStatic)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MGET_MSET(inverseMass, getInverseMass, setInverseMass)
	OOLUA_MGET_MSET(coeffOfRest, getCoeffOfRest, setCoeffOfRest)
    OOLUA_MGET_MSET(zCoeffOfRest, getZCoeffOfRest, setZCoeffOfRest)
    //OOLUA_MGET_MSET(damping, getDamping, setDamping)
	OOLUA_MGET_MSET(invMomOfInertia, getInvMomOfInertia, setInvMomOfInertia)
	OOLUA_MGET_MSET(angDamping, getAngDamping, setAngDamping)
OOLUA_PROXY_END

OOLUA_PROXY(CollidableStaticAttrib, GYStatic)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setDamping, float)
    OOLUA_MEM_FUNC(void, setZDamping, float)
    OOLUA_MEM_FUNC(void, setDamping3f, float, float, float)

OOLUA_PROXY_END

OOLUA_PROXY(CollidableAttrib)
	OOLUA_MEM_FUNC_CONST(GY_COLLIDER_TYPE, getColliderType)
	OOLUA_MEM_FUNC(void, setColliderType, GY_COLLIDER_TYPE)
    OOLUA_MEM_FUNC(void, setRelPos, float, float, float)
    OOLUA_MEM_FUNC(void, setRelSize, float, float, float)
	OOLUA_MEM_FUNC(CollisionFlags*, getFlags)
    OOLUA_MEM_FUNC(CollidableStaticAttrib*, getStaticAttrib)
OOLUA_PROXY_END

OOLUA_PROXY(CollisionContact)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(Collidable*, getCollidable, unsigned)
	OOLUA_MEM_FUNC_CONST(void, getWorldPos, GYVector2&)
	OOLUA_MEM_FUNC_CONST(void, getNormal, GYVector2&)
	OOLUA_MEM_FUNC_CONST(void, getObjPos, unsigned, GYVector2&)
	OOLUA_MEM_FUNC_CONST(float, getSeparatingVel)
	OOLUA_MEM_FUNC_CONST(float, getClosingVel)
	OOLUA_MEM_FUNC_CONST(float, getDeltaVel)
	OOLUA_MEM_FUNC_CONST(float, getDepth)
OOLUA_PROXY_END

OOLUA_PROXY(Collidable, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(CollidableAttrib*, getAttrib)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<CollidableAttrib*>)
	OOLUA_MEM_FUNC_CONST(unsigned, getContactCount)
	OOLUA_MEM_FUNC(void, setResolvePenetration, bool)
OOLUA_PROXY_END

OOLUA_PROXY(RigidBodyAttrib)
	OOLUA_MEM_FUNC(void, setForceMask, uint32_t)
	OOLUA_MEM_FUNC_CONST(uint32_t, getForceMask)
	OOLUA_MEM_FUNC(void, setBreakable, bool)
	OOLUA_MEM_FUNC(void, setBaseDurability, float)
OOLUA_PROXY_END

OOLUA_PROXY(ForceTarget)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, applyForce, const GYVector3&)
	OOLUA_MEM_FUNC(void, applyForceToPoint, const GYVector2&, const GYVector2&)
OOLUA_PROXY_END

OOLUA_PROXY(RigidBody, Component, ForceTarget)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<RigidBodyAttrib*>)
	OOLUA_MEM_FUNC_CONST(RigidBodyAttrib*, getAttrib)
	OOLUA_MEM_FUNC(void, setDurability, float)
	OOLUA_MEM_FUNC_CONST(float, getDurability)
    OOLUA_MEM_FUNC_CONST(GYVector3, getVelocity)
    OOLUA_MEM_FUNC(void, setVelocity, const GYVector3&)
OOLUA_PROXY_END

OOLUA_PROXY(ForceGenerator)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setForceMask, uint32_t)
OOLUA_PROXY_END

OOLUA_PROXY(GravityForceGenerator, ForceGenerator)
	OOLUA_MEM_FUNC(void, setAccel, const GYVector3&)
OOLUA_PROXY_END

OOLUA_PROXY(ImplosionForceGenerator, ForceGenerator)
	OOLUA_MEM_FUNC(void, setFocalEntity, Entity*)
	OOLUA_MEM_FUNC(void, setForceMagnitude, float)
	OOLUA_MEM_FUNC(void, setRadius, float)
OOLUA_PROXY_END

OOLUA_PROXY(ExplosionForceGenerator, ForceGenerator)
	OOLUA_MEM_FUNC(void, setFocalEntity, Entity*)
	OOLUA_MEM_FUNC(void, setForceMagnitude, float)
	OOLUA_MEM_FUNC(void, setRadius, float)
OOLUA_PROXY_END

OOLUA_PROXY(Direction)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END

OOLUA_PROXY(Character, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, faceEntity, Entity&)
	OOLUA_MEM_FUNC(void, faceDir, Direction::DIR4)
	OOLUA_MEM_FUNC(void, move, const GYVector2&)
	OOLUA_MEM_FUNC(void, setAnimate, bool)
	OOLUA_MEM_FUNC_CONST(bool, isFacing, const Entity&)
	OOLUA_MEM_FUNC_CONST(Direction::DIR4, getFaceDir)
    OOLUA_MEM_FUNC(void, setWalkForce, float)
    OOLUA_MEM_FUNC(void, setDashMultiplier, float)
    OOLUA_MEM_FUNC(void, setDashing, bool)
    OOLUA_MEM_FUNC_CONST(Entity*, getHeldEntity)
OOLUA_PROXY_END

OOLUA_PROXY(CharacterSystem)
OOLUA_TAGS (
	No_public_constructors,
	No_public_destructor
)
OOLUA_MEM_FUNC(void, walkToWayPoint, Entity*, float, float, EventMonitor*)
OOLUA_MEM_FUNC(void, moveToWayPoint, Entity*, float, float, float, EventMonitor*)
OOLUA_PROXY_END

OOLUA_PROXY(Warp, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setToPos, const float, const float, unsigned char)
	OOLUA_MEM_FUNC(void, setAreaPath, const char*)
	OOLUA_MEM_FUNC(void, setLevelPath, const char*)
	OOLUA_MEM_FUNC(void, setChangeArea, bool)
	OOLUA_MEM_FUNC(void, setChangeLevel, bool)
	OOLUA_MEM_FUNC(void, setObliqueTransition, bool)
OOLUA_PROXY_END

OOLUA_PROXY(AudioSource, Component)
	 OOLUA_TAGS (
		 No_public_constructors,
		 No_public_destructor
	 )
	OOLUA_MEM_FUNC(void, setWav, const GYWav*)
	OOLUA_MEM_FUNC(void, playWav, const GYWav*)
	OOLUA_MEM_FUNC(void, play)
	OOLUA_MEM_FUNC(void, stop)
	OOLUA_MEM_FUNC(void, setGain, float)
	OOLUA_MEM_FUNC(void, setPitch, float)
	OOLUA_MEM_FUNC(void, setLooping, bool)
OOLUA_PROXY_END

OOLUA_PROXY(LightAttrib)
	OOLUA_MEM_FUNC(void, setAmbientColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setDiffuseColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setSpecularColor, const GYVector4&)
	OOLUA_MEM_FUNC(void, setAttenuationFactors, const GYVector3&)
	OOLUA_MEM_FUNC(void, setSpotExponent, const float)
	//OOLUA_MEM_FUNC(void, setComputeDistanceAttenuation, const bool)
OOLUA_PROXY_END

OOLUA_PROXY(LightSource, Component)
	 OOLUA_TAGS (
		 No_public_constructors,
		 No_public_destructor
	 )
	OOLUA_MEM_FUNC(void, setLightType, const GY_LIGHT_TYPE)
	OOLUA_MEM_FUNC(void, setDirection, const GYVector3&)
    OOLUA_MEM_FUNC(void, setRelativePos, float, float, float)
	OOLUA_MEM_FUNC(void, setSpotCutoffAngle, const float)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<const LightAttrib*>)
	OOLUA_MEM_FUNC(LightAttrib&, getAttrib)
	OOLUA_MFUNC(setShadowCaster)
OOLUA_PROXY_END

OOLUA_PROXY(ParticleEmitterAttrib, SpriteAttrib, GYStatic)
	OOLUA_MFUNC(setAcceleration)
	OOLUA_MFUNC(setMaxLife)
	OOLUA_MEM_FUNC(void, setMaterial, cpp_in_p<Material*>)
	OOLUA_MEM_FUNC(void, loadSpecularMap, const char*, unsigned, unsigned)
	OOLUA_MEM_FUNC(void, setForceMask, uint32_t)
	OOLUA_MEM_FUNC(void, setIntensityFactor, float)
    OOLUA_MEM_FUNC(void, setStartSize, unsigned)
    OOLUA_MEM_FUNC(void, setEndSize, unsigned)
    OOLUA_MEM_FUNC(void, setEndColor, GYColor4)
    OOLUA_MEM_FUNC(void, setAnimGridSize, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, setAnimGridIndex, unsigned, unsigned, int)
    OOLUA_MEM_FUNC(void, setAnimFrameDuration, unsigned)
    OOLUA_MEM_FUNC(void, setStick, bool)
    OOLUA_MEM_FUNC(void, loadNormalMap, const char*, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, setRandomStartFrame, bool)
    OOLUA_MEM_FUNC(void, setShadowVolume, bool)
OOLUA_PROXY_END

OOLUA_PROXY(ParticleEmitter, Component)
    OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, allocatePool, unsigned)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<ParticleEmitterAttrib*>)
	OOLUA_MEM_FUNC(void, emitFromPoint, const GYVector3&, float, unsigned)
	OOLUA_MEM_FUNC(void, emitFromEntity, const GYVector3&, float, unsigned)
	OOLUA_MEM_FUNC(void, emitFromPointInDir, const GYVector3&, const GYVector3&, unsigned, float)
OOLUA_PROXY_END

OOLUA_PROXY(DialogScreen)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(const char*, getText)
	OOLUA_MEM_FUNC(void, addSourceEntity, Entity*)
	OOLUA_MEM_FUNC_CONST(unsigned, getIndex)
	OOLUA_MEM_FUNC(void, setTrigger, bool)
OOLUA_PROXY_END

OOLUA_PROXY(DialogText, DialogScreen)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(int, getNextIndex)
	OOLUA_MEM_FUNC(void, setNextIndex, int)
OOLUA_PROXY_END

OOLUA_PROXY(DialogPrompt, DialogScreen)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addOption, const char*, int)
OOLUA_PROXY_END

OOLUA_PROXY(Dialog)
	OOLUA_MEM_FUNC(DialogText*, addTextScreen, const char*)
	OOLUA_MEM_FUNC(DialogPrompt*, addPromptScreen, const char*)
	OOLUA_MEM_FUNC(void, setTriggerEntity, Entity*)
OOLUA_PROXY_END

OOLUA_PROXY(Examinable, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setDialog, cpp_in_p<Dialog*>)
	OOLUA_MEM_FUNC(void, setMaxDist, float)
OOLUA_PROXY_END

OOLUA_PROXY(CombatReceipt)
	OOLUA_MEM_FUNC(void, setText, const char*)
	OOLUA_MEM_FUNC(void, setTextColor, const GYColor4&)
	OOLUA_MEM_FUNC(void, setPartColor, const GYColor4&)
	OOLUA_MEM_FUNC(void, setPartIntensityFactor, float)
OOLUA_PROXY_END

OOLUA_PROXY(CombatAttrib)
	OOLUA_MEM_FUNC(void, setTypeFlags, uint32_t)
	OOLUA_MEM_FUNC(void, setTypeAgainstFlags, uint32_t)
	OOLUA_MEM_FUNC(void, setParticleAttrib, ParticleEmitterAttrib*)
OOLUA_PROXY_END

OOLUA_PROXY(CombatStatus)
	OOLUA_MEM_FUNC(void, setHp, int)
	OOLUA_MEM_FUNC(void, setMp, int)
	OOLUA_MEM_FUNC_CONST(int, getHp)
	OOLUA_MEM_FUNC_CONST(int, getMp)
OOLUA_PROXY_END

OOLUA_PROXY(Combat, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setSelfTable, Lua_table_ref)
	OOLUA_MEM_FUNC(void, setActionTable, Lua_table_ref)
	OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
	OOLUA_MEM_FUNC(Lua_table_ref&, getActionTable)
	OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<CombatAttrib*>)
	OOLUA_MEM_FUNC(void, setDmgType, Combat::DMG_TYPE)
	OOLUA_MEM_FUNC(void, setDmgActive, bool)
	OOLUA_MEM_FUNC(void, setCollisionFlags, uint32_t)
    OOLUA_MEM_FUNC_CONST(CombatStatus, getStatus)
    OOLUA_MEM_FUNC(void, setStatus, const CombatStatus&)
OOLUA_PROXY_END

OOLUA_PROXY(MenuWidget)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END

OOLUA_PROXY(MenuText, MenuWidget)
	OOLUA_MEM_FUNC(void, setRelPos, float, float)
	OOLUA_MEM_FUNC(void, setFontType, unsigned)
	OOLUA_MEM_FUNC(void, setColor, GYColor4)
	OOLUA_MEM_FUNC(void, setAlignment, elysian::Font::ALIGNMENT)
	OOLUA_MEM_FUNC(void, setText, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(MenuEntity, MenuWidget, Entity)
	OOLUA_MEM_FUNC(void, addChildWidget, MenuWidget*)
	OOLUA_MEM_FUNC(void, setSizeType, MenuEntity::SIZE_TYPE)
	OOLUA_MEM_FUNC(void, removeChildWidget, MenuWidget*)
OOLUA_PROXY_END

OOLUA_PROXY(Behavior, Component)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addTrigger, Behavior::TRIGGER)
	OOLUA_MEM_FUNC(void, removeTrigger, Behavior::TRIGGER)
	OOLUA_MEM_FUNC_CONST(const char*, getBehavName)
OOLUA_PROXY_END

OOLUA_PROXY(LuaBehavior, Behavior)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
OOLUA_PROXY_END

OOLUA_PROXY(Tile)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, setColliderType, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getColliderType)
	OOLUA_MEM_FUNC(void, setDamping, float)
	OOLUA_MEM_FUNC(void, setWalkSfx, GYWav*)
	OOLUA_MEM_FUNC(void, setHitSfx, GYWav*)
OOLUA_PROXY_END

OOLUA_PROXY(Level)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(Tile*, getTile, uint16_t)
    //OOLUA_MEM_FUNC_CONST(unsigned short, getTileAmount)
    OOLUA_MEM_FUNC_CONST(const char*, getDirName)
	OOLUA_MEM_FUNC_CONST(const char*, getPath)
OOLUA_PROXY_END

OOLUA_PROXY(MapEntry)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(uint32_t, getValue)
	OOLUA_MEM_FUNC(void, setValue, uint32_t)
	OOLUA_MEM_FUNC_CONST(GY_RECT_TEX_ORIENT, getOrient)
	OOLUA_MEM_FUNC(void, setOrient, GY_RECT_TEX_ORIENT)
	OOLUA_MEM_FUNC_CONST(unsigned char, getElev)
	OOLUA_MEM_FUNC(void, setElev, unsigned char)
OOLUA_PROXY_END

OOLUA_PROXY(TileEntry, MapEntry)
	OOLUA_MEM_FUNC_CONST(uint16_t, getIndex)
	OOLUA_MEM_FUNC(void, setIndex, uint16_t)
	OOLUA_MEM_FUNC_CONST(bool, getZTran)
	OOLUA_MEM_FUNC(void, setZTran, bool)
	OOLUA_MEM_FUNC(void, setNormal, bool)
	OOLUA_MEM_FUNC_CONST(bool, getNormal)
    OOLUA_MEM_FUNC_CONST(GY_RECT_PLANE_TYPE, getPlane)
    OOLUA_MEM_FUNC(void, setPlane, GY_RECT_PLANE_TYPE)
OOLUA_PROXY_END

OOLUA_PROXY(Area)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
    OOLUA_MEM_FUNC_CONST(const char*, getDirName)
	OOLUA_MEM_FUNC_CONST(const char*, getPath)
	OOLUA_MEM_FUNC_CONST(TileEntry, getTileEntry, unsigned, unsigned, unsigned)
	OOLUA_MEM_FUNC(void, setTileEntry, unsigned, unsigned, unsigned, TileEntry)
	OOLUA_MEM_FUNC(void, addEntity, cpp_in_p<Entity*>)
	OOLUA_MEM_FUNC(void, removeEntity, in_out_p<Entity*>)
	OOLUA_MEM_FUNC(maybe_null<Entity*>, findEntity, const char*)
	OOLUA_MEM_FUNC_CONST(unsigned short, getMapWidth);
	OOLUA_MEM_FUNC_CONST(unsigned short, getMapHeight);
OOLUA_PROXY_END

OOLUA_PROXY(GameInput)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(bool, getTapped, GameInput::BUTTON)
	OOLUA_MEM_FUNC_CONST(bool, getDown, GameInput::BUTTON)
	OOLUA_MEM_FUNC(void, setUp, GameInput::BUTTON)
	OOLUA_MEM_FUNC(void, setDown, GameInput::BUTTON)
OOLUA_PROXY_END

OOLUA_PROXY(PartySystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(bool, addCharacterToParty, cpp_in_p<Entity*>)
	OOLUA_MEM_FUNC(bool, removeCharacterFromParty, in_out_p<const Entity*>)
	OOLUA_MEM_FUNC_CONST(Entity*, getPartyLeader)
	OOLUA_MEM_FUNC_CONST(Entity*, getCharacterPartyMember, unsigned)
OOLUA_PROXY_END

OOLUA_PROXY(TimerSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC_CONST(getTotalTicks)
	OOLUA_MFUNC_CONST(getCurrentFrameTicks)
	OOLUA_MFUNC_CONST(getDeltaTime)
	OOLUA_MFUNC_CONST(getTotalTime)
    OOLUA_MEM_FUNC(void, startTimerEvent, float, EventMonitor*)
OOLUA_PROXY_END

OOLUA_PROXY(LevelSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC_CONST(getPrevAreaPath)
	OOLUA_MFUNC_CONST(getPrevLevelPath)
OOLUA_PROXY_END

OOLUA_PROXY(RigidBodySystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
    OOLUA_MEM_FUNC(void, addForce, cpp_in_p<ForceGenerator*>)
	OOLUA_MEM_FUNC(void, applyForce, ForceGenerator*)
	OOLUA_MEM_FUNC(void, removeForce, ForceGenerator*)
	OOLUA_MEM_FUNC(void, clearForces)
OOLUA_PROXY_END

OOLUA_PROXY(LightSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC(setGlobalAmbientColor)
    OOLUA_MEM_FUNC(void, interpLightAttrib, float, const LightAttrib&, LightAttrib*, EventMonitor*)
OOLUA_PROXY_END

OOLUA_PROXY(AudioSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC(playSfxChannel)
	OOLUA_MFUNC_CONST(stopSfxChannel)
	OOLUA_MFUNC_CONST(setSfxChannelGain)
	OOLUA_MFUNC_CONST(setSfxChannelPitch)
	OOLUA_MFUNC_CONST(setSfxChannelLooping)
	OOLUA_MFUNC_CONST(setSfxChannelRolloffFactor)
	OOLUA_MFUNC_CONST(setSfxChannelPos)
	OOLUA_MFUNC(loadBgmTrack)
	OOLUA_MFUNC_CONST(playBgmTrack)
	OOLUA_MFUNC_CONST(stopBgmTrack)
	OOLUA_MFUNC_CONST(pauseBgmTrack)
	OOLUA_MFUNC_CONST(isBgmTrackPlaying)
	OOLUA_MFUNC_CONST(getBgmTrack)
	OOLUA_MFUNC(setBgmTrack)
	OOLUA_MFUNC_CONST(getBgmTrackPath)
	OOLUA_MFUNC(fadeOutBgmTrack)
	OOLUA_MFUNC(fadeInBgmTrack)
	OOLUA_MFUNC(morphIntoBgmTrack)
	OOLUA_MFUNC(setBgmTrackLooping)
	OOLUA_MFUNC(setBgmTrackGain)
	OOLUA_MFUNC_CONST(setBgmTrackPitch)
	OOLUA_MFUNC_CONST(setBgmTrackPos)
	OOLUA_MFUNC_CONST(setBgmTrackRolloffFactor)
	OOLUA_MFUNC(fadeIntoBgmTrack)
	OOLUA_MFUNC(morphIntoNewBgmTrack)
	OOLUA_MFUNC(fadeIntoNewBgmTrack)
	OOLUA_MFUNC_CONST(getBgmTrackTime)
	OOLUA_MFUNC(loadAuxTrack)
	OOLUA_MFUNC_CONST(playAuxTrack)
	OOLUA_MFUNC_CONST(stopAuxTrack)
	OOLUA_MFUNC_CONST(pauseAuxTrack)
	OOLUA_MFUNC_CONST(isAuxTrackPlaying)
	OOLUA_MFUNC_CONST(getAuxTrackPath)
	OOLUA_MFUNC(fadeOutAuxTrack)
	OOLUA_MFUNC(fadeInAuxTrack)
	OOLUA_MFUNC(morphIntoNewAuxTrack)
	OOLUA_MFUNC(fadeIntoNewAuxTrack)
	OOLUA_MFUNC(setAuxTrackLooping)
	OOLUA_MFUNC(setAuxTrackGain)
	OOLUA_MFUNC_CONST(setAuxTrackPitch)
	OOLUA_MFUNC_CONST(setAuxTrackPos)
	OOLUA_MFUNC_CONST(setAuxTrackRolloffFactor)
	OOLUA_MFUNC_CONST(getAuxTrackTime)
OOLUA_PROXY_END

OOLUA_PROXY(ParticleSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC(emitFromPoint)
	OOLUA_MFUNC_CONST(getAttrib)
OOLUA_PROXY_END

OOLUA_PROXY(DaySystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MFUNC_CONST(getDays)
	OOLUA_MFUNC_CONST(getHours)
	OOLUA_MFUNC_CONST(getMinutes)
	OOLUA_MFUNC_CONST(getSunLightAttrib)
	OOLUA_MFUNC(setTimeDilation)
	OOLUA_MFUNC(setSunEnabled)
    OOLUA_MEM_FUNC(void, setTimeOfDay, float)
OOLUA_PROXY_END


OOLUA_PROXY(DialogSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, startDialog, Dialog*, EventMonitor*)
OOLUA_PROXY_END

OOLUA_PROXY(CombatSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addCombatant, Entity&)
	OOLUA_MEM_FUNC(void, removeCombatant, Entity&)
	OOLUA_MEM_FUNC(void, killCombatant, Entity&)
	OOLUA_MEM_FUNC(void, resolveCombatAction, Lua_table_ref, Combat*, Combat*)
    OOLUA_MEM_FUNC(void, sendStatusModifier, Combat*, CombatStatus*)
	OOLUA_MEM_FUNC(void, setBgmCombatTrack, unsigned)
	OOLUA_MEM_FUNC(void, setBgmDynamic, bool)
OOLUA_PROXY_END

OOLUA_PROXY(GuiSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC(void, addCombatText, const char*, GYColor4, const GYVector2&, const GYVector2&)
	OOLUA_MEM_FUNC(void, setFgMenuWidget, cpp_in_p<MenuWidget*>)
	OOLUA_MEM_FUNC(void, clearFgMenuWidget)
	OOLUA_MEM_FUNC(void, setBgMenuWidget, cpp_in_p<MenuWidget*>)
	OOLUA_MEM_FUNC(void, clearBgMenuWidget)
	OOLUA_MEM_FUNC(void, setClearColor, GYColor4)
    OOLUA_MEM_FUNC(void, toggleAdvLog)
    OOLUA_MEM_FUNC_CONST(void, getResolution, out_p<unsigned&>, out_p<unsigned&>)
OOLUA_PROXY_END

OOLUA_PROXY(CameraKeyFrame)
	OOLUA_MEM_FUNC(void, setPos, float, float, float)
    OOLUA_MEM_FUNC_CONST(void, getPos, out_p<float&>, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC(void, setDir, float, float, float)
    OOLUA_MEM_FUNC_CONST(void, getDir, out_p<float&>, out_p<float&>, out_p<float&>)
    OOLUA_MEM_FUNC(void, setOffset, float, float, float)
    OOLUA_MEM_FUNC_CONST(void, getOffset, out_p<float&>, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC(void, setScale, float)
    OOLUA_MEM_FUNC_CONST(float, getScale)
	OOLUA_MEM_FUNC(void, setZOrient, float)
    OOLUA_MEM_FUNC_CONST(float, getZOrient)
	OOLUA_MEM_FUNC(void, setXOrient, float)
    OOLUA_MEM_FUNC_CONST(float, getXOrient)
OOLUA_PROXY_END

OOLUA_PROXY(CameraSystem)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
	OOLUA_MEM_FUNC_CONST(void, addEntity, Entity*)
	OOLUA_MEM_FUNC_CONST(void, removeEntity, Entity*)
    OOLUA_MEM_FUNC(void, setFollowEntity, Entity*)
	OOLUA_MEM_FUNC(void, scale, float, float, EventMonitor*)
	OOLUA_MEM_FUNC_CONST(CameraKeyFrame, getKeyFrame)
	OOLUA_MEM_FUNC(void, moveToKeyFrame, const CameraKeyFrame&, float, EventMonitor*)
	OOLUA_MEM_FUNC(void, setBoundaries, float, float, float, float)
	OOLUA_MEM_FUNC(void, setDynamic, bool)
	OOLUA_MEM_FUNC(void, pushPerspectiveState)
	OOLUA_MEM_FUNC(void, popState)
    OOLUA_MEM_FUNC(void, shake, const GYVector3&, unsigned, float, bool, EventMonitor*)
OOLUA_PROXY_END


OOLUA_PROXY(Singletons)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END

//dummy class for encapsulating libGyro within its own namespace
class gyro {};
OOLUA_PROXY(gyro)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END


// ====== C/++ ENUMERATIONS AND CONSTANTS WRAPPERS ======
static inline void luaRegisterEnumsConstants(OOLUA::Script *s) {

	//gyro::TYPES
	s->register_class_static<gyro>("COLLIDER_NONE", GY_COLLIDER_NONE);
	s->register_class_static<gyro>("COLLIDER_AABB", GY_COLLIDER_AABB);
	s->register_class_static<gyro>("COLLIDER_OBB", GY_COLLIDER_OBB);
	s->register_class_static<gyro>("COLLIDER_RTRI", GY_COLLIDER_RTRI);
	s->register_class_static<gyro>("COLLIDER_CIRCLE", GY_COLLIDER_CIR);
	s->register_class_static<gyro>("TEX_ORIENT_NORMAL", GY_RECT_TEX_ORIENT_NORMAL);
	s->register_class_static<gyro>("TEX_ORIENT_FLIP_HORIZ", GY_RECT_TEX_ORIENT_FLIP_HORIZ);
	s->register_class_static<gyro>("TEX_ORIENT_FLIP_VERT", GY_RECT_TEX_ORIENT_FLIP_VERT);
	s->register_class_static<gyro>("TEX_ORIENT_ROT_90", GY_RECT_TEX_ORIENT_ROT_90);
	s->register_class_static<gyro>("TEX_ORIENT_ROT_180", GY_RECT_TEX_ORIENT_ROT_180);
	s->register_class_static<gyro>("TEX_ORIENT_ROT_270", GY_RECT_TEX_ORIENT_ROT_270);
	s->register_class_static<gyro>("TEX_ORIENT_FLIP_HORIZ_ROT_90", GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90);
	s->register_class_static<gyro>("TEX_ORIENT_FLIP_HORIZ_ROT_270", GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270);

	s->register_class_static<gyro>("LIGHT_DIRECTIONAL", GY_LIGHT_DIRECTIONAL);
	s->register_class_static<gyro>("LIGHT_SPOT", GY_LIGHT_SPOT);
	s->register_class_static<gyro>("LIGHT_POINT", GY_LIGHT_POINT);

    s->register_class_static<gyro>("RECT_PLANE_BOTTOM", GY_RECT_PLANE_BOTTOM);
    s->register_class_static<gyro>("RECT_PLANE_FRONT", GY_RECT_PLANE_FRONT);
    s->register_class_static<gyro>("RECT_PLANE_LEFT", GY_RECT_PLANE_LEFT);
    s->register_class_static<gyro>("RECT_PLANE_RIGHT", GY_RECT_PLANE_RIGHT);


	//Entity::DEPTH_TYPE
	typedef OOLUA::Entity Entity;
	s->register_class_static<Entity>("NORMAL_FLAT", elysian::NORMAL_FLAT);
	s->register_class_static<Entity>("NORMAL_FRONT", elysian::NORMAL_FRONT);

	//CollisionFlags::TYPE
	typedef OOLUA::CollisionFlags CollisionFlags;
	s->register_class_static<CollisionFlags>("TILE", CollisionFlags::TILE);
	s->register_class_static<CollisionFlags>("RENDERABLE", CollisionFlags::RENDERABLE);
	s->register_class_static<CollisionFlags>("COLLIDABLE", CollisionFlags::COLLIDABLE);
	s->register_class_static<CollisionFlags>("CHARACTER", CollisionFlags::CHARACTER);
	s->register_class_static<CollisionFlags>("WARP", CollisionFlags::WARP);
	s->register_class_static<CollisionFlags>("RIGID_BODY", CollisionFlags::RIGID_BODY);
	s->register_class_static<CollisionFlags>("ITEM", CollisionFlags::ITEM);
	s->register_class_static<CollisionFlags>("FRIENDLY", CollisionFlags::FRIENDLY);
	s->register_class_static<CollisionFlags>("ENEMY", CollisionFlags::ENEMY);
	s->register_class_static<CollisionFlags>("PLAYER", CollisionFlags::PLAYER);
	s->register_class_static<CollisionFlags>("ALL", CollisionFlags::ALL);

	//Combat::DMG_TYPE
	typedef OOLUA::Combat Combat;
	s->register_class_static<Combat>("DMG_NONE", Combat::DMG_NONE);
	s->register_class_static<Combat>("DMG_SOURCE", Combat::DMG_SOURCE);
	s->register_class_static<Combat>("DMG_TARGET", Combat::DMG_TARGET);
	s->register_class_static<Combat>("DMG_SOURCE_TARGET", Combat::DMG_SOURCE_TARGET);
	s->register_class_static<Combat>("ON_COLLIDE", Combat::ON_COLLIDE);
	s->register_class_static<Combat>("ON_COLLISION_ENTER", Combat::ON_COLLISION_ENTER);
	s->register_class_static<Combat>("ON_COLLISION_EXIT", Combat::ON_COLLISION_EXIT);

	//Behavior::TRIGGER
	typedef OOLUA::Behavior Behavior;
	s->register_class_static<Behavior>("ON_SCRIPT_CREATE", Behavior::ON_SCRIPT_CREATE);
	s->register_class_static<Behavior>("ON_SCRIPT_DESTROY", Behavior::ON_SCRIPT_DESTROY);
	s->register_class_static<Behavior>("ON_CREATE", Behavior::ON_CREATE);
	s->register_class_static<Behavior>("ON_DESTROY", Behavior::ON_DESTROY);
	s->register_class_static<Behavior>("ON_LOAD", Behavior::ON_LOAD);
	s->register_class_static<Behavior>("ON_UNLOAD", Behavior::ON_UNLOAD);
	s->register_class_static<Behavior>("ON_ENTER", Behavior::ON_ENTER);
	s->register_class_static<Behavior>("ON_EXIT", Behavior::ON_EXIT);
	s->register_class_static<Behavior>("ON_UPDATE", Behavior::ON_UPDATE);
	s->register_class_static<Behavior>("ON_RENDER", Behavior::ON_RENDER);
	s->register_class_static<Behavior>("ON_COLLIDE", Behavior::ON_COLLIDE);
	s->register_class_static<Behavior>("ON_COLLISION_ENTER", Behavior::ON_COLLISION_ENTER);
	s->register_class_static<Behavior>("ON_COLLISION_EXIT", Behavior::ON_COLLISION_EXIT);
	s->register_class_static<Behavior>("ON_EXAMINE", Behavior::ON_EXAMINE);
	s->register_class_static<Behavior>("ON_DIALOG", Behavior::ON_DIALOG);
    s->register_class_static<Behavior>("ON_COMBAT_ENTER", Behavior::ON_COMBAT_ENTER);
    s->register_class_static<Behavior>("ON_COMBAT_EXIT", Behavior::ON_COMBAT_EXIT);
    s->register_class_static<Behavior>("ON_COMBAT_DEATH", Behavior::ON_COMBAT_DEATH);
    s->register_class_static<Behavior>("ON_SHATTER", Behavior::ON_SHATTER);
    s->register_class_static<Behavior>("ON_COMBAT_RECEIVE_STATUS_MODIFIER", Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER);
    s->register_class_static<Behavior>("ON_COMBAT_TARGET_COLLISION_ENTER", Behavior::ON_COMBAT_TARGET_COLLISION_ENTER);

	//Tile::COLLIDER_TYPE
	typedef OOLUA::Tile Tile;
	s->register_class_static<Tile>("COLLIDER_NONE", Tile::COLLIDER_NONE);
	s->register_class_static<Tile>("COLLIDER_FULL", Tile::COLLIDER_FULL);
	s->register_class_static<Tile>("COLLIDER_HALF_E", Tile::COLLIDER_HALF_E);
	s->register_class_static<Tile>("COLLIDER_HALF_N", Tile::COLLIDER_HALF_N);
	s->register_class_static<Tile>("COLLIDER_HALF_W", Tile::COLLIDER_HALF_W);
	s->register_class_static<Tile>("COLLIDER_HALF_S", Tile::COLLIDER_HALF_S);
	s->register_class_static<Tile>("COLLIDER_QUAD_SE", Tile::COLLIDER_QUAD_SE);
	s->register_class_static<Tile>("COLLIDER_QUAD_NE", Tile::COLLIDER_QUAD_NE);
	s->register_class_static<Tile>("COLLIDER_QUAD_NW", Tile::COLLIDER_QUAD_NW);
	s->register_class_static<Tile>("COLLIDER_QUAD_SW", Tile::COLLIDER_QUAD_SW);
	s->register_class_static<Tile>("COLLIDER_L_SE", Tile::COLLIDER_L_SE);
	s->register_class_static<Tile>("COLLIDER_L_NE", Tile::COLLIDER_L_NE);
	s->register_class_static<Tile>("COLLIDER_L_NW", Tile::COLLIDER_L_NW);
	s->register_class_static<Tile>("COLLIDER_L_SW", Tile::COLLIDER_L_SW);
	s->register_class_static<Tile>("COLLIDER_QUADS_SE_NW", Tile::COLLIDER_QUADS_SE_NW);
	s->register_class_static<Tile>("COLLIDER_QUADS_SW_NE", Tile::COLLIDER_QUADS_SW_NE);
	s->register_class_static<Tile>("COLLIDER_45DEG_SE", Tile::COLLIDER_45DEG_SE);
	s->register_class_static<Tile>("COLLIDER_45DEG_NE", Tile::COLLIDER_45DEG_NE);
	s->register_class_static<Tile>("COLLIDER_45DEG_NW", Tile::COLLIDER_45DEG_NW);
	s->register_class_static<Tile>("COLLIDER_45DEG_SW", Tile::COLLIDER_45DEG_SW);

	//GameInput::BUTTONS
	typedef OOLUA::GameInput GameInput;
	s->register_class_static<GameInput>("BUT_NONE", GameInput::BUT_NONE);
	s->register_class_static<GameInput>("BUT_A", GameInput::BUT_A);
	s->register_class_static<GameInput>("BUT_B", GameInput::BUT_B);
	s->register_class_static<GameInput>("BUT_C", GameInput::BUT_C);
	s->register_class_static<GameInput>("BUT_D", GameInput::BUT_D);
	s->register_class_static<GameInput>("BUT_E", GameInput::BUT_E);
	s->register_class_static<GameInput>("BUT_F", GameInput::BUT_F);
	s->register_class_static<GameInput>("BUT_G", GameInput::BUT_G);
	s->register_class_static<GameInput>("BUT_H", GameInput::BUT_H);
	s->register_class_static<GameInput>("BUT_I", GameInput::BUT_I);
	s->register_class_static<GameInput>("BUT_J", GameInput::BUT_J);
	s->register_class_static<GameInput>("BUT_K", GameInput::BUT_K);
	s->register_class_static<GameInput>("BUT_L", GameInput::BUT_L);
	s->register_class_static<GameInput>("BUT_M", GameInput::BUT_M);
	s->register_class_static<GameInput>("BUT_N", GameInput::BUT_N);
	s->register_class_static<GameInput>("BUT_O", GameInput::BUT_O);
	s->register_class_static<GameInput>("BUT_P", GameInput::BUT_P);
	s->register_class_static<GameInput>("BUT_Q", GameInput::BUT_Q);
	s->register_class_static<GameInput>("BUT_R", GameInput::BUT_R);
	s->register_class_static<GameInput>("BUT_S", GameInput::BUT_S);
	s->register_class_static<GameInput>("BUT_T", GameInput::BUT_T);
	s->register_class_static<GameInput>("BUT_U", GameInput::BUT_U);
	s->register_class_static<GameInput>("BUT_V", GameInput::BUT_V);
	s->register_class_static<GameInput>("BUT_W", GameInput::BUT_W);
	s->register_class_static<GameInput>("BUT_X", GameInput::BUT_X);
	s->register_class_static<GameInput>("BUT_Y", GameInput::BUT_Y);
	s->register_class_static<GameInput>("BUT_Z", GameInput::BUT_Z);
	s->register_class_static<GameInput>("BUT_DPAD_U", GameInput::BUT_DPAD_U);
	s->register_class_static<GameInput>("BUT_DPAD_D", GameInput::BUT_DPAD_D);
	s->register_class_static<GameInput>("BUT_DPAD_L", GameInput::BUT_DPAD_L);
	s->register_class_static<GameInput>("BUT_DPAD_R", GameInput::BUT_DPAD_R);
	s->register_class_static<GameInput>("BUT_0", GameInput::BUT_0);
	s->register_class_static<GameInput>("BUT_1", GameInput::BUT_1);
	s->register_class_static<GameInput>("BUT_2", GameInput::BUT_2);
	s->register_class_static<GameInput>("BUT_3", GameInput::BUT_3);
	s->register_class_static<GameInput>("BUT_4", GameInput::BUT_4);
	s->register_class_static<GameInput>("BUT_5", GameInput::BUT_5);
	s->register_class_static<GameInput>("BUT_6", GameInput::BUT_6);
	s->register_class_static<GameInput>("BUT_7", GameInput::BUT_7);
	s->register_class_static<GameInput>("BUT_8", GameInput::BUT_8);
	s->register_class_static<GameInput>("BUT_9", GameInput::BUT_9);
	s->register_class_static<GameInput>("BUT_ESC", GameInput::BUT_ESC);
	s->register_class_static<GameInput>("BUT_SPACE", GameInput::BUT_SPACE);
	s->register_class_static<GameInput>("BUT_RETURN", GameInput::BUT_RETURN);
	s->register_class_static<GameInput>("BUT_LSHIFT", GameInput::BUT_LSHIFT);
	s->register_class_static<GameInput>("BUT_RSHIFT", GameInput::BUT_RSHIFT);
	s->register_class_static<GameInput>("BUT_LCTRL", GameInput::BUT_LCTRL);
	s->register_class_static<GameInput>("BUT_RCTRL", GameInput::BUT_RCTRL);
	s->register_class_static<GameInput>("BUT_LALT", GameInput::BUT_LALT);
	s->register_class_static<GameInput>("BUT_RALT", GameInput::BUT_RALT);
	s->register_class_static<GameInput>("BUT_BACKQUOTE", GameInput::BUT_BACKQUOTE);
    s->register_class_static<GameInput>("BUT_BACKSLASH", GameInput::BUT_BACKSLASH);
    s->register_class_static<GameInput>("BUT_FORWARDSLASH", GameInput::BUT_FORWARDSLASH);
	s->register_class_static<GameInput>("BUT_MAX", GameInput::BUT_MAX);


	//Direction::DIR4
	typedef OOLUA::Direction Direction;
	s->register_class_static<Direction>("DIR4_CENTER", Direction::DIR4_CENTER);
	s->register_class_static<Direction>("DIR4_LEFT", Direction::DIR4_LEFT);
	s->register_class_static<Direction>("DIR4_RIGHT", Direction::DIR4_RIGHT);
	s->register_class_static<Direction>("DIR4_UP", Direction::DIR4_UP);
	s->register_class_static<Direction>("DIR4_DOWN", Direction::DIR4_DOWN);

	//Font Alignment
	typedef OOLUA::MenuText MenuText;
	s->register_class_static<MenuText>("L_ALIGN", elysian::Font::L_ALIGN);
	s->register_class_static<MenuText>("R_ALIGN", elysian::Font::R_ALIGN);
	s->register_class_static<MenuText>("C_ALIGN", elysian::Font::C_ALIGN);

	//MenuEntity Size Type
	typedef OOLUA::MenuEntity MenuEntity;
	s->register_class_static<MenuEntity>("SIZE_ABSOLUTE", MenuEntity::SIZE_ABSOLUTE);
	s->register_class_static<MenuEntity>("SIZE_RELATIVE", MenuEntity::SIZE_RELATIVE);
}


// ======= C GLOBAL AND C++ STATIC FUNCTION WRAPPERS ======

static inline int print(lua_State* const l) {
	char string[GYRO_PRINTF_BUFFER_SIZE] = {0};
	unsigned n = lua_gettop(l);
	const char *curString;

	for(unsigned i = 1; i <= n; ++i) {
		int t = lua_type(l, i);

		switch(t) {
			case LUA_TSTRING:
			case LUA_TNUMBER:
			//case LUA_TBOOLEAN:
				curString = lua_tostring(l, i);
				strcat(string, curString);
				break;
			case LUA_TBOOLEAN:
				curString = (lua_toboolean(l, i))? "true" : "false";
				strcat(string, curString);
				break;
			default:
                curString = lua_typename(l, t);
                strcat(string, curString);

		}

		strcat(string, " ");

	}

	elysian::Debug::luaPrint(string);

	return 1;
}

static inline int Singletons_getLevelSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::LevelSystem*, elysian::Singletons::getLevelSys)
}

static inline int Singletons_getCurArea(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::Area*, elysian::Singletons::getCurArea)
}

static inline int Singletons_getCurLevel(lua_State* const vm) {
	OOLUA_C_FUNCTION_0(elysian::Level*, elysian::Singletons::getCurLevel)
}

static inline int Singletons_getGameInput(lua_State* const vm) {
	OOLUA_C_FUNCTION_0(elysian::GameInput*, elysian::Singletons::getGameInput)
}

static inline int Singletons_setProcessInput(lua_State* const vm) {
	OOLUA_C_FUNCTION(void, elysian::Singletons::setProcessInput, bool)
}

static inline int Singletons_getAnimationSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::AnimationSystem*, elysian::Singletons::getAnimSys)
}

static inline int Singletons_getPartySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::PartySystem*, elysian::Singletons::getPartySys)
}

static inline int Singletons_getAudioSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::AudioSystem*, elysian::Singletons::getAudioSys)
}

static inline int Singletons_getLightSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::LightSystem*, elysian::Singletons::getLightSys)
}

static inline int Singletons_getParticleSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::ParticleSystem*, elysian::Singletons::getPartSys)
}

static inline int Singletons_getDaySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::DaySystem*, elysian::Singletons::getDaySys)
}

static inline int Singletons_getDialogSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::DialogSystem*, elysian::Singletons::getDialogSys)
}

static inline int Singletons_getGuiSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::GuiSystem*, elysian::Singletons::getGuiSys)
}

static inline int Singletons_getTimerSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::TimerSystem*, elysian::Singletons::getTimerSys)
}

static inline int Singletons_getRigidBodySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::RigidBodySystem*, elysian::Singletons::getRigidBodySys)
}

static inline int Singletons_getCombatSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CombatSystem*, elysian::Singletons::getCombatSys)
}

static inline int Singletons_getCameraSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CameraSystem*, elysian::Singletons::getCameraSys)
}

static inline int Singletons_getCharacterSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CharacterSystem*, elysian::Singletons::getCharSys)
}

static inline int EventMonitor_null(lua_State *const vm) {
	OOLUA_C_FUNCTION(elysian::EventMonitor*, elysian::EventMonitor::null)
}

static inline int gyroWavCreate(lua_State *const vm) {
	OOLUA_C_FUNCTION(GYWav*, gyAudWavCreate, const char*)
}

static inline int gyroWavDestroy(lua_State *const vm) {
	OOLUA_C_FUNCTION(int, gyAudWavDestroy, GYWav*)
}

static inline int gyroMatPush(lua_State *const vm) {
    OOLUA_C_FUNCTION(void, gyMatPush)
}

//===== GYRO MATRIX STACK ====

static inline int gyroMatPop(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatPop)
}

static inline int gyroMatLoadIdentity(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatLoadIdentity)
}

static inline int gyroMatTranslate(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatTranslate, const float, const float, const float)
}

static inline int gyroMatRotateZ(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatRotateZ, const float)
}

static inline int gyroMatScale(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatScale, const float, const float, const float)
}

static inline int gyroMatTranslateScale(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatTranslateScale, const float, const float, const float, const float, const float, const float)
}

/*
static inline int gyroMatInstance(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatInstance, const GYInstMat *const)
}
*/

static inline int gyroMatRotate(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatRotate, const float, const float, const float, const float)
}

static inline int gyroMatShearZ(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatShearZ, const float, const float)
}

//==== GYRO VECTOR MATH ====
/*
static inline int gyroMatMultMat(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatMultMat, GYMatrix4, const GYMatrix4, const GYMatrix4)
}*/
/*
static inline int gyroMatMultVec(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatMultVec, const GYVector4*const, GYVector4*, const unsigned)
}

static inline int gyroVec4Add(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Add, void*, const void* const, const void* const)
}*/
/*
static inline int gyroVec4Mult(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Mult, const void*const, const void* const)
}

static inline int gyroVec4Add(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Add, void*, const void* const, const void* const)
}*/


#endif
