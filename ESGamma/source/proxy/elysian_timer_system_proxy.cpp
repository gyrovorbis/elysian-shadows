#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_timer_system_proxy.hpp>


/*!
    \qmltype TimerSystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem handling timers and timing-related logic.

    The TimerSystem is the \l{Engine} subsystem responsible for limiting framerate
    (when above 60fps), calculating the detaTime between frames, maintaining elapsed
    time since the engine began running, and impementing general-purpose timers.

    \i {NOTE: System "tick" resolution is 1ns (nanosecond) or 1x10^-9 secs.}

    \section2 Retrieving the TimerSystem
    The TimerSystem object can be retrieved through the static \l{Engine} class:
    \code
    timerSys = Engine.getTimerSys()
    \endcode

    \section2 Framerate-Indepndent Movement and Calculations
    In order to avoid movements and animations that are tied to framerate--where the game slowing
    down makes players walk slower or animations take longer--all timing-specific logic should be in
    the unit of seconds and should use TimerSystem::getDeltaTime() as a scaling factor.

    \sa Engine
*/

/*!
 * \qmlmethod unsigned long TimerSystem::getTotalTicks()
 * Retrieves the total number of ticks (nanoseconds) that have elapsed since the
 * engine began running.
 * \sa getTotalTime
 */

/*!
 * \qmlmethod unsigned long TimerSystem::getCurrentFrameTicks()
 * Retrieves the number of ticks that have elapsed since this frame began. Engine resets this value
 * at the beginning of every frame.
 */

/*!
 * \qmlmethod double TimerSystem::getTotalTime()
 * Retrieves the total number of seconds that have elapsed since the engine began running.
 * \sa getTotalTicks
 */

/*!
 * \qmlmethod float TimerSystem::getDeltaTime()
 * Retrieves the total number of seconds that have elapsed since the previous frame. Use this value
 * to scale all framerate-independent calculations and movement.
 */


/*!
 * \qmlmethod float TimerSystem::startTimerEvent(float duration, EventMonitor* monitor)
 * Starts a timer counting until the number of seconds supplied by \a{duration} elapses before
 * notifying the provided \a{monitor} that the timer has expired. If you require a cyclic timer,
 * just reschedule the timer repeatedly by calling this function from the \l{EventMonitor}'s callback
 * function.
 *
 * \sa EventMonitor
 */

OOLUA_EXPORT_FUNCTIONS_CONST(TimerSystem,
                                getTotalTicks,
                                getCurrentFrameTicks,
                                getDeltaTime,
                                getTotalTime,
                                getSpeedMultiplier)
OOLUA_EXPORT_FUNCTIONS(TimerSystem,
                       startTimerEvent,
                       setSpeedMultiplier)
