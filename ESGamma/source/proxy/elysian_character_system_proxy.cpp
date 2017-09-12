#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_character_system_proxy.hpp>

/*!
    \qmltype CharacterSystem
    \since 1.5.8
    \inmodule Engine
    \brief Engine subsystem handling character-related logic and components.

    The CharacterSystem is the \l{Engine} subsystem responsible for managing
    all active \l{Character} \l{Component}s within the current \l{Area}. It also
    offers a few useful functions for controlling movement asynchronously.

    \section2 Retrieving the CharacterSystem
    The CharacterSystem object can be retrieved through the static Engine class:
    \code
    charSys = Engine.getCharSys()
    \endcode
    \sa {Character}, {Engine}
*/
/*!
 * \qmlmethod void CharacterSystem::walkToWayPoint(Entity* entity, float x, float y, EventMonitor* monitor)
 *  Makes the given \a{entity} walk in a straight line from its current location to the 2D location given by
 * <\a{x}, \a{y}>. The \l{EventMonitor} provied by \a{monitor} is notified once the \l{Entity} arrives. This
 * uses the \l{Character} \l{Component} to walk, so one must be attached, and animations and speed are handled
 * automatically.
 * \sa moveToWayPoint()
 */
/*!
 * \qmlmethod void CharacterSystem::moveToWayPoint(Entity* entity, float x, float y, float z, EventMonitor* monitor)
 * Makes the given \a{entity} move in a straight line from its current location to the 3D position given by
 * <\a{x}, \a{y}, \a{z}>. Once the \l{Entity} arrives, the \l{EventMonitor} given by \a{monitor} will be notified.
 *
 * The movement is handled as a force applied to a \l{RigidBody} \l{Component}, so one must be attached, and
 * animations will not be handled.
 * \sa walkToWayPoint()
 */

OOLUA_EXPORT_FUNCTIONS(CharacterSystem,
                       walkToWayPoint,
                       moveToWayPoint)
OOLUA_EXPORT_FUNCTIONS_CONST(CharacterSystem)
