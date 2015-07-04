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



/*!
    \qmltype Entity
    \since 1.5.8
    \inmodule ESToolkit
    \brief Class representing all non-terrain Game Objects. An entity is
    an aggregate of Components, which may either be built into the engine
    or created custom as LuaBehaviors.
*/

/*!
 * \qmlproperty InstanceTransform Entity::transform
 *
 * \sa getTransform(), setTransform()
 */

/*!
 * \qmlproperty string Entity::name
 *
 * \sa getName(), setName()
 */
/*!
 * \qmlproperty string Entity::dispName
 *
 * \sa getDispName(), setDispName()
 */
/*!
 * \qmlproperty int Entity::elev
 *
 * \sa getElev(), setElev()
 */
/*!
 * \qmlproperty int Entity::planeType
 *
 * \sa getPlaneType(), setPlaneType()
 */
/*!
 * \qmlproperty bool Entity::shadowVolume
 *
 * \sa isShadowVolume(), setShadowVolume()
 */
/*!
 * \qmlproperty bool Entity::persistent
 *
 * \sa isPersistent(), setPersistent()
 */
/*!
 * \qmlproperty Renderable Entity::renderable
 *
 * \sa getRenderable(), setRenderable()
 */
/*!
 * \qmlproperty Animation Entity::animation
 *
 * \sa getAnimation(), setAnimation()
 */
/*!
 * \qmlproperty Collidable Entity::collidable
 *
 * \sa getCollidable(), setCollidable()
 */
/*!
 * \qmlproperty RigidBody Entity::rigidBody
 *
 * \sa getRigidBody(), setRigidBody()
 */
/*!
 * \qmlproperty Character Entity::character
 *
 * \sa getCharacter(), setCharacter()
 */
/*!
 * \qmlproperty Warp Entity::warp
 *
 * \sa getWarp(), setWarp()
 */
/*!
 * \qmlproperty Skeleton Entity::skeleton
 *
 * \sa getSkeleton(), setSkeleton()
 */
/*!
 * \qmlproperty LightSource Entity::lightSource
 *
 * \sa getLightSource(), setLightSource()
 */
/*!
 * \qmlproperty AudioSource Entity::audioSource
 *
 * \sa getAudioSource(), setAudioSource()
 */
/*!
 * \qmlproperty Examinable Entity::examinable
 *
 * \sa getExaminable(), setExaminable()
 */
/*!
 * \qmlproperty Combat Entity::combat
 *
 * \sa getCombat(), setCombat()
 */
/*!
 * \qmlproperty ParticleEmitter Entity::particleEmitter
 *
 * \sa getParticleEmitter(), setParticleEmitter()
 */
/*!
 * \qmlproperty LuaBehavior Entity::luaBehaviors
 *
 * \sa getLuaBehavior(), setLuaBehaviorByIndex()
 */








/*!
 * \qmlmethod InstanceTransform& Entity::getTransform()
 *
 * Returns a reference to the entity's transform.
 */
/*!
 * \qmlmethod void Entity::setTransform(Lua_table_ref valueTable)
 *
 * Sets the entity's transform to the value supplied in valueTable.
 */
/*!
 * \qmlmethod bool Entity::isShadowVolume()
 *
 * Returns true or false depending on whether the entity casts shadows from dynamic lights or not.
 */
/*!
 * \qmlmethod void Entity::setShadowVolume(bool value)
 *
 * Sets whether the current entity casts shadows from dynamic light sources or not.
 */
/*!
 * \qmlmethod gyro.RECT_PLANE_TYPE Entity::getPlaneType()
 *
 * Returns the plane the current entity is tied to, as one of the faces of a cube.
 */
/*!
 * \qmlmethod void Entity::setPlaneType(gyro.RECT_PLANE_TYPE plane)
 *
 * Sets the plane the current entity is tied to, as one of the faces of a cube.
 */
/*!
 * \qmlmethod bool Entity::isPersistent()
 *
 * Returns true if the entity is to be loaded and saved as part of the current area.
 */
/*!
 * \qmlmethod void Entity::setPersistent(bool value)
 *
 * Flags the entity for being saved and loaded as part of the current area or not.
 */


/*!
 * \qmlmethod void Entity::move(GYVector2& translationVec)
 *
 * Translates the entity by the provided vector, using a walking animation if applicable.
 */
/*!
 * \qmlmethod void Entity::setName(string name)
 *
 * Sets the entity's name to the provided name. The engine requires these names to be unique, so it will append
 * a number to the end if a duplicate exists when adding to the area.
 */
/*!
 * \qmlmethod string Entity::getName()
 *
 * Returns the (unique) name of the entity
 */
/*!
 * \qmlmethod void Entity::setDispName(string name)
 *
 * Sets the entity's display name to the provided name. This is what is shown as in-game text and does not have to be unique.
 */
/*!
 * \qmlmethod string Entity::getDispName()
 *
 * Returns the displayName of the entity.
 */
/*!
 * \qmlmethod void Entity::setElev(int elev)
 *
 * Sets the entity's elevation to "elev," and the entity's transform's position's z coordinate to elev*32.
 */
/*!
 * \qmlmethod int Entity::getElev()
 *
 * Returns the elevation of the current entity, equal to its transform's position's z / 32.
 */
/*!
 * \qmlmethod Renderable* Entity::addRenderable()
 *
 * Creates and adds a Renderable component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Animation* Entity::addAnimation()
 *
 * Creates and adds an Animation component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Collidable* Entity::addCollidable()
 *
 * Creates and adds a Collidable component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod RigidBody* Entity::addRigidBody()
 *
 * Creates and adds a RigidBody component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Character* Entity::addCharacter()
 *
 * Creates and adds a Character component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Examinable* Entity::addExaminable()
 *
 * Creates and adds an Examinable component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod LightSource* Entity::addLightSource()
 *
 * Creates and adds a LightSource component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod AudioSource* Entity::addAudioSource()
 *
 * Creates and adds an AudioSource component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod ParticleEmitter* Entity::addParticleEmitter()
 *
 * Creates and adds a ParticleEmitter component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Combat* Entity::addCombat()
 *
 * Creates and adds a Combat component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Warp* Entity::addWarp()
 *
 * Creates and adds a Warp component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod Skeleton* Entity::addSkeleton()
 *
 * Creates and adds a Skeleton component to the entity (if one doesn't already exist) and returns a reference to the component.
 */
/*!
 * \qmlmethod LuaBehavior* Entity::addLuaBehavior(string scriptPath, Lua_table_ref params)
 *
 * Creates and adds a LuaBehavior component to the entity (if one doesn't already exist), with the provided parameter table, instantiates the behavior script specified at "scriptPath," and returns a reference to the component.
 */


/*!
 * \qmlmethod Renderable* Entity::getRenderable()
 *
 * Returns the entity's attached renderable component or nil if there isn't one.
 */
/*!
 * \qmlmethod Animation* Entity::getAnimation()
 *
 * Returns the entity's attached Animation component or nil if there isn't one.
 */
/*!
 * \qmlmethod Collidable* Entity::getCollidable()
 *
 * Returns the entity's attached Collidable component or nil if there isn't one.
 */
/*!
 * \qmlmethod RigidBody* Entity::getRigidBody()
 *
 * Returns the entity's attached RigidBody component or nil if there isn't one.
 */
/*!
 * \qmlmethod Character* Entity::getCharacter()
 *
 * Returns the entity's attached Character component or nil if there isn't one.
 */
/*!
 * \qmlmethod Examinable* Entity::getExaminable()
 *
 * Returns the entity's attached Examinable component or nil if there isn't one.
 */
/*!
 * \qmlmethod LightSource* Entity::getLightSource()
 *
 * Returns the entity's attached LightSource component or nil if there isn't one.
 */
/*!
 * \qmlmethod AudioSource* Entity::getAudioSource()
 *
 * Returns the entity's attached AudioSource component or nil if there isn't one.
 */
/*!
 * \qmlmethod ParticleEmitter* Entity::getParticleEmitter()
 *
 * Returns the entity's attached ParticleEmitter component or nil if there isn't one.
 */
/*!
 * \qmlmethod Combat* Entity::getCombat()
 *
 * Returns the entity's attached Combat component or nil if there isn't one.
 */
/*!
 * \qmlmethod Warp* Entity::getWarp()
 *
 * Returns the entity's attached Warp component or nil if there isn't one.
 */
/*!
 * \qmlmethod Skeleton* Entity::getSkeleton()
 *
 * Returns the entity's attached Skeleton component or nil if there isn't one.
 */
/*!
 * \qmlmethod LuaBehavior* Entity::getLuaBehavior(string scriptPath)
 *
 *  Returns the LuaBehavior component instantiated from the given script.
 */


/*!
 * \qmlmethod void Entity::setRenderable(Lua_table_ref valueTable)
 *
 * Creates a new Renderable component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setAnimation(Lua_table_ref valueTable)
 *
 * Creates a new Animation component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setCollidable(Lua_table_ref valueTable)
 *
 * Creates a new Collidable component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setRigidBody(Lua_table_ref valueTable)
 *
 * Creates a new RigidBody component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setCharacter(Lua_table_ref valueTable)
 *
 * Creates a new Character component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setExaminable(Lua_table_ref valueTable)
 *
 * Creates a new Examinable component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setLightSource(Lua_table_ref valueTable)
 *
 * Creates a new LightSource component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setAudioSource(Lua_table_ref valueTable)
 *
 * Creates a new AudioSource component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setParticleEmitter(Lua_table_ref valueTable)
 *
 * Creates a new ParticleEmitter component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setCombat(Lua_table_ref valueTable)
 *
 * Creates a new Combat component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setWarp(Lua_table_ref valueTable)
 *
 * Creates a new Warp component and sets it equal to valueTable.
 */
/*!
 * \qmlmethod void Entity::setSkeleton(Lua_table_ref valueTable)
 *
 * Creates a new Skeleton component and sets it equal to valueTable.
 */


/*!
 * \qmlmethod void Entity::removeRenderable()
 *
 * Removes and deletes the attached Renderable component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeAnimation()
 *
 * Removes and deletes the attached Animation component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeCollidable()
 *
 * Removes and deletes the attached Collidable component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeRigidBody()
 *
 * Removes and deletes the attached RigidBody component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeCharacter()
 *
 * Removes and deletes the attached Character component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeExaminable()
 *
 * Removes and deletes the attached Examinable component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeLightSource()
 *
 * Removes and deletes the attached LightSource component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeAudioSource()
 *
 * Removes and deletes the attached AudioSource component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeParticleEmitter()
 *
 * Removes and deletes the attached ParticleEmitter component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeCombat()
 *
 * Removes and deletes the attached Combat component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeWarp()
 *
 * Removes and deletes the attached Warp component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeSkeleton()
 *
 * Removes and deletes the attached Skeleton component (if it exists).
 */
/*!
 * \qmlmethod void Entity::removeLuaBehavior(string scriptPath)
 *
 * Removes and deletes the attached LuaBehavior instantiated from the LuaScript at "scriptPath."
 */
/*!
 * \qmlmethod LuaBehavior* Entity::getLuaBehaviorByIndex(int index)
 *
 * Returns the LuaBehavior component at the specified index or nil if there isn't one.
 */
/*!
 * \qmlmethod int Entity::getLuaBehaviorCount()
 *
 * Returns the number of LuaBehavior components attached to the current entity.
 */
/*!
 * \qmlmethod void Entity::setLuaBehaviorByIndex(int index, Lua_table_ref valueTable)
 *
 * Sets the LuaBehavior at the specified index to the valueTable or inserts a new one if the specified index doesn't exist.
 */

































OOLUA_EXPORT_FUNCTIONS(RenderableObject,
                                    getTransform)
OOLUA_EXPORT_FUNCTIONS_CONST(RenderableObject)


OOLUA_EXPORT_FUNCTIONS(Entity,
                        move,
                        setName,
                        setElev,
                        addRenderable,
                        addAnimation,
                        addCollidable,
                        addCharacter,
                        addWarp,
                        addAudioSource,
                        addLightSource,
                        addExaminable,
                        addRigidBody,
                        addCombat,
                        addParticleEmitter,
                        addSkeleton,
                        addBehavior,
                        addLuaBehavior,
                        removeRenderable,
                        removeAnimation,
                        removeCollidable,
                        removeCharacter,
                        removeWarp,
                        removeAudioSource,
                        removeLightSource,
                        removeExaminable,
                        removeRigidBody,
                        removeCombat,
                        removeParticleEmitter,
                        removeSkeleton,
                        removeBehavior,
                        removeLuaBehavior,
                        setSelfTable,
                        getSelfTable,
                        setShadowDepth,
                        setShadowVolume,
                       setPlaneType,
                       setTransform,
                       setRenderable,
                       setAnimation,
                       setCollidable,
                       setRigidBody,
                       setCharacter,
                       setWarp,
                       setAudioSource,
                       setLightSource,
                       setParticleEmitter,
                       setCombat,
                       setExaminable,
                       setSkeleton,
                       setPersistent,
                       setDispName,
                       setLuaBehaviorByIndex)


OOLUA_EXPORT_FUNCTIONS_CONST(Entity,
                                getName,
                                getElev,
                                getRenderable,
                                getAnimation,
                                getCollidable,
                                getCharacter,
                                getWarp,
                                getAudioSource,
                                getLightSource,
                                getExaminable,
                                getRigidBody,
                                getCombat,
                                getParticleEmitter,
                                getSkeleton,
                                getBehavior,
                                getLuaBehavior,
                                isShadowVolume,
                                getPlaneType,
                                getShadowDepth,
                                isPersistent,
                                getDispName,
                                getLuaBehaviorByIndex,
                                getLuaBehaviorCount)
