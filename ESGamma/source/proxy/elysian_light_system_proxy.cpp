#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_light_system_proxy.hpp>

/*!
    \qmltype LightSystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem for managing dynamic lights and LightSource Components.

    The LightSystem is the \l{Engine} subsystem for managing all aspects of dynamic lighting,
    shadow mapping, bloom lighting, etc. It assumes ownership and operates upon any \l{Entity}
    within the current \l{Area} containing a \l{LightSource}.

    \section2 Retrieving the LightSystem
    The LightSystem object can be retrieved through the static \l{Engine} class:
    \code
    lightSys = Engine.getLightSys()
    \endcode

    \section2 Configuring Lighting
    Lighting is not actually configured directly through this subsystem. It operates upon data within
    the current \l{Area} instead. For creating spot, point, or directional dynanmic lights, reference
    \l{LightSource}. For simply setting the base or ambient light within a scene, reference the
    \l{Area::ambientColor} property.

    \sa LightSource, LightAttrib, Area, Engine
*/

/*!
 * \qmlmethod void LightSystem::interpLightAttrib(float secs, LightAttrib& target, LightAttrib* source, EventMonitor monitor)
 *
 * Utility function provided for letting the engine handle dynamic light animations by linearly interpolating the \l{LightAttrib} object given by \a{source}
 * into the values of the object given by \a{target} over the duration (in seconds) given by \a{secs}. Upon completion, when \a{secs} seconds have elapsed,
 * the \l{EventMonitor} given by \a{monitor} will be notified.
 *
 * For example, if \a{source} is a dim blue diffuse light while \a{dest} is a bright red diffuse light, \a{source} will gradually morph into
 * \a{dest} over the given time interval.
 * \sa LightAttrib, EventMonitor
 */


OOLUA_EXPORT_FUNCTIONS(LightSystem,
                       interpLightAttrib)
OOLUA_EXPORT_FUNCTIONS_CONST(LightSystem)
