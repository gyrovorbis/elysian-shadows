#ifndef ELYSIAN_LUA_WRAPPERS_HPP
#define ELYSIAN_LUA_WRAPPERS_HPP

#include <oolua.h>
#include <utility/elysian_game_input.hpp>
#include <utility/elysian_event_monitor.hpp>
#include <system/elysian_gui_system.hpp>
#include <lua/elysian_lua_singleton.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_debug.hpp>
#include <level/elysian_tile.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <crtp/elysian_combat.hpp>
#include <crtp/elysian_character.hpp>
#include <crtp/elysian_behavior.hpp>
#include <level/elysian_map_depth_buffer.hpp>
#include <gyro_shader_api.h>


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
 *
 * Now go forth unto the world with the power of Lua, my children.
 *				--Falco
 */


// ====== C/++ CLASS/STRUCT WRAPPERS ======

namespace OOLUA
{
    typedef elysian::Singletons     Singletons;
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
    s->register_class_static<gyro>("RECT_PLANE_FRONT_SLOPE_45", GY_RECT_PLANE_FRONT_SLOPE_45);
    s->register_class_static<gyro>("RECT_PLANE_LEFT_SLOPE_45", GY_RECT_PLANE_LEFT_SLOPE_45);
    s->register_class_static<gyro>("RECT_PLANE_RIGHT_SLOPE_45", GY_RECT_PLANE_RIGHT_SLOPE_45);

    s->register_class_static<gyro>("MOUSE_BUT_LEFT", GY_MOUSE_BUT_LEFT);
    s->register_class_static<gyro>("MOUSE_BUT_RIGHT", GY_MOUSE_BUT_RIGHT);
    s->register_class_static<gyro>("MOUSE_BUT_MIDDLE", GY_MOUSE_BUT_MIDDLE);

	//Entity::DEPTH_TYPE
//	typedef OOLUA::Entity Entity;
    s->register_class_static<OOLUA::Entity>("NORMAL_FLAT", elysian::NORMAL_FLAT);
    s->register_class_static<OOLUA::Entity>("NORMAL_FRONT", elysian::NORMAL_FRONT);

	//CollisionFlags::TYPE
    typedef elysian::CollisionFlags CollisionFlags;
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
    typedef elysian::Combat Combat;
	s->register_class_static<Combat>("DMG_NONE", Combat::DMG_NONE);
	s->register_class_static<Combat>("DMG_SOURCE", Combat::DMG_SOURCE);
	s->register_class_static<Combat>("DMG_TARGET", Combat::DMG_TARGET);
	s->register_class_static<Combat>("DMG_SOURCE_TARGET", Combat::DMG_SOURCE_TARGET);

    s->register_class_static<Combat>("TARGET_NONE", Combat::TARGET_NONE);
    s->register_class_static<Combat>("TARGET_PARTY", Combat::TARGET_PARTY);
    s->register_class_static<Combat>("TARGET_FRIENDLY", Combat::TARGET_FRIENDLY);
    s->register_class_static<Combat>("TARGET_ENEMY", Combat::TARGET_ENEMY);

	s->register_class_static<Combat>("ON_COLLIDE", Combat::ON_COLLIDE);
	s->register_class_static<Combat>("ON_COLLISION_ENTER", Combat::ON_COLLISION_ENTER);
	s->register_class_static<Combat>("ON_COLLISION_EXIT", Combat::ON_COLLISION_EXIT);

    //CombatStatus::STATUS_EFFECT
    typedef elysian::CombatStatus CombatStatus;
    s->register_class_static<CombatStatus>("POISON", CombatStatus::POISON);
    s->register_class_static<CombatStatus>("CONFUSION", CombatStatus::CONFUSION);
    s->register_class_static<CombatStatus>("SLEEP", CombatStatus::SLEEP);

	//Behavior::TRIGGER
    typedef elysian::Behavior Behavior;
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
    s->register_class_static<Behavior>("ON_BUTTON_PRESS", Behavior::ON_BUTTON_PRESS);
    s->register_class_static<Behavior>("ON_BUTTON_RELEASE", Behavior::ON_BUTTON_RELEASE);
    s->register_class_static<Behavior>("ON_TK_SELECT", Behavior::ON_TK_SELECT);
    s->register_class_static<Behavior>("ON_TK_UNSELECT", Behavior::ON_TK_UNSELECT);

    s->register_class_static<Behavior>("ON_SCRIPT_CREATE_INDEX", Behavior::ON_SCRIPT_CREATE_INDEX);
    s->register_class_static<Behavior>("ON_SCRIPT_DESTROY_INDEX", Behavior::ON_SCRIPT_DESTROY_INDEX);
    s->register_class_static<Behavior>("ON_CREATE_INDEX", Behavior::ON_CREATE_INDEX);
    s->register_class_static<Behavior>("ON_DESTROY_INDEX", Behavior::ON_DESTROY_INDEX);
    s->register_class_static<Behavior>("ON_LOAD_INDEX", Behavior::ON_LOAD_INDEX);
    s->register_class_static<Behavior>("ON_UNLOAD_INDEX", Behavior::ON_UNLOAD_INDEX);
    s->register_class_static<Behavior>("ON_ENTER_INDEX", Behavior::ON_ENTER_INDEX);
    s->register_class_static<Behavior>("ON_EXIT_INDEX", Behavior::ON_EXIT_INDEX);
    s->register_class_static<Behavior>("ON_UPDATE_INDEX", Behavior::ON_UPDATE_INDEX);
    s->register_class_static<Behavior>("ON_RENDER_INDEX", Behavior::ON_RENDER_INDEX);
    s->register_class_static<Behavior>("ON_COLLIDE_INDEX", Behavior::ON_COLLIDE_INDEX);
    s->register_class_static<Behavior>("ON_COLLISION_ENTER_INDEX", Behavior::ON_COLLISION_ENTER_INDEX);
    s->register_class_static<Behavior>("ON_COLLISION_EXIT_INDEX", Behavior::ON_COLLISION_EXIT_INDEX);
    s->register_class_static<Behavior>("ON_EXAMINE_INDEX", Behavior::ON_EXAMINE_INDEX);
    s->register_class_static<Behavior>("ON_DIALOG_INDEX", Behavior::ON_DIALOG_INDEX);
    s->register_class_static<Behavior>("ON_COMBAT_ENTER_INDEX", Behavior::ON_COMBAT_ENTER_INDEX);
    s->register_class_static<Behavior>("ON_COMBAT_EXIT_INDEX", Behavior::ON_COMBAT_EXIT_INDEX);
    s->register_class_static<Behavior>("ON_COMBAT_DEATH_INDEX", Behavior::ON_COMBAT_DEATH_INDEX);
    s->register_class_static<Behavior>("ON_SHATTER_INDEX", Behavior::ON_SHATTER_INDEX);
    s->register_class_static<Behavior>("ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX", Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX);
    s->register_class_static<Behavior>("ON_COMBAT_TARGET_COLLISION_ENTER_INDEX", Behavior::ON_COMBAT_TARGET_COLLISION_ENTER_INDEX);
    s->register_class_static<Behavior>("ON_BUTTON_PRESS_INDEX", Behavior::ON_BUTTON_PRESS_INDEX);
    s->register_class_static<Behavior>("ON_BUTTON_RELEASE_INDEX", Behavior::ON_BUTTON_RELEASE_INDEX);
    s->register_class_static<Behavior>("ON_TK_SELECT_INDEX", Behavior::ON_TK_SELECT_INDEX);
    s->register_class_static<Behavior>("ON_TK_UNSELECT_INDEX", Behavior::ON_TK_UNSELECT_INDEX);
    s->register_class_static<Behavior>("TRIGGER_MAX_INDEX", Behavior::TRIGGER_MAX_INDEX);

	//Tile::COLLIDER_TYPE
    typedef elysian::Tile Tile;
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
    typedef elysian::GameInput GameInput;
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
    typedef elysian::Direction Direction;
	s->register_class_static<Direction>("DIR4_CENTER", Direction::DIR4_CENTER);
	s->register_class_static<Direction>("DIR4_LEFT", Direction::DIR4_LEFT);
	s->register_class_static<Direction>("DIR4_RIGHT", Direction::DIR4_RIGHT);
	s->register_class_static<Direction>("DIR4_UP", Direction::DIR4_UP);
	s->register_class_static<Direction>("DIR4_DOWN", Direction::DIR4_DOWN);

	//Font Alignment
    typedef elysian::MenuText MenuText;
	s->register_class_static<MenuText>("L_ALIGN", elysian::Font::L_ALIGN);
	s->register_class_static<MenuText>("R_ALIGN", elysian::Font::R_ALIGN);
	s->register_class_static<MenuText>("C_ALIGN", elysian::Font::C_ALIGN);

	//MenuEntity Size Type
    typedef elysian::MenuEntity MenuEntity;
	s->register_class_static<MenuEntity>("SIZE_ABSOLUTE", MenuEntity::SIZE_ABSOLUTE);
	s->register_class_static<MenuEntity>("SIZE_RELATIVE", MenuEntity::SIZE_RELATIVE);
}


// ======= C GLOBAL AND C++ STATIC FUNCTION WRAPPERS ======

static inline int dbgPush(lua_State* const /*l*/) {
    elysian::Debug::push();
    return 0;
}

static inline int dbgPop(lua_State* const /*l*/) {
    elysian::Debug::pop();
    return 0;
}

static inline const char* luaGetVariableString(lua_State* const l) {
    static char string[GYRO_PRINTF_BUFFER_SIZE] = {0};
    unsigned n = lua_gettop(l);
    const char *curString;

    string[0] = 0;
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

    return string;

}

static inline int print(lua_State* const l) {


    elysian::LuaManager::luaPrint(luaGetVariableString(l));

    return 0;
}

static inline int printAdvLog(lua_State* const l) {
    elysian::Singletons::getGuiSys()->adventureLog.addLine(elysian::AdventureLog::TEXT_NORMAL, luaGetVariableString(l));
    return 0;
}

static inline int Singletons_getLevelSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::LevelSystem*, elysian::Singletons::getLevelSys)
}

static inline int Singletons_getCurArea(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::Area*, elysian::Singletons::getCurrentArea)
}

static inline int Singletons_getCurLevel(lua_State* const vm) {
    OOLUA_C_FUNCTION_0(elysian::Level*, elysian::Singletons::getCurrentLevel)
}

static inline int Singletons_getGameInput(lua_State* const vm) {
	OOLUA_C_FUNCTION_0(elysian::GameInput*, elysian::Singletons::getGameInput)
}

static inline int Singletons_getMouseInput(lua_State* const vm) {
    OOLUA_C_FUNCTION_0(elysian::MouseInput*, elysian::Singletons::getMouseInput)
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
