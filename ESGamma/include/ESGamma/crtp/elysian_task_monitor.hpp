/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_TASK_MONITOR_HPP
#define ELYSIAN_TASK_MONITOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_task_monitor_base.hpp>
namespace elysian {
    typedef TaskMonitorBase TaskMonitor;
}
#else
#   include <ESToolkit/ESGamma/tk_task_monitor.h>
namespace elysian {
    typedef TkTaskMonitor TaskMonitor;
}
typedef TkTaskMonitor TaskMonitor;

#endif



#endif // ELYSIAN_TASK_MONITOR_HPP

