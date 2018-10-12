#include <libGyro/gyro_shader_api.h>

#include <AssetIO/level/elysian_map_depth_buffer.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/proxy/elysian_level_proxy.hpp>
#include <AssetIO/proxy/elysian_area_proxy.hpp>
#include <AssetIO/proxy/elysian_bit_mask_proxy.hpp>
#include <AssetIO/proxy/elysian_event_proxy.hpp>

#include <ESGamma/gui/elysian_layout.hpp>
#include <ESGamma/proxy/elysian_input_system_proxy.hpp>
#include <ESGamma/proxy/elysian_widget_proxy.hpp>
#include <ESGamma/proxy/elysian_layout_proxy.hpp>
#include <ESGamma/proxy/elysian_gui_system_proxy.hpp>
#include <ESGamma/proxy/elysian_level_system_proxy.hpp>
#include <ESGamma/proxy/elysian_character_system_proxy.hpp>
#include <ESGamma/proxy/elysian_audio_system_proxy.hpp>
#include <ESGamma/proxy/elysian_camera_system_proxy.hpp>
#include <ESGamma/proxy/elysian_combat_system_proxy.hpp>
#include <ESGamma/proxy/elysian_day_system_proxy.hpp>
#include <ESGamma/proxy/elysian_dialog_system_proxy.hpp>
#include <ESGamma/proxy/elysian_light_system_proxy.hpp>
#include <ESGamma/proxy/elysian_particle_system_proxy.hpp>
#include <ESGamma/proxy/elysian_party_system_proxy.hpp>
#include <ESGamma/proxy/elysian_rigid_body_system_proxy.hpp>
#include <ESGamma/proxy/elysian_timer_system_proxy.hpp>
#include <ESGamma/proxy/elysian_event_system_proxy.hpp>
#include <ESGamma/proxy/elysian_collision_system_proxy.hpp>
#include <ESGamma/proxy/elysian_examinable_system_proxy.hpp>
#include <ESGamma/proxy/elysian_render_system_proxy.hpp>
#include <ESGamma/proxy/elysian_thread_system_proxy.hpp>
#include <ESGamma/proxy/elysian_compute_system_proxy.hpp>
#include <ESGamma/crtp/elysian_lua_manager.hpp>
#include <ESGamma/lua/elysian_lua_wrappers.hpp>

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
#   include <ESGamma/device/elysian_sensor_device.hpp>
#endif

namespace elysian {


    // ====== C/++ ENUMERATIONS AND CONSTANTS WRAPPERS ======
    static void luaRegisterEnumsConstants(OOLUA::Script *s) {

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

        //BlendMode
        typedef elysian::BlendMode BlendMode;
        s->register_class_static<BlendMode>("BLEND_ZERO",                       GY_BLEND_ZERO);
        s->register_class_static<BlendMode>("BLEND_ONE",                        GY_BLEND_ONE);
        s->register_class_static<BlendMode>("BLEND_SRC_COLOR",                  GY_BLEND_SRC_COLOR);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_SRC_COLOR",        GY_BLEND_ONE_MINUS_SRC_COLOR);
        s->register_class_static<BlendMode>("BLEND_DST_COLOR",                  GY_BLEND_DST_COLOR);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_DST_COLOR",        GY_BLEND_ONE_MINUS_DST_COLOR);
        s->register_class_static<BlendMode>("BLEND_SRC_ALPHA",                  GY_BLEND_SRC_ALPHA);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_SRC_ALPHA",        GY_BLEND_ONE_MINUS_SRC_ALPHA);
        s->register_class_static<BlendMode>("BLEND_DST_ALPHA",                  GY_BLEND_DST_ALPHA);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_DST_ALPHA",        GY_BLEND_ONE_MINUS_DST_ALPHA);
        s->register_class_static<BlendMode>("BLEND_CONSTANT_COLOR",             GY_BLEND_CONSTANT_COLOR);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_CONSTANT_COLOR",   GY_BLEND_ONE_MINUS_CONSTANT_COLOR);
        s->register_class_static<BlendMode>("BLEND_CONSTANT_ALPHA",             GY_BLEND_CONSTANT_ALPHA);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_CONSTANT_ALPHA",   GY_BLEND_ONE_MINUS_CONSTANT_ALPHA);
        s->register_class_static<BlendMode>("BLEND_SRC_ALPHA_SATURATE",         GY_BLEND_SRC_ALPHA_SATURATE);
        s->register_class_static<BlendMode>("BLEND_SRC1_COLOR",                 GY_BLEND_SRC1_COLOR);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_SRC1_COLOR",       GY_BLEND_ONE_MINUS_SRC1_COLOR);
        s->register_class_static<BlendMode>("BLEND_SRC1_ALPHA",                 GY_BLEND_SRC1_ALPHA);
        s->register_class_static<BlendMode>("BLEND_ONE_MINUS_SRC1_ALPHA",       GY_BLEND_ONE_MINUS_SRC1_ALPHA);

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

        //CombatSystem::COMBAT_STATE
        typedef elysian::CombatSystem CombatSystem;
        s->register_class_static<CombatSystem>("COMBAT_STATE_DISABLED", CombatSystem::COMBAT_STATE_DISABLED);
        s->register_class_static<CombatSystem>("COMBAT_STATE_DISABLED", CombatSystem::COMBAT_STATE_INACTIVE);
        s->register_class_static<CombatSystem>("COMBAT_STATE_DISABLED", CombatSystem::COMBAT_STATE_DISABLED);

        //CombatSystem::COMBAT_MGMT_TYPE
        s->register_class_static<CombatSystem>("COMBAT_MGMT_AUTO", CombatSystem::COMBAT_MGMT_AUTO);
        s->register_class_static<CombatSystem>("COMBAT_MGMT_MANUAL", CombatSystem::COMBAT_MGMT_MANUAL);

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

        s->register_class_static<Behavior>("ON_SCRIPT_CREATE_INDEX",                    Behavior::ON_SCRIPT_CREATE_INDEX);
        s->register_class_static<Behavior>("ON_SCRIPT_DESTROY_INDEX",                   Behavior::ON_SCRIPT_DESTROY_INDEX);
        s->register_class_static<Behavior>("ON_CREATE_INDEX",                           Behavior::ON_CREATE_INDEX);
        s->register_class_static<Behavior>("ON_DESTROY_INDEX",                          Behavior::ON_DESTROY_INDEX);
        s->register_class_static<Behavior>("ON_LOAD_INDEX",                             Behavior::ON_LOAD_INDEX);
        s->register_class_static<Behavior>("ON_UNLOAD_INDEX",                           Behavior::ON_UNLOAD_INDEX);
        s->register_class_static<Behavior>("ON_ENTER_INDEX",                            Behavior::ON_ENTER_INDEX);
        s->register_class_static<Behavior>("ON_EXIT_INDEX",                             Behavior::ON_EXIT_INDEX);
        s->register_class_static<Behavior>("ON_UPDATE_INDEX",                           Behavior::ON_UPDATE_INDEX);
        s->register_class_static<Behavior>("ON_RENDER_INDEX",                           Behavior::ON_RENDER_INDEX);
        s->register_class_static<Behavior>("ON_COLLIDE_INDEX",                          Behavior::ON_COLLIDE_INDEX);
        s->register_class_static<Behavior>("ON_COLLISION_ENTER_INDEX",                  Behavior::ON_COLLISION_ENTER_INDEX);
        s->register_class_static<Behavior>("ON_COLLISION_EXIT_INDEX",                   Behavior::ON_COLLISION_EXIT_INDEX);
        s->register_class_static<Behavior>("ON_EXAMINE_INDEX",                          Behavior::ON_EXAMINE_INDEX);
        s->register_class_static<Behavior>("ON_DIALOG_INDEX",                           Behavior::ON_DIALOG_INDEX);
        s->register_class_static<Behavior>("ON_COMBAT_ENTER_INDEX",                     Behavior::ON_COMBAT_ENTER_INDEX);
        s->register_class_static<Behavior>("ON_COMBAT_EXIT_INDEX",                      Behavior::ON_COMBAT_EXIT_INDEX);
        s->register_class_static<Behavior>("ON_COMBAT_DEATH_INDEX",                     Behavior::ON_COMBAT_DEATH_INDEX);
        s->register_class_static<Behavior>("ON_SHATTER_INDEX",                          Behavior::ON_SHATTER_INDEX);
        s->register_class_static<Behavior>("ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX",   Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX);
        s->register_class_static<Behavior>("ON_COMBAT_TARGET_COLLISION_ENTER_INDEX",    Behavior::ON_COMBAT_TARGET_COLLISION_ENTER_INDEX);
        s->register_class_static<Behavior>("ON_BUTTON_PRESS_INDEX",                     Behavior::ON_BUTTON_PRESS_INDEX);
        s->register_class_static<Behavior>("ON_BUTTON_RELEASE_INDEX",                   Behavior::ON_BUTTON_RELEASE_INDEX);
        s->register_class_static<Behavior>("ON_EVENT_RECEIVE",                          Behavior::ON_RECEIVE_EVENT);
        s->register_class_static<Behavior>("ON_EVENT_OBSERVE",                          Behavior::ON_OBSERVE_EVENT);
        s->register_class_static<Behavior>("ON_TK_SELECT_INDEX",                        Behavior::ON_TK_SELECT_INDEX);
        s->register_class_static<Behavior>("ON_TK_UNSELECT_INDEX",                      Behavior::ON_TK_UNSELECT_INDEX);
        s->register_class_static<Behavior>("TRIGGER_MAX_INDEX",                         Behavior::TRIGGER_MAX_INDEX);

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

        s->register_class_static<Controller>("ANALOG_LEFT", GY_CONTROLLER_ANALOG_LEFT);
        s->register_class_static<Controller>("ANALOG_RIGHT", GY_CONTROLLER_ANALOG_RIGHT);

        s->register_class_static<Controller>("TRIGGER_LEFT", GY_CONTROLLER_TRIGGER_LEFT);
        s->register_class_static<Controller>("TRIGGER_RIGHT", GY_CONTROLLER_TRIGGER_RIGHT);

        typedef elysian::Mouse Mouse;
        s->register_class_static<Mouse>("BUT_NONE", GY_MOUSE_BUT_NONE);
        s->register_class_static<Mouse>("BUT_LEFT", GY_MOUSE_BUT_LEFT);
        s->register_class_static<Mouse>("BUT_MIDDLE", GY_MOUSE_BUT_MIDDLE);
        s->register_class_static<Mouse>("BUT_RIGHT", GY_MOUSE_BUT_RIGHT);
        s->register_class_static<Mouse>("BUT_BACK", GY_MOUSE_BUT_BACK);

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

        typedef elysian::RumbleEffect   RumbleEffect;
        s->register_class_static<RumbleEffect>("INFINITE",              GY_RUMBLE_EFFECT_DURATION_INFINITE);

        typedef elysian::PeriodicRumbleEffect PeriodicRumbleEffect;
        s->register_class_static<PeriodicRumbleEffect>("SINE",          GY_RUMBLE_EFFECT_PERIODIC_SINE);
        s->register_class_static<PeriodicRumbleEffect>("TRIANGLE",      GY_RUMBLE_EFFECT_PERIODIC_TRIANGLE);
        s->register_class_static<PeriodicRumbleEffect>("SAWTOOTHUP",    GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHUP);
        s->register_class_static<PeriodicRumbleEffect>("SAWTOOTHDOWN",  GY_RUMBLE_EFFECT_PERIODIC_SAWTOOTHDOWN);

        typedef elysian::TableLayout TableLayout;
        s->register_class_static<TableLayout>("ALIGN_NEGATIVE",      TableLayout::ALIGN_NEGATIVE);
        s->register_class_static<TableLayout>("ALIGN_CENTER",        TableLayout::ALIGN_CENTER);
        s->register_class_static<TableLayout>("ALIGN_POSITIVE",      TableLayout::ALIGN_POSITIVE);

        typedef elysian::PartySystem PartySystem;
        s->register_class_static<PartySystem>("UnassignedIndex",     PartySystem::UnassignedPlayerIndex);
        s->register_class_static<PartySystem>("InvalidIndex",        PartySystem::InvalidPlayerIndex);

        typedef elysian::EventTarget EventTarget;
        s->register_class_static<EventTarget>("INVALID",            EVENT_TARGET_INVALID);
        s->register_class_static<EventTarget>("ENTITY",             EVENT_TARGET_ENTITY);
        s->register_class_static<EventTarget>("BEHAVIOR",           EVENT_TARGET_BEHAVIOR);
        s->register_class_static<EventTarget>("WIDGET",             EVENT_TARGET_WIDGET);
        s->register_class_static<EventTarget>("LUA",                EVENT_TARGET_LUA);

        typedef elysian::Event Event;
        s->register_class_static<EventTarget>("STATE_PENDING",      EVENT_STATE_PENDING);
        s->register_class_static<EventTarget>("STATE_ACCEPTED",     EVENT_STATE_ACCEPTED);
        s->register_class_static<EventTarget>("STATE_IGNORED",      EVENT_STATE_IGNORED);

        s->register_class_static<Event>("ENTITY_ACTIVATE",          EVENT_ENTITY_ACTIVATE);
        s->register_class_static<Event>("ENTITY_DEACTIVATE",        EVENT_ENTITY_DEACTIVATE);
        s->register_class_static<Event>("KEYBOARD_ATTACHED",        EVENT_KEYBOARD_ATTACHED);
        s->register_class_static<Event>("KEYBOARD_REMOVED",         EVENT_KEYBOARD_REMOVED);
        s->register_class_static<Event>("KEYBOARD_KEY_PRESSED",     EVENT_KEYBOARD_KEY_PRESSED);
        s->register_class_static<Event>("KEYBOARD_KEY_RELEASED",    EVENT_KEYBOARD_KEY_RELEASED);
        s->register_class_static<Event>("MOUSE_ATTACHED",           EVENT_MOUSE_ATTACHED);
        s->register_class_static<Event>("MOUSE_REMOVED",            EVENT_MOUSE_REMOVED);
        s->register_class_static<Event>("MOUSE_MOVED",              EVENT_MOUSE_MOVED);
        s->register_class_static<Event>("MOUSE_BUTTON_PRESSED",     EVENT_MOUSE_BUTTON_PRESSED);
        s->register_class_static<Event>("MOUSE_BUTTON_RELEASED",     EVENT_MOUSE_BUTTON_RELEASED);
        s->register_class_static<Event>("MOUSE_SCROLLED",           EVENT_MOUSE_SCROLLED);
        s->register_class_static<Event>("CONTROLLER_ATTACHED",      EVENT_CONTROLLER_ATTACHED);
        s->register_class_static<Event>("CONTROLLER_REMOVED",       EVENT_CONTROLLER_REMOVED);
        s->register_class_static<Event>("CONTROLLER_BUTTON_PRESSED",EVENT_CONTROLLER_BUTTON_PRESSED);
        s->register_class_static<Event>("CONTROLLER_BUTTON_RELEASED",EVENT_CONTROLLER_BUTTON_RELEASED);
        s->register_class_static<Event>("CONTROLLER_ANALOG_MOVED",  EVENT_CONTROLLER_ANALOG_MOVED);
        s->register_class_static<Event>("CONTROLLER_TRIGGER_MOVED", EVENT_CONTROLLER_TRIGGER_MOVED);
        s->register_class_static<Event>("RUMBLE_DEVICE_ATTACHED",   EVENT_RUMBLE_DEVICE_ATTACHED);
        s->register_class_static<Event>("RUMBLE_DEVICE_REMOVED",    EVENT_RUMBLE_DEVICE_REMOVED);
        s->register_class_static<Event>("VMU_ATTACHED",             EVENT_VMU_ATTACHED);
        s->register_class_static<Event>("VMU_REMOVED",              EVENT_VMU_REMOVED);
        s->register_class_static<Event>("COLLISION_ENTER",          EVENT_COLLISION_ENTER);
        s->register_class_static<Event>("COLLISION_EXIT",           EVENT_COLLISION_EXIT);
        s->register_class_static<Event>("AREA_ENTER",               EVENT_AREA_ENTER);
        s->register_class_static<Event>("AREA_EXIT",                EVENT_AREA_EXIT);
        s->register_class_static<Event>("AREA_CHANGED",             EVENT_AREA_CHANGED);
        s->register_class_static<Event>("GAME_TIME_DAWN",           EVENT_GAME_TIME_DAWN);
        s->register_class_static<Event>("GAME_TIME_NOON",           EVENT_GAME_TIME_NOON);
        s->register_class_static<Event>("GAME_TIME_DUSK",           EVENT_GAME_TIME_DUSK);
        s->register_class_static<Event>("GAME_TIME_MIDNIGHT",       EVENT_GAME_TIME_MIDNIGHT);
        s->register_class_static<Event>("LUA",                      EVENT_LUA);

    }


    void LuaManagerBase::_registerWrappers(void) {
        //Register classes and structs that have been proxied to Lua
        _mainState->register_class<Recti>();
        Recti::registerProxyMetaTable(*_mainState);
        _mainState->register_class<BitMask32>();
        BitMask32::registerProxyMetaTable(*_mainState);
        _mainState->register_class<BitMask64>();
        BitMask64::registerProxyMetaTable(*_mainState);
		_mainState->register_class<LuaBehaviorScriptAttrib>();
        OOLUA::register_class<GYVector2>(*_mainState);
        OOLUA::register_class<GYVector3>(*_mainState);
        OOLUA::register_class<GYVector4>(*_mainState);
        OOLUA::register_class<Vector2>(*_mainState);
        Vector2::registerProxyMetaTable(*_mainState);
        OOLUA::register_class<Vector3>(*_mainState);
        Vector3::registerProxyMetaTable(*_mainState);
        OOLUA::register_class<Vector4>(*_mainState);
        Vector4::registerProxyMetaTable(*_mainState);
        _mainState->register_class<GYColor4>();
        _mainState->register_class<Color4>();
        Color4::registerProxyMetaTable(*_mainState);
		OOLUA::register_class<LuaEventCallback>(*_mainState);
		_mainState->register_class<InstanceTransform>();
        InstanceTransform::registerProxyMetaTable(*_mainState);
		OOLUA::register_class<Entity>(*_mainState);
        Entity::registerProxyMetaTable(*_mainState);
        _mainState->register_class<BlendMode>();
        BlendMode::registerProxyMetaTable(*_mainState);
        _mainState->register_class<SpriteAttrib>();
        SpriteAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<QuadSpriteAttrib>();
		_mainState->register_class<Component>();
		_mainState->register_class<Material>();
        Material::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Renderable>();
        Renderable::registerProxyMetaTable(*_mainState);
        _mainState->register_class<LightAttrib>();
        LightAttrib::registerProxyMetaTable(*_mainState);
		_mainState->register_class<CollisionFlags>();
        CollisionFlags::registerProxyMetaTable(*_mainState);
        OOLUA::register_class<CollidableStaticAttrib>(*_mainState);
        CollidableStaticAttrib::registerProxyMetaTable(*_mainState);
		_mainState->register_class<CollidableAttrib>();
        CollidableAttrib::registerProxyMetaTable(*_mainState);
		_mainState->register_class<CollisionContact>();
		_mainState->register_class<Collidable>();
        Collidable::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Character>();
        Character::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Warp>();
        Warp::registerProxyMetaTable(*_mainState);
		_mainState->register_class<DialogScreen>();
        DialogScreen::registerProxyMetaTable(*_mainState);
		_mainState->register_class<DialogText>();
        DialogText::registerProxyMetaTable(*_mainState);
        _mainState->register_class<DialogOption>();
        DialogOption::registerProxyMetaTable(*_mainState);
		_mainState->register_class<DialogPrompt>();
        DialogPrompt::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Dialog>();
        Dialog::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Examinable>();
        Examinable::registerProxyMetaTable(*_mainState);
        OOLUA::register_class<CombatStatusModifier>(*_mainState);
        CombatStatus::registerProxyMetaTable(*_mainState);
		_mainState->register_class<CombatAttrib>();
        CombatAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<AggroManager>();
		_mainState->register_class<Combat>();
        Combat::registerProxyMetaTable(*_mainState);
        _mainState->register_class<RigidBodyAttrib>();
        RigidBodyAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<ForceTarget>();
        ForceTarget::registerProxyMetaTable(*_mainState);
        _mainState->register_class<RigidBody>();
        RigidBody::registerProxyMetaTable(*_mainState);
		_mainState->register_class<ParticleEmitterAttrib>();
        ParticleEmitterAttrib::registerProxyMetaTable(*_mainState);
		_mainState->register_class<ParticleEmitter>();

        _mainState->register_class<Event>();
       // Event::registerProxyMetaTable(*_mainState);
        _mainState->register_class<LuaEvent>();
       // LuaEvent::registerProxyMetaTable(*_mainState);
        _mainState->register_class<KeyEvent>();
        _mainState->register_class<ControllerEvent>();
        _mainState->register_class<ControllerButtonEvent>();
        _mainState->register_class<ControllerAnalogEvent>();
        _mainState->register_class<ControllerTriggerEvent>();
        _mainState->register_class<MouseBaseEvent>();
        _mainState->register_class<MouseEvent>();
        _mainState->register_class<MouseWheelEvent>();

        _mainState->register_class<EventTarget>();
        _mainState->register_class<EventObserver>();
        _mainState->register_class<EventSystem>();
        ParticleEmitter::registerProxyMetaTable(*_mainState);
        OOLUA::register_class<LuaBehaviorScript>(*_mainState);
		OOLUA::register_class<LuaBehavior>(*_mainState);
        //Behavior::registerProxyMetaTable(*_mainState);
        LuaBehavior::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Tile>();
		_mainState->register_class<Level>();
		OOLUA::register_class<TileEntry>(*_mainState);
		_mainState->register_class<Area>();
        Area::registerProxyMetaTable(*_mainState);
    //	_mainState->register_class<GameInput>();
       // _mainState->register_class<MouseInput>();
		_mainState->register_class<PartySystem>();
		_mainState->register_class<TimerSystem>();
        _mainState->register_class<Engine>();
		_mainState->register_class<gyro>();
		_mainState->register_class<GYWav>();
		_mainState->register_class<AudioSource>();
        AudioSource::registerProxyMetaTable(*_mainState);
		_mainState->register_class<LightSource>();
        LightSource::registerProxyMetaTable(*_mainState);
		_mainState->register_class<CharacterSystem>();
		_mainState->register_class<AudioSystem>();
		_mainState->register_class<LightSystem>();
		_mainState->register_class<ParticleSystem>();
		_mainState->register_class<DaySystem>();
        _mainState->register_class<ExaminableSystem>();
		_mainState->register_class<DialogSystem>();
        _mainState->register_class<CollisionSystem>();
		_mainState->register_class<RigidBodySystem>();
        _mainState->register_class<ComputeSystem>();
        _mainState->register_class<ThreadSystem>();
        _mainState->register_class<RenderSystem>();
        _mainState->register_class<AdventureLog>();
        OOLUA::register_class<HorizontalLayout>(*_mainState);
        _mainState->register_class<VerticalLayout>();
        OOLUA::register_class<EntityWidget>(*_mainState);
        _mainState->register_class<LabelWidget>();
		_mainState->register_class<GuiSystem>();
		_mainState->register_class<LevelSystem>();
        _mainState->register_class<Controller>();
        _mainState->register_class<Mouse>();
        _mainState->register_class<TouchDevice>();
        _mainState->register_class<RumbleEffect>();
        _mainState->register_class<DirectionalRumbleEffect>();
        _mainState->register_class<LeftRightRumbleEffect>();
        _mainState->register_class<ConstantRumbleEffect>();
        _mainState->register_class<RampRumbleEffect>();
        _mainState->register_class<PeriodicRumbleEffect>();
        _mainState->register_class<RumbleDevice>();
        _mainState->register_class<VisualMemoryUnit>();
        _mainState->register_class<Keyboard>();
#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
        _mainState->register_class<Accelerometer>();
        _mainState->register_class<Gyroscope>();
        _mainState->register_class<Magnetometer>();
        _mainState->register_class<Altimeter>();
        _mainState->register_class<Pedometer>();
        _mainState->register_class<Scale>();
#endif
        _mainState->register_class<InputSystem>();
		_mainState->register_class<CombatSystem>();
		_mainState->register_class<CameraSystem>();
		_mainState->register_class<Direction>();
		OOLUA::register_class<GravityForceGenerator>(*_mainState);
		_mainState->register_class<ImplosionForceGenerator>();
		_mainState->register_class<ExplosionForceGenerator>();
		_mainState->register_class<CameraKeyFrame>();
        _mainState->register_class<AnimationTrack>();
        AnimationTrack::registerProxyMetaTable(*_mainState);
        _mainState->register_class<Animation>();
        Animation::registerProxyMetaTable(*_mainState);
        _mainState->register_class<SkeletonAttrib>();
        SkeletonAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<Skeleton>();
        Skeleton::registerProxyMetaTable(*_mainState);


		//Register C and static member functions
		lua_register(*_mainState, "print", &print);
        lua_register(*_mainState, "warnprint", &warnprint);
        lua_register(*_mainState, "errprint", &errprint);
        lua_register(*_mainState, "printAdvLog", &printAdvLog);
        lua_register(*_mainState, "dbgPush", &dbgPush);
        lua_register(*_mainState, "dbgPop", &dbgPop);
        _mainState->register_class_static<Engine>("getCurArea",         &Singletons_getCurArea);
        _mainState->register_class_static<Engine>("getCurLevel",        &Singletons_getCurLevel);
        _mainState->register_class_static<Engine>("setProcessInput",    &Singletons_setProcessInput);
        _mainState->register_class_static<Engine>("getPartySys",        &Singletons_getPartySystem);
        _mainState->register_class_static<Engine>("getInputSys",        &Singletons_getInputSystem);
        _mainState->register_class_static<Engine>("getTimerSys",        &Singletons_getTimerSystem);
        _mainState->register_class_static<Engine>("getAudioSys",        &Singletons_getAudioSystem);
        _mainState->register_class_static<Engine>("getLightSys",        &Singletons_getLightSystem);
        _mainState->register_class_static<Engine>("getLevelSys",        &Singletons_getLevelSystem);
        _mainState->register_class_static<Engine>("getCombatSys",       &Singletons_getCombatSystem);
        _mainState->register_class_static<Engine>("getPartSys",         &Singletons_getParticleSystem);
        _mainState->register_class_static<Engine>("getDaySys",          &Singletons_getDaySystem);
        _mainState->register_class_static<Engine>("getCollisionSys",    &Singletons_getCollisionSystem);
        _mainState->register_class_static<Engine>("getRigidBodySys",    &Singletons_getRigidBodySystem);
        _mainState->register_class_static<Engine>("getDialogSys",       &Singletons_getDialogSystem);
        _mainState->register_class_static<Engine>("getGuiSys",          &Singletons_getGuiSystem);
        _mainState->register_class_static<Engine>("getCameraSys",       &Singletons_getCameraSystem);
        _mainState->register_class_static<Engine>("getCharSys",         &Singletons_getCharacterSystem);
        _mainState->register_class_static<Engine>("getEventSys",        &Singletons_getEventSystem);
        _mainState->register_class_static<Engine>("getExamSys",         &Singletons_getExaminableSystem);
        _mainState->register_class_static<Engine>("getComputeSys",      &Singletons_getComputeSystem);
        _mainState->register_class_static<Engine>("getRenderSys",       &Singletons_getRenderSystem);
        _mainState->register_class_static<Engine>("getThreadSys",       &Singletons_getThreadSystem);

		_mainState->register_class_static<EventMonitor>("null", &EventMonitor_null);

        _mainState->register_class_static<Direction>("fromVector2", &Direction_fromVector2);
        _mainState->register_class_static<Direction>("toVector2", &Direction_toVector2);
        _mainState->register_class_static<Direction>("toVector3", &Direction_toVector3);

		//libGyro wrappers
//		_mainState->register_class_static<gyro>("filePath", &gyroFilePath);
		_mainState->register_class_static<gyro>("createWav", &gyroWavCreate);
		_mainState->register_class_static<gyro>("destroyWav", &gyroWavDestroy);

		//Register constants and enumerations
		luaRegisterEnumsConstants(_mainState);
	}

}

/*!
    \qmltype Engine
    \since 1.6.0
    \inmodule Engine
    \brief Static class for Engine providing access to all subsystems.

    Engine is a static class (class of all static functions) providing access
    to the various subsystems and the current \l{Level} and \l{Area}.

    \section2 Asset/Data Access and Centrality
    90% of what is operated upon by the engine and its various subsystems is
    stored within the current \l{Level} and \a{Area} objects, rather than within
    the subsystems themselves. This drastically simplifies the process of loading
    and swapping between multiple levels and areas, and allows for the ones that
    are not active to be cached to a certain extent for platforms with enough RAM
    to do so. The main advantage is that Entities, components, and other data does
    not have to be copied in and out of each subsystem when an Area is activated or
    when new data arrives. Since it's all accessed indirectly through the current
    area, swapping areas (and levels) ultimately becomes simply reassigning a
    pointer internally, and the higher-level game logic and Lua scripts never
    have to worry about maintaining consistency between assets in a Level/Area
    and the subsystems or even data stored within one subsystem vs. another.

    From a Behavior and LuaBehavior scripting perspective, most of the data you
    need to access and manipulate from within your scripts are also stored within
    either the current Level or Area. You can retreive these through the Engine.
    \code
    local area = Engine.getCurArea()
    local level = Engine.getCurLevel()
    \endcode

    \section2 Subsystems
    The engine itself delegates all processing, input, and rendering logic to a
    series of subsystems, most of which manage a particular type of \l{Component}.
    \table
        \header
            \li \b{Subsystem}
            \li \b{Managed Data}
            \li \b{Functionality}
        \row
            \li \l{AudioSystem}
            \li \l{AudioSource}
            \li Music and SFX playback
        \row
            \li \l{CameraSystem}
            \li \l{CameraKeyFrame}
            \li Camera properties, cutscenes, boundaries, viewport
        \row
            \li \l{CharacterSystem}
            \li \l{Character}
            \li Basic character functionality: walking, running, jumping, holding, etc
        \row
            \li \l{CollisionSystem}
            \li \l{Collidable}
            \li Collision checks, contact generation, penetration resolution
        \row
            \li \l{CombatSystem}
            \li \l{Combat}
            \li Battles, stats, aggro, damage resolution, death
        \row
            \li \l{DaySystem}
            \li Sun \l{Entity}
            \li Day/Night dynamic lighting, in-game time
        \row
            \li \l{DialogSystem}
            \li \l{Dialog}
            \li Dialog box text from conversations, examined items, messages
        \row
            \li \l{EventSystem}
            \li \l{Event}
            \li Sends, receives, and propogates events to other objects
        \row
            \li \l{ExaminableSystem}
            \li \l{Examinable}
            \li Object/Entity inspection/interaction
        \row
            \li \l{GuiSystem}
            \li \l{Widget}
            \li Menus, UIs, overlays
        \row
            \li \l{InputSystem}
            \li \l{PlayerIODevice}
            \li \l{Controller}, \l{Keyboard}, \l{Mouse}, \l{VisualMemoryUnit}
        \row
            \li \l{LevelSystem}
            \li \l{Level}, \l{Area}
            \li Maintains current level/area, and handles swapping between new ones
        \row
            \li \l{LightSystem}
            \li \l{LightSource}
            \li Handles all of dynamic lighting
        \row
            \li \l{ParticleSystem}
            \li \l{ParticleEmitter}
            \li Emitting, updating, and maintaining particles
        \row
            \li \l{PartySystem}
            \li \l{Playable}
            \li Basic player input like walking, running, jumping, throwing, etc
        \row
            \li \l{RigidBodySystem}
            \li \l{RigidBody}, \l{ForceGenerator}
            \li All of physics integration, force application, contact velocity resolution
        \row
            \li \l{TimerSystem}
            \li \l{TimerSystem::getDeltaTime()}{deltaTime}
            \li Calculates frame time, maintains framerate, manages timers
        \row
            \li \l{ThreadSystem}
            \li Threads, ThreadTasks
            \li Manages CPU threads and thread-level parallelization
        \row
            \li \l{ComputeSystem}
            \li GPGPU Kernels/Compute Shaders
            \li Manages GPGPU and hardware accelerated parallel tasks
        \row
            \li \l{RenderSystem}
            \li \l{Renderable}, \l{Skeleton}, \l{LightSource}
            \li Handles all rendering and displaying graphics
    \endtable

    \sa PartySystem, InputSystem, TimerSystem, AudioSystem, LightSystem, LevelSystem,
    CombatSystem, ParticleSystem, DaySystem, CollisionSystem, RigidBodySystem, GuiSystem,
    CharacterSystem, CameraSystem, DialogSystem, ExaminableSystem, ThreadSystem,
    ComputeSystem, RenderSystem
*/

/*!
 * \qmlmethod Area* Engine::getCurArea()
 *
 * Returns the current active Area.
 */
/*!
 * \qmlmethod Level* Engine::getCurLevel()
 *
 * Returns the current active Level.
 */
/*!
 * \qmlmethod void Engine::setProcessInput(bool val)
 *
 * Tells the engine to process input normally if \a val is true, otherwise the engine will no longer process input events,
 * giving full control to Lua.
 */
/*!
 * \qmlmethod PartySystem* Engine::getPartySys()
 *
 * Returns the PartySystem subsystem.
 */
/*!
 * \qmlmethod InputSystem* Engine::getInputSys()
 *
 * Returns the InputSystem subsystem.
 */
/*!
 * \qmlmethod TimerSystem* Engine::getTimerSys()
 *
 * Returns the TimerSystem subsystem.
 */
/*!
 * \qmlmethod AudioSystem* Engine::getAudioSys()
 *
 * Returns the AudioSystem subsystem.
 */
/*!
 * \qmlmethod LightSystem* Engine::getLightSys()
 *
 * Returns the LightSystem subsystem.
 */
/*!
 * \qmlmethod LevelSystem* Engine::getLevelSys()
 *
 * Returns the LevelSystem subsystem.
 */
/*!
 * \qmlmethod CombatSystem* Engine::getCombatSys()
 *
 * Returns the CombatSystem subsystem.
 */
/*!
 * \qmlmethod ParticleSystem* Engine::getPartSys()
 *
 * Returns the ParticleSystem subsystem.
 */
/*!
 * \qmlmethod DaySystem* Engine::getDaySys()
 *
 * Returns the DaySystem subsystem.
 */
/*!
 * \qmlmethod CollisionSystem* Engine::getCollisionSys()
 *
 * Returns the CollisionSystem subsystem.
 */
/*!
 * \qmlmethod RigidBodySystem* Engine::getRigidBodySys()
 *
 * Returns the RigidBodySystem subsystem.
 */
/*!
 * \qmlmethod GuiSystem* Engine::getGuiSys()
 *
 * Returns the GuiSystem subsystem.
 */
/*!
 * \qmlmethod ExaminableSystem* Engine::getExamSys()
 *
 * Returns the ExaminableSystem subsystem.
 */
/*!
 * \qmlmethod CameraSystem* Engine::getCameraSys()
 *
 * Returns the CameraSystem subsystem.
 */
/*!
 * \qmlmethod CharacterSystem* Engine::getCharSys()
 *
 * Returns the CharacterSystem subsystem.
 */
/*!
 * \qmlmethod DialogSystem* Engine::getDialogSys()
 *
 * Returns the DialogSystem subsystem.
 */
/*!
 * \qmlmethod EventSystem* Engine::getEventSys()
 *
 * Returns the EventSystem subsystem.
 */
/*!
 * \qmlmethod RenderSystem* Engine::getRenderSys()
 *
 * Returns the RenderSystem subsystem.
 */
/*!
 * \qmlmethod ComputeSystem* Engine::getComputeSys()
 *
 * Returns the ComputeSystem subsystem.
 */
/*!
 * \qmlmethod ThreadSystem* Engine::getThreadSys()
 *
 * Returns the ThreadSystem subsystem.
 */

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Engine)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Engine)


/*!
    \qmltype gyro
    \since 1.6.0
    \inmodule Engine
    \brief Static class for libGyro constants and enumerations.
*/
/*!
 * \qmlmethod GYWav* gyro::createWav(string path)
 * This function is static.
 * Loads a wave file from the given asset-relative \a{path} and returns a new \l{GYWav}
 * object containing its audio buffer or returns a reference to an existing \l{GYWav}
 * object if it has already been loaded.
 *
 * \e{NOTE: Any WAV created in this manner must also be deleted with destroyWav()!}
 * \sa destroyWav(), GYWav
 */
/*!
 * \qmlmethod void gyro::destroyWav(GYWav* wav)
 * This function is static.
 * Destroys the given \a{wav} object and frees its audio buffer if \a{wav} was the last
 * object still referencing the buffer or simply decrements the internal reference count
 * for the buffer if its still in use by other objects.
 * \sa createWav(), GYWav
 */

/*!
\qmlproperty enumeration gyro::TEX_ORIENT_TYPE

Represents the different possible orientations of a 2D texture.

The orientations can be of:
\list
\li gyro.TEX_ORIENT_NORMAL              - No special orientation
\li gyro.TEX_ORIENT_FLIP_HORIZ          - Textured flipped over the Y axis
\li gyro.TEX_ORIENT_FLIP_VERT           - Textured flipped over the X axis
\li gyro.TEX_ORIENT_ROT_90              - Texture rotated 90 degrees clockwise
\li gyro.TEX_ORIENT_ROT_180             - Texture rotated 180 degrees clockwise
\li gyro.TEX_ORIENT_ROT_270             - Textured rotated 270 degrees clockwise
\li gyro.TEX_ORIENT_FLIP_HORIZ_ROT_90   - Texture flipped over the Y axis then rotated 90 degrees clockwise
\li gyro.TEX_ORIENT_FLIP_HORIZ_ROT_270  - Texture flipped over the Y axis then rotated 270 degrees clockwise
\endlist
*/

/*!
\qmlproperty enumeration gyro::RECT_PLANE_TYPE

Represents the different planes a 2D texture can be applied to. Imagine these as the faces of a cube.

The different possible plane types are:
\list
\li gyro.RECT_PLANE_BOTTOM              - Regular, flat texture
\li gyro.RECT_PLANE_FRONT               - Texture standing up, facing front
\li gyro.RECT_PLANE_LEFT                - Texture standing up, facing left
\li gyro.RECT_PLANE_RIGHT               - Texture standing up, facing right
\li gyro.RECT_PLANE_FRONT_SLOPE_45      - Texture as a 45 degree ramp, facing front
\li gyro.RECT_PLANE_LEFT_SLOPE_45       - Texture as a 45 degree ramp, facing left
\li gyro.RECT_PLANE_RIGHT_SLOPE_45      - Texture as a 45 degree ramp, facing right
\endlist
*/

/*!
\qmlproperty enumeration gyro::COLLIDER_TYPE

Represents the different types of 2D colliders supported by the physics engine.

The different collider primitives are:
\list
\li gyro.COLLIDER_NONE  - No collider
\li gyro.COLLIDER_AABB  - Axis-aligned bounding box (only for rectangles that never rotate)
\li gyro.COLLIDER_OBB   - Oriented bounding box (for rectangles that can rotate)
\li gyro.COLLIDER_RTRI  - Right-triangle
\li gyro.COLLIDER_CIR   - Circle
\endlist
*/


EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(gyro)
EXPORT_OOLUA_FUNCTIONS_0_CONST(gyro)



/*!
    \qmltype _G
    \since 1.6.1
    \inmodule Engine
    \brief Global Lua Environment Table

    _G is the global environment table for all scripts, behaviors, modules and
    executable Lua code within the Engine (including ESTk's Lua terminal/console).
    The pairs within this table represent global variable names and their values,
    all Lua contexts have at least read-only access to this table, with or without
    explicitly referencing the variable through _G (you can just leave it off).
    Scripts do not implicitly inherit write-access to this table, so writing to a
    global variable must be done explicitly through the _G.* accessor.

    _G houses built-in Lua functions, and some custom overrides supplied by the engine,
    as well as some utility functions, and the actual userdata metatables for \e every
    C++ object that has been proxied/wrapped to Lua.


    \sa Engine, Toolkit
*/

/*!
 * \qmlmethod void _G::print(string msg, ...)
 *  Custom variadic print routine form the Engine, overriding the built-in print function.
 *  It works the exact same way (any number of any argument), but it does a much
 *  better job of converting data types to strings for displaying them, and is
 *  also Lua proxy/userdata type-aware. The default severity level for anything
 *  printed with this function is "verbose," meaning it's simply for debugging and logging
 *  purposes, and these messages do not represent errors or warnings. In the Toolkit's terminal
 *  window, output from this function is displayed as a light blue/teal color
 *
 *  \e {NOTE: Due to the nature of Lua string concatenation, it's advisable to
 *      pass multiple values as a comma separated (',') list (there will automatically
 *      be spaces appended between them) rather than concatenating everything into a single
 *      string by conjoining with the '..' operator. This will make a lot of tiny temporary
 *      allocations on the heap for each substring and is slower and wasteful. Passing a list
 *      of entries allows the Engine to do the heavy lifting. }
 *
 * \e {NOTE: For warning and error severities, see the other variations of this function.}
 *
 * \sa warnprint, errprint, alertprint, rPrint, dbgpush, dbgpop
 */
/*!
 * \qmlmethod void _G::rPrint(LuaTable table, int maxDepth, string stringName)
 *  Custom "Recursive Print" routine, especially useful for printing Lua table or userdata values,
 *  allowing you to inspect deeply embedded tables and objects. The given \a{table} is traversed,
 *  recursively, including all of its nested subtables, until the \a{maxDepth} limit has been reached.
 *  \a{stringName} provides a display name for the top-level table.
 *
 * \e {NOTE: The default \a{maxDepth} value is 100, and the default \a{stringName} is just
 *  an empty string, so these two arguments are optional}
 *
 * \sa print, serialize
 */
/*!
 * \qmlmethod void _G::warnprint(string msg, ...)
 * Custom logging method, similar to \l{print}, only with the Toolkit logging severity set to warning,
 * meaning the message denotes a non-fatal issue or something that may potentially be something the
 * user may want to look into.
 *
 * Warnings are displayed in dark yellow within the ESTk terminal/log.
 * \sa print, errprint, alertprint
 */
/*!
 * \qmlmethod void _G::errprint(string msg, ...)
 * Custom logging method, similar to \l{print} and \l{warnprint}, only with the Toolkit logging severity set to error,
 * meaning the message contains some sort of important, potentially fatal error message, and the user should fix it.
 *
 * Errors are displayed in dark red within the ESTk terminal/log.
 * \sa print, warnprint, alertprint
 */
/*!
 * \qmlmethod void _G::alertprint(string msg, ...)
 * Custom logging method similar to \l{print}, \l{warnprint}, and \l{errprint}, except the output is displayed as a modal
 * alert box widget with an 'Ok' button.
 *
 * \e { NOTE: This method requires ESTk and depends on the \l{Toolkit} class, meaning you should only be using the  method
 *      in Toolkit-specific triggers or control flow. }
 * \sa print, warnprint, errprint
 */
/*!
 * \qmlmethod void _G::printAdvLog(string msg, ...)
 * Custom logging method similar to \l{print}, only the output is added to the \l{AdventureLog} instead of being logged.
 *
 * \e {NOTE: \a{msg} may contain special \l {Font} mark-up for highlighting keywords and/or more customizing the text's appearance}
 *
 * \sa print, warnprint, errprint, alertprint
 */
/*!
 * \qmlmethod void _G::dbgpush()
 * Utility function used to increase the depth of the debug log stack, meaning all following print/warnprint/errprint
 * log messages are considered children of the previous message. This is recommended at the top of a function with
 * complex, multi-layer/aggregate debug output, so that it can be grouped together, formatted and tabbed prettily,
 * and can be collapsed/expanded within the debug output window in ESTk.
 *
 * \sa dbgpop, print
 */
/*!
 * \qmlmethod void _G::dbgpop(int count=1)
 * Utility function used to decrease the depth of the debug log stack (by \a{count}) meaning the following log messages are no longer
 * children of the previous parent, and are instead children to the parent that was active before the last \a{count} pushes. A corresponding
 * dbgpop() is recommended to go at the end of a complex/aggregate function with multiple layers of logging where dbgpush() was
 * previously used.
 *
 * \e {NOTE: \a{count} defaults to 1, which means it will do the exact opposite of the dbgpush function when it's not passed an arguments.
 *      Under certain circumstances you may have nested multilayer logic requiring multiple calls to dbgpush() before calling a dbgpop(),
 *      The two dbgpop() calls can be combined into a single call, with the \a{count} argument set to 2... This can make the code cleaner
 *      (and faster) by allowing you to combine multiple back-to-back dbgpop calls. }
 *
 * \e {NOTE: It is VERY important that for every call to dbgpush at the beginning of a block of logic, there is a corresponding dbgpop(),
 *      otherwise the stack can become incorrect, as all later debug messages will no longer have the correct parent, screwing up the entire
 *      grouping. Also note that poping more to the stack than you push is invalid, and may cause the program to crash. }
 * \sa dbgpush, print
 */
/*!
 * \qmlmethod string _G::serialize(LuaTable ref)
 * Utility function provided by the Engine for converting any arbirary Lua table, userdata, or value (given by \a{ref}) into a single executable string of Lua code
 * in the form of a simple table/object declaration, which may then be written to a file and directly executed later to fully recover the state of the data.
 * The format of this serialized string data is being tenderly named "LSON" as it is the exact Lua equivalent of the "JSON" format... (copyright Falco Girgis)
 *
 * The function is very complex and is the basis for all LSON asset files within ESTk. There are some general considerations to keep in mind when using it:
 * \list
 * \li \b {Userdata} - The values saved for C/++ userdata types are the PROPERTIES that have been wrapped to Lua. A userdata type without properties cannot be
 *      serialized properly, and will just have a placeholder "userdata" string.
 * \li \b {Recursive tables} - If table A contains a reference to table B which then contains a reference to table A, this is considered a recursively
 *      referenced Lua table and cannot be totally represented as serial data. The function will store an error message string rather than a reference back
 *      to the parent once recursion has been detected
 * \li \b {Prefabs} - When a table has been instantiated from a prefab value, the serialize function should attempt to honor this association, by instantiating
 *      the object with the prefab as the parent data, rather than creating an entirely new object
 * \li \b {Data Sharing} - ESTk tries to be as optimal as possible with not bloating save files. It does this by NOT duplicating data that is already present with a
 *      parent prefab table. It also attempts to skip writing out read-only values as well as values which have not been changed from their defaults.
 * \endlist
 *
 * \sa rPrint
 */
