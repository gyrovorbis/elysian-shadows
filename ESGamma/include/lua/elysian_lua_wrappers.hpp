#ifndef ELYSIAN_LUA_WRAPPERS_HPP
#define ELYSIAN_LUA_WRAPPERS_HPP

#include <oolua.h>
#include <utility/elysian_event_monitor.hpp>
#include <system/elysian_gui_system_base.hpp>
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
#include <proxy/elysian_input_system_proxy.hpp>
#include <gui/elysian_layout.hpp>
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
    typedef elysian::Engine     Engine;
}




OOLUA_PROXY(Engine)
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

    //Keyboard::KEYS
    typedef elysian::Keyboard Keyboard;
    s->register_class_static<Keyboard>("KEY_NONE", GY_KEY_NONE);
    s->register_class_static<Keyboard>("KEY_A", GY_KEY_A);
    s->register_class_static<Keyboard>("KEY_B", GY_KEY_B);
    s->register_class_static<Keyboard>("KEY_C", GY_KEY_C);
    s->register_class_static<Keyboard>("KEY_D", GY_KEY_D);
    s->register_class_static<Keyboard>("KEY_E", GY_KEY_E);
    s->register_class_static<Keyboard>("KEY_F", GY_KEY_F);
    s->register_class_static<Keyboard>("KEY_G", GY_KEY_G);
    s->register_class_static<Keyboard>("KEY_H", GY_KEY_H);
    s->register_class_static<Keyboard>("KEY_I", GY_KEY_I);
    s->register_class_static<Keyboard>("KEY_J", GY_KEY_J);
    s->register_class_static<Keyboard>("KEY_K", GY_KEY_K);
    s->register_class_static<Keyboard>("KEY_L", GY_KEY_L);
    s->register_class_static<Keyboard>("KEY_M", GY_KEY_M);
    s->register_class_static<Keyboard>("KEY_N", GY_KEY_N);
    s->register_class_static<Keyboard>("KEY_O", GY_KEY_O);
    s->register_class_static<Keyboard>("KEY_P", GY_KEY_P);
    s->register_class_static<Keyboard>("KEY_Q", GY_KEY_Q);
    s->register_class_static<Keyboard>("KEY_R", GY_KEY_R);
    s->register_class_static<Keyboard>("KEY_S", GY_KEY_S);
    s->register_class_static<Keyboard>("KEY_T", GY_KEY_T);
    s->register_class_static<Keyboard>("KEY_U", GY_KEY_U);
    s->register_class_static<Keyboard>("KEY_V", GY_KEY_V);
    s->register_class_static<Keyboard>("KEY_W", GY_KEY_W);
    s->register_class_static<Keyboard>("KEY_X", GY_KEY_X);
    s->register_class_static<Keyboard>("KEY_Y", GY_KEY_Y);
    s->register_class_static<Keyboard>("KEY_Z", GY_KEY_Z);
    s->register_class_static<Keyboard>("KEY_DPAD_U", GY_KEY_DPAD_U);
    s->register_class_static<Keyboard>("KEY_DPAD_D", GY_KEY_DPAD_D);
    s->register_class_static<Keyboard>("KEY_DPAD_L", GY_KEY_DPAD_L);
    s->register_class_static<Keyboard>("KEY_DPAD_R", GY_KEY_DPAD_R);
    s->register_class_static<Keyboard>("KEY_SPACE", GY_KEY_SPACE);
    s->register_class_static<Keyboard>("KEY_RETURN", GY_KEY_RETURN);
    s->register_class_static<Keyboard>("KEY_ESC", GY_KEY_ESC);
    s->register_class_static<Keyboard>("KEY_BACKSLASH", GY_KEY_BACKSLASH);
    s->register_class_static<Keyboard>("KEY_FORWARDSLASH", GY_KEY_FORWARDSLASH);
    s->register_class_static<Keyboard>("KEY_LSHIFT", GY_KEY_LSHIFT);
    s->register_class_static<Keyboard>("KEY_RSHIFT", GY_KEY_RSHIFT);
    s->register_class_static<Keyboard>("KEY_LCTRL", GY_KEY_LCTRL);
    s->register_class_static<Keyboard>("KEY_RCTRL", GY_KEY_RCTRL);
    s->register_class_static<Keyboard>("KEY_LALT", GY_KEY_LALT);
    s->register_class_static<Keyboard>("KEY_RALT", GY_KEY_RALT);
    s->register_class_static<Keyboard>("KEY_BACKQUOTE", GY_KEY_BACKQUOTE);
    s->register_class_static<Keyboard>("KEY_COUNT", GY_KEY_COUNT);

    typedef elysian::Controller Controller;
    s->register_class_static<Controller>("BUT_DPAD_UP", GY_CONTROLLER_BUT_DPAD_UP);
    s->register_class_static<Controller>("BUT_DPAD_DOWN", GY_CONTROLLER_BUT_DPAD_DOWN);
    s->register_class_static<Controller>("BUT_DPAD_LEFT", GY_CONTROLLER_BUT_DPAD_LEFT);
    s->register_class_static<Controller>("BUT_DPAD_RIGHT", GY_CONTROLLER_BUT_DPAD_RIGHT);
    s->register_class_static<Controller>("BUT_START", GY_CONTROLLER_BUT_START);
    s->register_class_static<Controller>("BUT_SELECT", GY_CONTROLLER_BUT_SELECT);
    s->register_class_static<Controller>("BUT_HOME", GY_CONTROLLER_BUT_HOME);
    s->register_class_static<Controller>("BUT_LSHOULDER", GY_CONTROLLER_BUT_LSHOULDER);
    s->register_class_static<Controller>("BUT_RSHOULDER", GY_CONTROLLER_BUT_RSHOULDER);
    s->register_class_static<Controller>("BUT_LANALOG", GY_CONTROLLER_BUT_LANALOG);
    s->register_class_static<Controller>("BUT_RANALOG", GY_CONTROLLER_BUT_RANALOG);
    s->register_class_static<Controller>("BUT_X", GY_CONTROLLER_BUT_X);
    s->register_class_static<Controller>("BUT_SQUARE", GY_CONTROLLER_BUT_SQUARE);
    s->register_class_static<Controller>("BUT_CIRCLE", GY_CONTROLLER_BUT_CIRCLE);
    s->register_class_static<Controller>("BUT_TRIANGLE", GY_CONTROLLER_BUT_TRIANGLE);
    s->register_class_static<Controller>("BUT_COUNT", GY_CONTROLLER_BUT_COUNT);

    typedef elysian::InputSystem InputSystem;
    s->register_class_static<InputSystem>("BUT_STATE_UP", GY_BUT_STATE_UP);
    s->register_class_static<InputSystem>("BUT_STATE_TAPPED", GY_BUT_STATE_TAPPED);
    s->register_class_static<InputSystem>("BUT_STATE_RELEASED", GY_BUT_STATE_RELEASED);
    s->register_class_static<InputSystem>("BUT_STATE_DOWN", GY_BUT_STATE_DOWN);

	//Direction::DIR4
    typedef elysian::Direction Direction;
	s->register_class_static<Direction>("DIR4_CENTER", Direction::DIR4_CENTER);
	s->register_class_static<Direction>("DIR4_LEFT", Direction::DIR4_LEFT);
	s->register_class_static<Direction>("DIR4_RIGHT", Direction::DIR4_RIGHT);
	s->register_class_static<Direction>("DIR4_UP", Direction::DIR4_UP);
	s->register_class_static<Direction>("DIR4_DOWN", Direction::DIR4_DOWN);

    //Character::ANIM_STATE
    typedef elysian::Character Character;
    s->register_class_static<Character>("STATE_STAND", Character::STATE_STAND);
    s->register_class_static<Character>("STATE_WALK", Character::STATE_WALK);
    s->register_class_static<Character>("STATE_RUN", Character::STATE_RUN);
    s->register_class_static<Character>("STATE_DASH", Character::STATE_DASH);
    s->register_class_static<Character>("STATE_JUMP", Character::STATE_JUMP);
    s->register_class_static<Character>("STATE_FALL", Character::STATE_FALL);
    s->register_class_static<Character>("STATE_PUSH", Character::STATE_PUSH);
    s->register_class_static<Character>("STATE_DRAG", Character::STATE_DRAG);
    s->register_class_static<Character>("STATE_HOLD", Character::STATE_HOLD);
    s->register_class_static<Character>("STATE_THROW", Character::STATE_THROW);
    s->register_class_static<Character>("STATE_COMBAT", Character::STATE_COMBAT);
    s->register_class_static<Character>("STATE_STRAFE", Character::STATE_STRAFE);
    s->register_class_static<Character>("STATE_KNOCKBACK", Character::STATE_KNOCKBACK);
    s->register_class_static<Character>("STATE_DEATH", Character::STATE_DEATH);
    s->register_class_static<Character>("STATE_LUA", Character::STATE_LUA);

    typedef elysian::Layout Layout;
    s->register_class_static<Layout>("ALIGN_NEGATIVE", Layout::ALIGN_NEGATIVE);
    s->register_class_static<Layout>("ALIGN_CENTER", Layout::ALIGN_CENTER);
    s->register_class_static<Layout>("ALIGN_POSITIVE", Layout::ALIGN_POSITIVE);
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
            default: {
                elysian::LuaVariant v = elysian::LuaVariant::fromStack(i, false);
                //curString = lua_typename(l, t);
                strcat(string, v.asString().getValue<const char*>());
            }
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

static inline int Singletons_setProcessInput(lua_State* const vm) {
	OOLUA_C_FUNCTION(void, elysian::Singletons::setProcessInput, bool)
}

static inline int Singletons_getPartySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::PartySystem*, elysian::Singletons::getPartySys)
}

static inline int Singletons_getInputSystem(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::InputSystem*, elysian::Singletons::getInputSys)
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

static inline int Direction_fromVector2(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::Direction::DIR4, elysian::Direction::fromVector2, const GYVector2&)
}

static inline int Direction_toVector2(lua_State* const vm) {
    OOLUA_C_FUNCTION(GYVector2, elysian::Direction::toVector2, elysian::Direction::DIR4)
}

static inline int Direction_toVector3(lua_State* const vm) {
    OOLUA_C_FUNCTION(GYVector3, elysian::Direction::toVector3, elysian::Direction::DIR4, float)
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
