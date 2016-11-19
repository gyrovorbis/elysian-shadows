#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_camera_system_proxy.hpp>


/*!
    \qmltype CameraKeyFrame
    \inherits InstanceTransform
    \since 1.5.8
    \inmodule ESToolkit
    \brief Represents a Camera transform state.
*/


OOLUA_EXPORT_FUNCTIONS_CONST(CameraKeyFrame)
OOLUA_EXPORT_FUNCTIONS(CameraKeyFrame)


/*!
    \qmltype CameraSystem
    \since 1.5.8
    \inmodule ESToolkit
    \brief Engine subsystem handling all Camera-related logic, transformations, and states.
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
 */
/*!
 * \qmlmethod void CameraSystem::popPerspectiveState()
 *
 * Pops the debug 3D perspective and restores the normal perspective as the Camera's current state.
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
