#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_timer_system_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS_CONST(TimerSystem,
                                getTotalTicks,
                                getCurrentFrameTicks,
                                getDeltaTime,
                                getTotalTime)
OOLUA_EXPORT_FUNCTIONS(TimerSystem,
                       startTimerEvent)
