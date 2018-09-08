#ifndef ELYSIAN_TIMER_SYSTEM_PROXY_HPP
#define ELYSIAN_TIMER_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/crtp/elysian_timer_system.hpp>

namespace OOLUA {
    typedef elysian::TimerSystem    TimerSystem;
    typedef elysian::EventMonitor   EventMonitor;

}

OOLUA_PROXY(TimerSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getTotalTicks)
    OOLUA_MFUNC_CONST(getCurrentFrameTicks)
    OOLUA_MFUNC_CONST(getDeltaTime)
    OOLUA_MFUNC_CONST(getTotalTime)
    OOLUA_MEM_FUNC(void, startTimerEvent, float, cpp_in_p<EventMonitor*>)
    OOLUA_MEM_FUNC(void, setSpeedMultiplier, float)
    OOLUA_MEM_FUNC_CONST(float, getSpeedMultiplier)
OOLUA_PROXY_END

#endif // ELYSIAN_TIMER_SYSTEM_PROXY_HPP

