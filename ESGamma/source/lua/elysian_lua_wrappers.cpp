#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_animation_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>
#include <proxy/elysian_level_proxy.hpp>
#include <proxy/elysian_area_proxy.hpp>

#include <proxy/elysian_game_input_proxy.hpp>
#include <proxy/elysian_gui_system_proxy.hpp>
#include <proxy/elysian_level_system_proxy.hpp>
#include <proxy/elysian_animation_system_proxy.hpp>
#include <proxy/elysian_character_system_proxy.hpp>
#include <proxy/elysian_audio_system_proxy.hpp>
#include <proxy/elysian_camera_system_proxy.hpp>
#include <proxy/elysian_combat_system_proxy.hpp>
#include <proxy/elysian_day_system_proxy.hpp>
#include <proxy/elysian_dialog_system_proxy.hpp>
#include <proxy/elysian_light_system_proxy.hpp>
#include <proxy/elysian_particle_system_proxy.hpp>
#include <proxy/elysian_party_system_proxy.hpp>
#include <proxy/elysian_rigid_body_system_proxy.hpp>
#include <proxy/elysian_timer_system_proxy.hpp>
#include <proxy/elysian_bit_mask_proxy.hpp>

#include <crtp/elysian_lua_manager.hpp>

#include <lua/elysian_lua_wrappers.hpp>

namespace elysian {

    void LuaManagerBase::_registerWrappers(void) {

		//Register classes and structs
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
        _mainState->register_class<SpriteAttrib>();
        SpriteAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<QuadSpriteAttrib>();
		_mainState->register_class<Component>();
		_mainState->register_class<Material>();
        Material::registerProxyMetaTable(*_mainState);
		_mainState->register_class<Renderable>();
        Renderable::registerProxyMetaTable(*_mainState);
		_mainState->register_class<AnimationSequence>();
		_mainState->register_class<AnimationAttrib>();
		_mainState->register_class<ChildAnimationSequence>();
		_mainState->register_class<ChildAnimationAttrib>();
		_mainState->register_class<ChildAnimation>();
        _mainState->register_class<LightAttrib>();
        LightAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<Animation>();
        Animation::registerProxyMetaTable(*_mainState);
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
		_mainState->register_class<Combat>();
        Combat::registerProxyMetaTable(*_mainState);
		OOLUA::register_class<MenuText>(*_mainState);
        _mainState->register_class<RigidBodyAttrib>();
        RigidBodyAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<ForceTarget>();
        ForceTarget::registerProxyMetaTable(*_mainState);
        _mainState->register_class<RigidBody>();
        RigidBody::registerProxyMetaTable(*_mainState);
		_mainState->register_class<MenuEntity>();
		_mainState->register_class<ParticleEmitterAttrib>();
        ParticleEmitterAttrib::registerProxyMetaTable(*_mainState);
		_mainState->register_class<ParticleEmitter>();
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
		_mainState->register_class<Singletons>();
		_mainState->register_class<gyro>();
		_mainState->register_class<GYWav>();
		_mainState->register_class<AudioSource>();
        AudioSource::registerProxyMetaTable(*_mainState);
		_mainState->register_class<LightSource>();
        LightSource::registerProxyMetaTable(*_mainState);
		_mainState->register_class<AnimationSystem>();
		_mainState->register_class<CharacterSystem>();
		_mainState->register_class<AudioSystem>();
		_mainState->register_class<LightSystem>();
		_mainState->register_class<ParticleSystem>();
		_mainState->register_class<DaySystem>();
		_mainState->register_class<DialogSystem>();
		_mainState->register_class<RigidBodySystem>();
        _mainState->register_class<AdventureLog>();
		_mainState->register_class<GuiSystem>();
		_mainState->register_class<LevelSystem>();
        _mainState->register_class<Controller>();
        _mainState->register_class<InputSystem>();
		_mainState->register_class<CombatSystem>();
		_mainState->register_class<CameraSystem>();
		_mainState->register_class<Direction>();
		OOLUA::register_class<GravityForceGenerator>(*_mainState);
		_mainState->register_class<ImplosionForceGenerator>();
		_mainState->register_class<ExplosionForceGenerator>();
		_mainState->register_class<CameraKeyFrame>();
        _mainState->register_class<SkeletonAttrib>();
        SkeletonAttrib::registerProxyMetaTable(*_mainState);
        _mainState->register_class<Skeleton>();
        Skeleton::registerProxyMetaTable(*_mainState);


		//Register C and static member functions
		lua_register(*_mainState, "print", &print);
        lua_register(*_mainState, "printAdvLog", &printAdvLog);
        lua_register(*_mainState, "dbgPush", &dbgPush);
        lua_register(*_mainState, "dbgPop", &dbgPop);
		_mainState->register_class_static<Singletons>("getCurArea", &Singletons_getCurArea);
		_mainState->register_class_static<Singletons>("getCurLevel", &Singletons_getCurLevel);
		_mainState->register_class_static<Singletons>("getGameInput", &Singletons_getGameInput);
        _mainState->register_class_static<Singletons>("getMouseInput", &Singletons_getMouseInput);
		_mainState->register_class_static<Singletons>("setProcessInput", &Singletons_setProcessInput);
		_mainState->register_class_static<Singletons>("getAnimSys", &Singletons_getAnimationSystem);
		_mainState->register_class_static<Singletons>("getPartySys", &Singletons_getPartySystem);
        _mainState->register_class_static<Singletons>("getInputSys", &Singletons_getInputSystem);
		_mainState->register_class_static<Singletons>("getTimerSys", &Singletons_getTimerSystem);
		_mainState->register_class_static<Singletons>("getAudioSys", &Singletons_getAudioSystem);
		_mainState->register_class_static<Singletons>("getLightSys", &Singletons_getLightSystem);
		_mainState->register_class_static<Singletons>("getLevelSys", &Singletons_getLevelSystem);
        _mainState->register_class_static<Singletons>("getCombatSys", &Singletons_getCombatSystem);
		_mainState->register_class_static<Singletons>("getPartSys", &Singletons_getParticleSystem);
		_mainState->register_class_static<Singletons>("getDaySys", &Singletons_getDaySystem);
		_mainState->register_class_static<Singletons>("getRigidBodySys", &Singletons_getRigidBodySystem);
		_mainState->register_class_static<Singletons>("getDialogSys", &Singletons_getDialogSystem);
		_mainState->register_class_static<Singletons>("getGuiSys", &Singletons_getGuiSystem);
		_mainState->register_class_static<Singletons>("getCameraSys", &Singletons_getCameraSystem);
		_mainState->register_class_static<Singletons>("getCharSys", &Singletons_getCharacterSystem);

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



EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Singletons)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Singletons)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(gyro)
EXPORT_OOLUA_FUNCTIONS_0_CONST(gyro)
