#ifndef ELYSIAN_TIMER_SYSTEM_HPP
#define ELYSIAN_TIMER_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <elysian_timer_system_base.hpp>
namespace elysian {
    typedef TimerSystemBase TimerSystem;
}
#else
#   include <tk_timer_system.h>
namespace elysian {
    typedef TkTimerSystem TimerSystem;
}
typedef TimerSystem TimerSystem;

#endif


#endif // ELYSIAN_TIMER_SYSTEM_HPP

