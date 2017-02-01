#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_level_proxy.hpp>
#include <proxy/elysian_area_proxy.hpp>
#include <proxy/elysian_gui_system_proxy.hpp>
#include <proxy/elysian_level_system_proxy.hpp>
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
        MenuEntity::registerProxyMetaTable(*_mainState);
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
        lua_register(*_mainState, "printAdvLog", &printAdvLog);
        lua_register(*_mainState, "dbgPush", &dbgPush);
        lua_register(*_mainState, "dbgPop", &dbgPop);
        _mainState->register_class_static<Engine>("getCurArea", &Singletons_getCurArea);
        _mainState->register_class_static<Engine>("getCurLevel", &Singletons_getCurLevel);
        _mainState->register_class_static<Engine>("setProcessInput", &Singletons_setProcessInput);
        _mainState->register_class_static<Engine>("getPartySys", &Singletons_getPartySystem);
        _mainState->register_class_static<Engine>("getInputSys", &Singletons_getInputSystem);
        _mainState->register_class_static<Engine>("getTimerSys", &Singletons_getTimerSystem);
        _mainState->register_class_static<Engine>("getAudioSys", &Singletons_getAudioSystem);
        _mainState->register_class_static<Engine>("getLightSys", &Singletons_getLightSystem);
        _mainState->register_class_static<Engine>("getLevelSys", &Singletons_getLevelSystem);
        _mainState->register_class_static<Engine>("getCombatSys", &Singletons_getCombatSystem);
        _mainState->register_class_static<Engine>("getPartSys", &Singletons_getParticleSystem);
        _mainState->register_class_static<Engine>("getDaySys", &Singletons_getDaySystem);
        _mainState->register_class_static<Engine>("getRigidBodySys", &Singletons_getRigidBodySystem);
        _mainState->register_class_static<Engine>("getDialogSys", &Singletons_getDialogSystem);
        _mainState->register_class_static<Engine>("getGuiSys", &Singletons_getGuiSystem);
        _mainState->register_class_static<Engine>("getCameraSys", &Singletons_getCameraSystem);
        _mainState->register_class_static<Engine>("getCharSys", &Singletons_getCharacterSystem);

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
    \inmodule ESToolkit
    \brief Static class for Engine providing access to all subsystems.

    Engine is a static class (class of all static functions) providing access
    to the various subsystems and the current \l{Level} and \l{Area}.

    \sa PartySystem, InputSystem, TimerSystem, AudioSystem, LightSystem, LevelSystem,
    CombatSystem, ParticleSystem, DaySystem, RigidBodySystem, GuiSystem, DaySystem,
    CharacterSystem, CameraSystem, DialogSystem
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

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Engine)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Engine)


/*!
    \qmltype gyro
    \since 1.6.0
    \inmodule ESToolkit
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
