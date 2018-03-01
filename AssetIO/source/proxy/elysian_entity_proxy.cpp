#include <proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype Entity
    \since 1.5.8
    \inmodule Engine
    \brief Class representing all non-terrain Game Objects.

    \l{Entity} is the central class for representing any game object that inhabits an \l{Area}.
    An Entity can be a player, a warp, an NPC, an enemy, a bookcase, an invisible
    trigger region, a particle emitter, a dynamic light, a positional audio source,
    and virtually anything else.

    \section2 Entities as a Collection of Components
    Entities are an aggregate of Components, which may either be built into the engine
    or created custom as Behaviors. By attaching Components to an Entity, you are
    able to add to its run-time capabilities and behaviors. For example, adding a Renderable
    Component to an Entity makes it visible as either a rectangle or sprite. Adding a
    Collidable Component to an Entity makes it solid in the game world. Adding an AudioSource
    to an Entity gives it the ability to emit 3D positional audio. An Entity is basically a blank
    slate from which any non-tile game object can be constructed.

    Another powerful aspect of the Entity/Component architecture is the fact that these Components
    can be added and removed to modify the characteristics of an Entity at run-time, rather than having
    to statically define them at compile-time through the use of inheritance, as in traditional engines.
    This means the player could dyamically turn into a warp that swallows up the rest of the party
    and shits out a million particles with a crazy flash of dynamic light before morphing back into
    himself by simply adding and removing components... because why the fuck not?

    \section2 Built-in Components
    The following is a table of all the Components built into the engine and the capabilities they add
    to an Entity when attached.

    \table
    \header
        \li \l{Component} Type
        \li Capabilities
    \row
        \li \l{Renderable}
        \li Draws the Entity as a sprite or quad.
    \row
        \li \l{Skeleton}
        \li Draws the Entity as a Spine Skeleton with animation support.
    \row
        \li \l{Collidable}
        \li Gives the Entity solidity.
    \row
        \li \l{RigidBody}
        \li Gives the Entity rigid body physical traits and adds it to the physics simulator.
    \row
        \li \l{AudioSource}
        \li Allows the Entity to play positional sounds in 3D space.
    \row
        \li \l{Character}
        \li Gives the Entity the ability to jump, pick up objects, and other characteristics common to players, NPCs, or enemies.
    \row
        \li \l{Combat}
        \li Gives the Entity battle stats and allows it to engage in battle with other Entities.
    \row
        \li \l{Examinable}
        \li Gives the Entity dialog or a description.
    \row
        \li \l{LightSource}
        \li Gives the Entity a point, spot, or directional dynamic light.
    \row
        \li \l{ParticleEmitter}
        \li Allows the Entity to spawn particles for special effects.
    \row
        \li \l{Warp}
        \li Makes the Entity transport the player from one location to another upon collision.
    \endtable

    \section2 Behaviors as Custom Components
    \l{Behavior} is a special type of dynamic Component which allows you to create your own custom
    Components and extend Entity functionality beyond what was built into the engine. A \l{LuaBehavior}
    is a special kind of \l{Behavior} which is implemented within a Lua script, allowing you to use
    the Lua scriptig language to extend the engine. See the \l{Behavior} and \l{LuaBehavior} documentation
    for more details.

    \sa Component, Renderable, Collidable, Skeleton, RigidBody, Character, AudioSource, Combat, Examinable,
    LightSource, ParticleEmitter, Warp, Behavior, LuaBehavior
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
 * \qmlproperty Behavior Entity::behaviors
 *
 * \sa getBehaviorByIndex(), setBehaviorByIndex()
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
 * \qmlmethod void Entity::setPlaneType(gyro::RECT_PLANE_TYPE plane)
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
 * \qmlmethod bool Entity::isActive()
 *
 * Returns whether the current entity is active within an area or not
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
 * \qmlmethod Behavior* Entity::getBehaviorByIndex(int index)
 *
 * Returns the Behavior component at the specified index or nil if there isn't one.
 */
/*!
 * \qmlmethod int Entity::getBehaviorCount()
 *
 * Returns the number of Behavior components attached to the current entity.
 */
/*!
 * \qmlmethod void Entity::setBehaviorByIndex(int index, Lua_table_ref valueTable)
 *
 * Sets the Behavior at the specified index to the valueTable or inserts a new one if the specified index doesn't exist.
 */




























//OOLUA_EXPORT_FUNCTIONS(LuaVariant)
//OOLUA_EXPORT_FUNCTIONS_CONST(LuaVariant)




OOLUA_EXPORT_FUNCTIONS(RenderableObject,
                                    getTransform)
OOLUA_EXPORT_FUNCTIONS_CONST(RenderableObject)


OOLUA_EXPORT_FUNCTIONS(Entity,
                        move,
                        setName,
                        setElev,
                        addRenderable,
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
                       setBehaviorByIndex)


OOLUA_EXPORT_FUNCTIONS_CONST(Entity,
                                getName,
                                getElev,
                                getRenderable,
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
                                getBehaviorByIndex,
                                getBehaviorCount,
                                isActive)
