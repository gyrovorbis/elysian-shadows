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
#include "elysian_character.hpp"
#include "elysian_warp.hpp"
#include "elysian_lua_behavior.hpp"
#include "elysian_lua_singleton.hpp"
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"
#include "elysian_party_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_audio_source.hpp"

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
    typedef elysian::InstanceTransform		InstanceTransform;
	typedef elysian::RenderableObject		RenderableObject;
	typedef elysian::Component				Component;
	typedef elysian::Entity					Entity;
	typedef elysian::RectAttrib				RectAttrib;
	typedef elysian::TexturedAttrib			TexturedAttrib;
	typedef elysian::SpriteAttrib			SpriteAttrib;
	typedef elysian::Renderable				Renderable;
	typedef elysian::AnimationSequence		AnimationSequence;
	typedef elysian::AnimationAttrib		AnimationAttrib;
	typedef elysian::Animation				Animation;
	typedef elysian::CollisionFlags			CollisionFlags;
	typedef elysian::CollidableAttrib		CollidableAttrib;
	typedef elysian::Collidable				Collidable;
	typedef elysian::Character				Character;
	typedef elysian::Warp					Warp;
	typedef elysian::Behavior				Behavior;
	typedef elysian::LuaBehavior			LuaBehavior;
	typedef elysian::Tile					Tile;
	typedef elysian::Level					Level;
	typedef elysian::MapEntry				MapEntry;
	typedef elysian::TileEntry				TileEntry;
	typedef elysian::Area					Area;
	typedef elysian::GameInput				GameInput;
	typedef elysian::PartySystem			PartySystem;
	typedef elysian::TimerSystem			TimerSystem;
	typedef elysian::Singletons				Singletons;
	typedef elysian::AudioSource			AudioSource;
}

OOLUA_CLASS_NO_BASES(GYVector2)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_PUBLIC_MEMBER_GET_SET(x)
	OOLUA_PUBLIC_MEMBER_GET_SET(y)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(GYWav)
OOLUA_TYPEDEFS
	No_public_constructors,
	No_public_destructor
OOLUA_TYPEDEFS_END
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(InstanceTransform)
	OOLUA_NO_TYPEDEFS
    OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_0_CONST(void, apply)
	OOLUA_MEM_FUNC_2_CONST_RENAME(getPos2f, void, getPos, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_3_CONST_RENAME(getPos3f, void, getPos, out_p<float&>, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_2_RENAME(setPos2f, void, setPos, float, float)
	OOLUA_MEM_FUNC_3_RENAME(setPos3f, void, setPos, float, float, float)
	OOLUA_MEM_FUNC_2_RENAME(translate2f, void, translate, float, float)
	OOLUA_MEM_FUNC_3_RENAME(translate3f, void, translate, float, float, float)
	OOLUA_MEM_FUNC_2_RENAME(setSize2f, void, setSize, float, float)
	OOLUA_MEM_FUNC_2_RENAME(setScale2f, void, setScale, float, float)
	OOLUA_MEM_FUNC_1(void, setOrientZ, float)
	OOLUA_MEM_FUNC_1(void, rotate, float)
	OOLUA_MEM_FUNC_2_RENAME(scale2f, void, scale, float, float)
	OOLUA_MEM_FUNC_2_CONST_RENAME(getSize2f, void, getSize, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_2_CONST_RENAME(getScale2f, void, getScale, out_p<float&>, out_p<float&>)
	OOLUA_MEM_FUNC_0_CONST(float, getOrientZ)

OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(RenderableObject)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor,
		Abstract
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_0(InstanceTransform&, getTransform)
OOLUA_CLASS_END

namespace OOLUA {
    template<> class Proxy_class<elysian::Renderable>;
    template<> class Proxy_class<elysian::Animation>;
    template<> class Proxy_class<elysian::Collidable>;
    template<> class Proxy_class<elysian::Character>;
    template<> class Proxy_class<elysian::Warp>;
	template<> class Proxy_class<elysian::AudioSource>;
    template<> class Proxy_class<elysian::Behavior>;
    template<> class Proxy_class<elysian::LuaBehavior>;
}
OOLUA_CLASS(Entity): public Proxy_class<RenderableObject>
	OOLUA_BASIC	
	OOLUA_NO_TYPEDEFS
	OOLUA_BASES_START 
		RenderableObject
	OOLUA_BASES_END
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_0(bool, activate)
	OOLUA_MEM_FUNC_0(bool, deactivate)
	OOLUA_MEM_FUNC_1(void, move, const GYVector2&)
	OOLUA_MEM_FUNC_1(void, setName, const char*)	
	OOLUA_MEM_FUNC_0_CONST(const char *, getName)
	OOLUA_MEM_FUNC_1(void, setElev, const unsigned char)
	OOLUA_MEM_FUNC_0_CONST(unsigned char, getElev)
	OOLUA_MEM_FUNC_0(Renderable*, addRenderable)
	OOLUA_MEM_FUNC_0(Animation*, addAnimation)
	OOLUA_MEM_FUNC_0(Collidable*, addCollidable)
	OOLUA_MEM_FUNC_0(Character*, addCharacter)
	OOLUA_MEM_FUNC_0(Warp*, addWarp)
	OOLUA_MEM_FUNC_0(AudioSource*, addAudioSource)
	OOLUA_MEM_FUNC_1(Behavior*, addBehavior, const char*)
	OOLUA_MEM_FUNC_1(LuaBehavior*, addLuaBehavior, const char*)
	OOLUA_MEM_FUNC_0_CONST(Renderable*, getRenderable);
	OOLUA_MEM_FUNC_0_CONST(Animation*, getAnimation);
	OOLUA_MEM_FUNC_0_CONST(Collidable*, getCollidable);
	OOLUA_MEM_FUNC_0_CONST(Character*, getCharacter);
	OOLUA_MEM_FUNC_0_CONST(Warp*, getWarp)
	OOLUA_MEM_FUNC_0_CONST(AudioSource*, getAudioSource)
	OOLUA_MEM_FUNC_1_CONST(Behavior*, getBehavior, const char*)
	OOLUA_MEM_FUNC_1_CONST(LuaBehavior*, getLuaBehavior, const char*)
	//OOLUA_MEM_FUNC_0(void, removeRenderable)
	//OOLUA_MEM_FUNC_0(void, removeAnimation)
	//OOLUA_MEM_FUNC_0(void, removeCollidable)
	//OOLUA_MEM_FUNC_0(void, removeCharacter)
	//OOLUA_MEM_FUNC_0(void, removeWarp)
	//OOLUA_MEM_FUNC_1(void, removeBehavior, const char*)
	//OOLUA_MEM_FUNC_1(void, removeLuaBehavior, const char*)


	/* TODO: Components and Move */
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Component)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor,
		Abstract
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_0_CONST(Entity*, getEntity)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(RectAttrib)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_4(void, setColor, unsigned char, unsigned char, unsigned char, unsigned char)
	OOLUA_MEM_FUNC_4_CONST(void, getColor, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>)
	OOLUA_MEM_FUNC_0_CONST(unsigned char, getAlpha)
	OOLUA_MEM_FUNC_1(void, setAlpha, unsigned char)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(TexturedAttrib)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_5(void, loadTexture, const char *, unsigned, unsigned, unsigned, unsigned)
	OOLUA_MEM_FUNC_3_RENAME(loadSheet, void, loadTexture, const char *, unsigned, unsigned)
OOLUA_CLASS_END

OOLUA_CLASS(SpriteAttrib):
	public Proxy_class<RectAttrib>,
	public Proxy_class<TexturedAttrib>
	OOLUA_BASIC
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_BASES_START
		RectAttrib,
		TexturedAttrib
	OOLUA_BASES_END
	OOLUA_MEM_FUNC_1(void, setFrame, unsigned)
OOLUA_CLASS_END

OOLUA_CLASS(Renderable): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_1_CONST(void, getOffset, GYVector2&)
	OOLUA_MEM_FUNC_1(void, setOffset, const GYVector2&)
	OOLUA_MEM_FUNC_0_CONST(unsigned, getFrame)
	OOLUA_MEM_FUNC_1(void, setFrame, unsigned)
	OOLUA_MEM_FUNC_0_CONST(SpriteAttrib*, getSpriteAttrib)
	OOLUA_MEM_FUNC_1(void, setSpriteAttrib, cpp_in_p<SpriteAttrib*>)
OOLUA_CLASS_END
	
OOLUA_CLASS_NO_BASES(AnimationSequence)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor,
		Abstract
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_4(void, addFrame, float, float, float, unsigned)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(AnimationAttrib)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_1(AnimationSequence*, addSequence, const char*)
OOLUA_CLASS_END

OOLUA_CLASS(Animation): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_0_CONST(AnimationAttrib*, getAttrib)
	OOLUA_MEM_FUNC_1(void, setAttrib, cpp_in_p<AnimationAttrib *>)
	OOLUA_MEM_FUNC_1(void, playAnimation, const char*)
	OOLUA_MEM_FUNC_0(void, stopAnimation)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(CollisionFlags)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_0(void, resetType)
	OOLUA_MEM_FUNC_0(void, resetCheckAgainst)
	OOLUA_MEM_FUNC_1(void, addType, uint32_t)
	OOLUA_MEM_FUNC_1(void, addCheckAgainst, uint32_t)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(GYStatic)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_PUBLIC_MEMBER_GET_SET(inverseMass)
	OOLUA_PUBLIC_MEMBER_GET_SET(coeffOfRest)
	OOLUA_PUBLIC_MEMBER_GET_SET(damping)
	OOLUA_PUBLIC_MEMBER_GET_SET(invMomOfInertia)
	OOLUA_PUBLIC_MEMBER_GET_SET(angDamping)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(CollidableAttrib)
	OOLUA_NO_TYPEDEFS
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_0_CONST(GY_COLLIDER_TYPE, getColliderType)
	OOLUA_MEM_FUNC_1(void, setColliderType, GY_COLLIDER_TYPE)
	OOLUA_MEM_FUNC_2(void, setRelPos, float, float)
	OOLUA_MEM_FUNC_2(void, setRelSize, float, float)
	OOLUA_MEM_FUNC_0(CollisionFlags*, getFlags)
	OOLUA_MEM_FUNC_0(GYStatic*, getStaticAttrib)
OOLUA_CLASS_END

OOLUA_CLASS(Collidable): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_0_CONST(CollidableAttrib*, getAttrib)
	OOLUA_MEM_FUNC_1(void, setAttrib, cpp_in_p<CollidableAttrib*>)
	OOLUA_MEM_FUNC_0_CONST(unsigned, getContactCount)
	OOLUA_MEM_FUNC_1(void, setResolvePenetration, bool)
OOLUA_CLASS_END

OOLUA_CLASS(Character): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_1(void, face, Entity&)
	OOLUA_MEM_FUNC_1(void, move, const GYVector2&)
OOLUA_CLASS_END

OOLUA_CLASS(Warp): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_1(void, setToPos, const GYVector2&)
	OOLUA_MEM_FUNC_1(void, setAreaPath, const char*)
	OOLUA_MEM_FUNC_1(void, setLevelPath, const char*)
	OOLUA_MEM_FUNC_1(void, setChangeArea, bool)
	OOLUA_MEM_FUNC_1(void, setChangeLevel, bool)
OOLUA_CLASS_END

OOLUA_CLASS(AudioSource): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_1(void, setWav, const GYWav*)
	OOLUA_MEM_FUNC_1_CONST(void, playWav, const GYWav*)
	OOLUA_MEM_FUNC_0_CONST(void, play)
	OOLUA_MEM_FUNC_0_CONST(void, stop)
	OOLUA_MEM_FUNC_1_CONST(void, setGain, float)
	OOLUA_MEM_FUNC_1_CONST(void, setPitch, float)
	OOLUA_MEM_FUNC_1_CONST(void, setLooping, bool)
OOLUA_CLASS_END

OOLUA_CLASS(Behavior): public Proxy_class<Component>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component OOLUA_BASES_END
	OOLUA_MEM_FUNC_1(void, addTrigger, Behavior::TRIGGER)
	OOLUA_MEM_FUNC_1(void, removeTrigger, Behavior::TRIGGER)
	OOLUA_MEM_FUNC_0_CONST(const char*, getBehavName)
OOLUA_CLASS_END

OOLUA_CLASS(LuaBehavior): public Proxy_class<Behavior>
	OOLUA_BASIC
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_BASES_START Component, Behavior OOLUA_BASES_END
	OOLUA_MEM_FUNC_0(Lua_table_ref&, getDataTable)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Tile)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_1(void, setColliderType, unsigned)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Level)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_1(Tile*, getTile, uint16_t)
	OOLUA_MEM_FUNC_0_CONST(unsigned short, getTileAmount)
	OOLUA_MEM_FUNC_0_CONST(const char*, getName)
	OOLUA_MEM_FUNC_0_CONST(const char*, getPath)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(MapEntry)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_0_CONST(uint32_t, getValue)
	OOLUA_MEM_FUNC_1(void, setValue, uint32_t)
	OOLUA_MEM_FUNC_0_CONST(GY_RECT_TEX_ORIENT, getOrient)
	OOLUA_MEM_FUNC_1(void, setOrient, GY_RECT_TEX_ORIENT)
	OOLUA_MEM_FUNC_0_CONST(unsigned char, getElev)
	OOLUA_MEM_FUNC_1(void, setElev, unsigned char)
OOLUA_CLASS_END

OOLUA_CLASS(TileEntry): public Proxy_class<MapEntry>
	OOLUA_BASIC
	OOLUA_NO_TYPEDEFS
	OOLUA_BASES_START MapEntry OOLUA_BASES_END
	OOLUA_ONLY_DEFAULT_CONSTRUCTOR
	OOLUA_MEM_FUNC_0_CONST(uint16_t, getIndex)
	OOLUA_MEM_FUNC_1(void, setIndex, uint16_t)
	OOLUA_MEM_FUNC_0_CONST(bool, getZTran)
	OOLUA_MEM_FUNC_1(void, setZTran, bool)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Area)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_0_CONST(const char*, getName)
	OOLUA_MEM_FUNC_0_CONST(const char*, getPath)
	OOLUA_MEM_FUNC_3_CONST(TileEntry, getTileEntry, unsigned, unsigned, unsigned)
	OOLUA_MEM_FUNC_4(void, setTileEntry, unsigned, unsigned, unsigned, TileEntry)
	OOLUA_MEM_FUNC_1(void, addEntity, cpp_in_p<Entity*>)
	OOLUA_MEM_FUNC_1(void, removeEntity, in_out_p<Entity*>)
	OOLUA_MEM_FUNC_1(Entity*, findEntity, const char*)
	OOLUA_MEM_FUNC_0_CONST(unsigned short, getMapWidth);
	OOLUA_MEM_FUNC_0_CONST(unsigned short, getMapHeight);
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(GameInput)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_1_CONST(bool, getTapped, GameInput::BUTTON)
	OOLUA_MEM_FUNC_1_CONST(bool, getDown, GameInput::BUTTON)
	OOLUA_MEM_FUNC_1(void, setUp, GameInput::BUTTON)
	OOLUA_MEM_FUNC_1(void, setDown, GameInput::BUTTON)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(PartySystem)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_1(bool, addCharacterToParty, Entity&)
	OOLUA_MEM_FUNC_1(bool, removeCharacterFromParty, const Entity&)
	OOLUA_MEM_FUNC_0_CONST(Entity*, getPartyLeader)
	OOLUA_MEM_FUNC_1_CONST(Entity*, getCharacterPartyMember, unsigned)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(TimerSystem)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
	OOLUA_MEM_FUNC_0_CONST(unsigned long, getTotalTicks)
	OOLUA_MEM_FUNC_0_CONST(unsigned long, getCurrentFrameTicks)
	OOLUA_MEM_FUNC_0_CONST(float, getDeltaTime)
OOLUA_CLASS_END

OOLUA_CLASS_NO_BASES(Singletons)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
OOLUA_CLASS_END

//dummy class for encapsulating libGyro within its own namespace
class gyro {}; 
OOLUA_CLASS_NO_BASES(gyro)
	OOLUA_TYPEDEFS
		No_public_constructors,
		No_public_destructor
	OOLUA_TYPEDEFS_END
OOLUA_CLASS_END


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

	//Behavior::TRIGGER
	typedef OOLUA::Behavior Behavior;
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
	s->register_class_static<GameInput>("BUT_MAX", GameInput::BUT_MAX);
	
}


// ======= C GLOBAL AND C++ STATIC FUNCTION WRAPPERS ======

static inline int print(lua_State* const l) {
	OOLUA_C_FUNCTION_1(void, elysian::Debug::print, const char*)
}

static inline int Singletons_getCurArea(lua_State* const l) {
	OOLUA_C_FUNCTION_0(elysian::Area*, elysian::Singletons::getCurArea)
}

static inline int Singletons_getCurLevel(lua_State* const l) {
	OOLUA_C_FUNCTION_0(elysian::Level*, elysian::Singletons::getCurLevel)
}

static inline int Singletons_getGameInput(lua_State* const l) {
	OOLUA_C_FUNCTION_0(elysian::GameInput*, elysian::Singletons::getGameInput)
}

static inline int Singletons_setProcessInput(lua_State* const l) {
	OOLUA_C_FUNCTION_1(void, elysian::Singletons::setProcessInput, bool)
}

static inline int Singletons_getPartySystem(lua_State* const l) {
	OOLUA_C_FUNCTION_0(elysian::PartySystem*, elysian::Singletons::getPartySys)
}

static inline int Singletons_getTimerSystem(lua_State* const l) {
	OOLUA_C_FUNCTION_0(elysian::TimerSystem*, elysian::Singletons::getTimerSys)
}

static inline int gyroFilePath(lua_State *const l) {
	OOLUA_C_FUNCTION_1(const char*, gyFilePath, const char*)
}

static inline int gyroWavCreate(lua_State *const l) {
	OOLUA_C_FUNCTION_1(GYWav*, gyAudWavCreate, const char*)
}

static inline int gyroWavDestroy(lua_State *const l) {
	OOLUA_C_FUNCTION_1(int, gyAudWavDestroy, GYWav*)
}

#endif
