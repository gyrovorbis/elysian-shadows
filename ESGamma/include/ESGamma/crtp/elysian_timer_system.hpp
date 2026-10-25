/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_TIMER_SYSTEM_HPP
#define ELYSIAN_TIMER_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <elysian_timer_system_base.hpp>
namespace elysian {
    typedef TimerSystemBase TimerSystem;
}
#else
#   include <ESToolkit/ESGamma/tk_timer_system.h>
namespace elysian {
    typedef TkTimerSystem TimerSystem;
}
typedef TimerSystem TimerSystem;

#endif


#endif // ELYSIAN_TIMER_SYSTEM_HPP

