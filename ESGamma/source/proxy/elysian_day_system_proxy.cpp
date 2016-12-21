#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_day_system_proxy.hpp>

/*!
    \qmltype DaySystem
    \since 1.5.8
    \inmodule ESToolkit
    \brief Engine subsystem handling day/night-related logic.

    The DaySystem is the \l{Engine} subsystem handling all in-game time and a
    directional light that can be added to outdoor \l{Area}s representing the
    sun, which rises and sets according to in-game time.

    \section2 Using the Sun
    To use the sun for an outdoor level, the Area::useSun property should be
    set to true, and the Area::ambientColor should be set low enough so that
    the sun's diffuse color adequately lights the \l{Area} to represent day.

    \section2 In-Game Time
    The in-game time period tries to mimic real-life time by having 24 hours
    in a day. At 0:00, the sun begins to rise, at 12:00 it's at its peak, and
    begins slowly setting until the cycle begins anew.

    \section2 Retrieving the DaySystem
    The DaySystem object can be retrieved through the static \l{Engine} class:
    \code
    daySys = Engine.getDaySys()
    \endcode
    \sa LightSource, Area, Engine
*/

/*!
 * \qmlmethod int DaySystem::getDays()
 * Gets the total number of days that have elapsed since the engine has been running.
 * \sa getHours(), getMinutes()
 */
/*!
 * \qmlmethod int DaySystem::getHours()
 * Gets the in-game hour (0-24).
 * \sa getDays(), getMinutes()
 */
/*!
 * \qmlmethod int DaySystem::getMinutes()
 * Gets the in-game minute (0-60).
 * \sa getDays(), getHours()
 */
/*!
 * \qmlmethod LightAttrib* DaySystem::getSunLightAttrib()
 * Returns a reference to the \l{LightAttrib} object used as the LightSource::attrib
 * property for the sun's \l{LightSource} component. You can modify the properties of
 * the sun through this object, such as its colors or intensity.
 */
/*!
 * \qmlmethod void DaySystem::setTimeDilation(float multiplier)
 * Sets the time multiplier used to map real-time to in-game time to \a{multiplier}.
 * By default, this value is set to 60.0, meaning in-game time runs 60 times faster
 * than real-life time (ie a minute elapses in a second, an hour elapses in a minute, etc).
 */
/*!
 * \qmlmethod void DaySystem::setTimeOfDay(float time)
 * Sets the time of the day to \a{time}, where \a{time} is a normalized float representing the hour,
 * such that 0.0 is hour 0, 0.5 is hour 12, and 1.0 is hour 24.
 */



OOLUA_EXPORT_FUNCTIONS_CONST(DaySystem,
                             getDays,
                             getHours,
                             getMinutes,
                             getSunLightAttrib)
OOLUA_EXPORT_FUNCTIONS(DaySystem,
                       setTimeDilation,
                  //    setSunEnabled,
                       setTimeOfDay)

