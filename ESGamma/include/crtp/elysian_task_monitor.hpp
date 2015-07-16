#ifndef ELYSIAN_TASK_MONITOR_HPP
#define ELYSIAN_TASK_MONITOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_task_monitor_base.hpp>
namespace elysian {
    typedef TaskMonitorBase TaskMonitor;
}
#else
#   include <tk_task_monitor.h>
namespace elysian {
    typedef TkTaskMonitor TaskMonitor;
}
typedef TkTaskMonitor TaskMonitor;

#endif



#endif // ELYSIAN_TASK_MONITOR_HPP

