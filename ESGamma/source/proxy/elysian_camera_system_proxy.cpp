#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_camera_system_proxy.hpp>


/*!
    \qmltype CameraKeyFrame
    \inherits InstanceTransform
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a Camera transform state.

    A CameraKeyFrame represents the transform of a camera. It can be used for setting
    the state of a camera or for transitioning a camera to another state in the
    \l{CameraSystem}.

    A CameraKeyFrame inherits a size, scale, and orientation from \l{InstanceTransform}.

    \sa CameraSystem
*/


OOLUA_EXPORT_FUNCTIONS_CONST(CameraKeyFrame)
OOLUA_EXPORT_FUNCTIONS(CameraKeyFrame)


/*!
    \qmltype CameraSystem
    \since 1.5.8
    \inmodule ESToolkit
    \brief Engine subsystem handling all Camera-related logic, transformations, and states.

    The CameraSystem is the engine subystem for managing all aspects of the camera and any
    effects applied to the camera. When in dynamic mode, the CameraSystem follows the player
    and tries to keep combatants onscreen during Combat. Boundaries can be set
    for the CameraSystem so that the camera does not scroll too far off of the map. By default,
    the boundaries are the \l{Area} size.

    \section2 Retrieving the CameraSystem
    The CameraSystem object can be retrieved through the static Engine class:
    \code
    camSys = Engine.getCameraSys()
    \endcode
    \sa {CameraKeyFrame}, {Engine}, {Cutscenes}
*/

/*!
 * \qmlmethod void CameraSystem::scale(float factor, float seconds, EventMonitor* monitor)
 *
 * Uniformly scales (zooms) the camera by \a factor over \a seconds. After the scale is done, \a monitor is notified. Linear interpolation is used.
 */
/*!
 * \qmlmethod void CameraSystem::moveToKeyFrame(const CameraKeyFrame& nextKeyFrame, float seconds, EventMonitor* monitor)
 *
 * Linearly interpolates the Camera from the current keyframe to \a nextKeyFrame over \a seconds. \a monitor is notified when the operation is complete.
 * \sa CameraKeyFrame
 */
/*!
 * \qmlmethod void CameraSystem::setBoundaries(float top, float bottom, float left, float right)
 *
 * Sets the bounding rectangle of the Camera to the rectangular region formed by the arguments.
 */
/*!
 * \qmlmethod void CameraSystem::setDynamic(bool val)
 *
 * Changes whether the Camera is updated dynamically or not. Setting this to false gives Lua full control over the Camera. Setting it to true tells the engine to update the Camera.
 */
/*!
 * \qmlmethod void CameraSystem::shake(const Vector3& displacement, unsigned int oscillations, float seconds, bool fade, EventMonitor* monitor)
 *
 * Begins a shake effect on the Camera. This can be used to simulate events like earthquakes.
 * The \a displacement is the maximum distance the camera will move. \a oscillations is the number of times the camera will shake.
 * \a fade makes the effect happen smoothly and each oscillation become less and less prominent.
 * The effect is applied over \a seconds and \a monitor is notified upon completion.
 */
/*!
 * \qmlmethod void CameraSystem::pushPerspectiveState()
 *
 * Pushes the debug 3D perspective Camera as the current Camera state. This must be popped later to restore the Camera to its original state.
 * \sa popPerspectiveState()
 */
/*!
 * \qmlmethod void CameraSystem::popPerspectiveState()
 *
 * Pops the debug 3D perspective and restores the normal perspective as the Camera's current state.
 * \sa pushPerspectiveState()
 */
/*!
 * \qmlmethod void CameraSystem::setFollowEntity(Entity* entity)
 *
 * Sets the camera to automatically follow the given \a{entity} when in dynamic mode.
 * By default, the camera follows the party leader \l{Entity}.
 */
/*!
 * \qmlmethod void CameraSystem::addEntity(Entity* entity)
 *
 * Adds the given \a{entity} to the list of Entities the camera attempts to keep in view
 * when in dynamic mode. By default, the \l{CombatSystem} adds and removes combatants in this
 * manner to keep them on screen.
 *
 * \e {NOTE: Any \l{Entity} added in this manner must also be removed with removeEntity().}
 * \sa removeEntity()
 */
/*!
 * \qmlmethod void CameraSystem::removeEntity(Entity* entity)
 *
 * Removes the given \a{entity} from the list of Entities the camera attempts to keep in view
 * when in dynamic mode.
 *
 * \sa addEntity()
 */

OOLUA_EXPORT_FUNCTIONS(CameraSystem,
                       scale,
                       moveToKeyFrame,
                       setBoundaries,
                       setDynamic,
                       pushPerspectiveState,
                       popState,
                       shake,
                       setFollowEntity)
OOLUA_EXPORT_FUNCTIONS_CONST(CameraSystem,
                       addEntity,
                       removeEntity,
                       getKeyFrame)
