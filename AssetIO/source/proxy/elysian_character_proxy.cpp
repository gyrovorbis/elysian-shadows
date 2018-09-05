#include <AssetIO/proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype Direction
    \since 1.6.0
    \inmodule Engine
    \brief Static helper class containing functions relevant to directions in 2D/3D space.

    Static helper class containing methods relevant to directions in 2D and 3D space.
    Contains methods for converting a between a simple 4 directional value and a 2D unit vector.
*/

/*!
\qmlproperty enumeration Direction::DIR4_TYPE

Enumeration representing the 4 basic 2D directions.

The different direction types are:
\list
\li Direction.DIR4_CENTER  - Default (unset) direction.
\li Direction.DIR4_LEFT
\li Direction.DIR4_RIGHT
\li Direction.DIR4_UP
\li Direction.DIR4_DOWN
\endlist
*/


EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Direction)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Direction)

/*!
    \qmltype Character
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component containing common functionality for party, enemies, and NPCs.

    The Character Component is a Component that can be attached to an Entity. It contains various attributes such as
    facing direction, animation states, and walk speed and provides functionality such as dashing, picking up objects,
    and jumping required by party members, enemies, and NPCs.

    \section2 Prerequesite Components
    The Character Component requires the following Components to be present before it can be added:
    \list
    \li Collidable
    \li RigidBody
    \li Renderable/Skeleton
    \endlist

    \section2 Animation States
    The Character component is responsible for managing and playing built-in animations that are common to all
    characters. Each action that a Character can perform is represented by a different ANIM_STATE state.
    The Character component maintains an internal list of every state that is currently active.
    The correct animation is then deduced from the list of active states. For example, adding STATE_HOLD
    and STATE_JUMP to a Character results in an animation being played that is a combination of jumping and holding an object.

    \section2 Modifying Animation State
    As an example, to pick up an object, we need to tell the engine to put the Entity's arms in a holding position for
    subsequent animations.
    \code
    entity:getCharacter():addState(Character.STATE_HOLD)
    \endcode
    Now once we put the object down later, we need to signify to the engine that the arms can go back to the normal.
    \code
    entity:getCharacter():removeState(Character.STATE_HOLD)
    \endcode

    \section2 Triggering an Event from an Animation State
    When we play animations from a Lua script such as dying or swinging a sword, we may want to halt
    our logic until we know that the animation has been completed. For example, when the player dies, you want to wait
    until the death animation is complete before moving on to the "Game Over" screen. This kind of advanced animaton
    control can be accomplished by using a temporary state with a callback function
    \code
    local deathCount = 0
    entity:getCharacter():addTempState(Character.DEATH, -1, function()
        print("Death animation finished a single iteration!")
        deathCount = deathCount + 1
        if(deathCount >= 3) then
            return true     -- remove death state, show Game Over screen
        else
            return false    -- play the animation again
        end
    end)
    \endcode
    The preceding code segment will play the death animation 3 times before exiting. Character.DEATH is added as a
    temporary state and an anonymous function is provided that will be invoked every time the animation completes an
    iteration. If the function returns false, the animation continues playing. If it returns true, the engine will
    automatically remove the Character.DEATH state.

    \sa Animation
*/
/*!
\qmlproperty enumeration Character::ANIM_STATE

Enumeration representing the different animation states of a Character.

The animation states are:
\list
\li Character.STATE_STAND
\li Character.STATE_WALK
\li Character.STATE_RUN
\li Character.STATE_DASH
\li Character.STATE_JUMP
\li Character.STATE_FALL
\li Character.STATE_PUSH
\li Character.STATE_DRAG
\li Character.STATE_HOLD
\li Character.STATE_THROW
\li Character.STATE_COMBAT
\li Character.STATE_STRAFE
\li Character.STATE_KNOCKBACK
\li Character.STATE_DEATH
\li Character.STATE_LUA
\endlist
*/

/*!
 * \qmlproperty DIR4_TYPE Character::faceDir
 *
 * The basic 2D direction the Character is facing.
 * \sa Direction::DIR4_TYPE, getFaceDir(), setFaceDir(), getFaceDirVec()
 */
/*!
 * \qmlproperty float Character::walkForce
 *
 * The magnitude of the force vector applied to make the Character walk.
 * \sa getWalkForce(), setWalkForce()
 */
/*!
 * \qmlproperty float Character::dashMultiplier
 *
 * The multiplier applied to the walking force when dashing.
 * \sa getDashMultiplier(), setDashMultiplier()
 */
/*!
 * \qmlproperty Entity* Character::heldEntity
 *
 * Read-only property containing the Entity that is currently being held or nil if no Entity is currently held.
 * \sa getHeldEntity()
 */
/*!
 * \qmlproperty float Character::holdingDampFactor
 *
 * Read-only property for the damping factor applied to walking forces based on the held Entity's mass.
 * \sa getEntityForceDampFactor()
 */

/*!
 * \qmlmethod void Character::faceEntity(Entity* entity)
 *
 * Makes the current Entity face the Entity specified by \a{entity}.
 * \sa setFaceDir()
 */
/*!
 * \qmlmethod void Character::setFaceDir(DIR4_TYPE dir)
 *
 * Makes the current Entity face the direction given by \a{dir}.
 * \sa Direction::DIR4_TYPE, getFaceDir(), getFaceDirVec()
 */
/*!
 * \qmlmethod void Character::moveByForce(Vector2 force)
 *
 * Moves the current Entity by applying the 2D force vector given by \a{force}. Also animates the Entity.
 * \sa RigidBody
 */
/*!
 * \qmlmethod void Character::setAnimate(bool value)
 *
 * Enables animations for the given Character if \a{value} is true. Disables animations if \a{value} is false.
 */
/*!
 * \qmlmethod bool Character::isFacing(Entity* entity)
 *
 * Returns true if the Entity containing the Character component is facing the Entity given by \a{entity}.
 * Returns false otherwise.
 */
/*!
 * \qmlmethod DIR4_TYPE Character::getFaceDir()
 *
 * Returns the enumerated type representing the basic 2D direction the Character is facing.
 * \sa Direction::DIR4_TYPE, getFaceDirVec()
 */
/*!
 * \qmlmethod Vector2 Character::getFaceDir()
 *
 * Returns a normalized 2D vector in the direction the Entity is facing.
 * \sa getFaceDir(), setFaceDir()
 */

/*!
 * \qmlmethod void Character::setWalkForce(float mag)
 *
 * Sets the magnitude of the force applied when the Character walks to \a{mag}.
 * \sa getWalkForce()
 */
/*!
 * \qmlmethod void Character::setDashMultiplier(float factor)
 *
 * Sets the speed multiplier applied during a dash to \a{factor}.
 * \sa getDashMultiplier()
 */
/*!
 * \qmlmethod void Character::setDashing(bool value)
 *
 * If \a{value} is true, the current Entity's movements become a dash, otherwise they become a walk.
 */
/*!
 * \qmlmethod Entity* Character::getHeldEntity()
 *
 * Returns the Entity currently being held by the Entity containing the Character component, or nil
 * if it isn't holding an Entity.
 */
/*!
 * \qmlmethod Vector3 Character::getLastMoveDir()
 *
 * Returns the 3 dimensional normalized direction vector representing the direction of the Character's
 * movement during the previous frame.
 */
/*!
 * \qmlmethod float Character::getWalkForce()
 *
 * Returns the magnitude of the force vector to be applied to the Character for walking.
 * \sa setWalkForce()
 */
/*!
 * \qmlmethod float Character::getDashMultiplier()
 *
 * Returns the speed multiplier that is applied to walking for dashing.
 * \sa setDashMultiplier()
 */
/*!
 * \qmlmethod float Character::getEntityForceDampFactor()
 *
 * Returns the factor used to damp walking and dashing forces when holding an Entity. This is based on the
 * held Entity's mass.
 */
/*!
 * \qmlmethod void Character::addState(ANIM_STATE state)
 *
 * Adds the animation state given by \a{state} to the Character's set of Animation states.
 * \sa removeState()
 */
/*!
 * \qmlmethod void Character::addTempState(ANIM_STATE state, int track, Lua_function callback)
 *
 * Adds the state given by \a{state} to the Character's set of active states then removes
 * the state when the animation is done playing. The function given by \a{callback} is invoked when the
 * AnimationTrack given by \a{track} is complete. If \a{track} is -1, \a{callback} is invoked when
 * ALL tracks are complete. The \a{callback} function should return false if the animation is to continue
 * playing and true if the state should be removed and the animation should stop.
 */
/*!
 * \qmlmethod void Character::removeState(ANIM_STATE state)
 *
 * Removes the animation state given by \a{state} from the Character's list of active animation states.
 * \sa addState()
 */
/*!
 * \qmlmethod void Character::jump()
 *
 * Makes the Entity with the attached Character component jump.
 */
/*!
 * \qmlmethod void Character::applyKnockbackForce(Vector3 force)
 *
 * Applies the 3D force vector given by \a{force} to the Entity's RigidBody and
 * adds the animation state STATE_KNOCKBACK to the Entity. This is to be used for when
 * an Entity takes damage or is hit by an object to knock it back by recoil.
 */
/*!
 * \qmlmethod bool Character::hasState(ANIM_STATE state)
 *
 * Returns true if the animation state given by \a{state} is active in the Character
 * or returns false otherwise.
 * \sa addState(), removeState(), addTempState()
 */
/*!
 * \qmlmethod string Character::getAnimNameFromStates(DIR4_TYPE dir)
 *
 * Returns the string name of the current Animation given the Character's active states and
 * the direction give by \a{dir}.
 */


OOLUA_EXPORT_FUNCTIONS(Character,
                        faceEntity,
                        setFaceDir,
                        moveByForce,
                        setAnimate,
                        setWalkForce,
                        setDashMultiplier,
                        setDashing,
                        getEntityForceDampFactor,
                        addState,
                        removeState,
                        jump,
                        applyKnockbackForce,
                        addTempState)
OOLUA_EXPORT_FUNCTIONS_CONST(Character,
                            getFaceDir,
                            getFaceDirVec,
                            isFacing,
                            getHeldEntity,
                            getLastMoveDir,
                            getWalkForce,
                            getDashMultiplier,
                            hasState,
                            getAnimNameFromStates)
