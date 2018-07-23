#ifndef ELYSIAN_TIMER_MONITOR_HPP
#define ELYSIAN_TIMER_MONITOR_HPP

#define PERFORMANCE_MONITOR_CIRC_BUFFER_SIZE        20
#define PERFORMANCE_MONITOR_MAX_CHILDREN            20
#define PERFORMANCE_MONITOR_NAME_SIZE               50

#include <utility/elysian_mem_pool.hpp>
#include <gyro_performance_api.h>
#include <utility/elysian_asset_typedefs.hpp>
#include <vector>

ELYSIAN_ASSET_TYPEDEF(TaskMonitor);

namespace elysian {

    class TaskMonitorBase {
        int64_t             _tickBuffer [PERFORMANCE_MONITOR_CIRC_BUFFER_SIZE]  = { 0 };
        TaskMonitor*        _children[PERFORMANCE_MONITOR_MAX_CHILDREN];
        char                _name       [PERFORMANCE_MONITOR_NAME_SIZE];
        double              _avgTime    = 0.0;
        int64_t             _startTicks = 0;
        int                 _buffDepth  = 0;
        TaskMonitor*        _parent     = nullptr;
        unsigned            _curIndex   = 0;
        bool                _dirty      = true;
    public:

                            TaskMonitorBase(const char*const name);

        const char*         getName(void) const;
        TaskMonitor*        getParent(void) const;

        void                print(void) const;
        void                startNewFrame(void);
        void                resume(void);
        void                pause(void);

        double              getAvgTime(void) const;
        double              getPercentTotalTime(void) const;
        double              getPercentParentTime(void) const;

        bool                addChild(TaskMonitor*const child);
        unsigned            getChildCount(void) const;
        TaskMonitor*        getChild(const unsigned index) const;
        TaskMonitor*        getChild(const char*const name);
        void                clearChildren(void);
        void                sortChildrenBySlowest(void);

    };











    //==== INLINE ======

    inline TaskMonitorBase::TaskMonitorBase(const char*const name) {
        strcpy(_name, name);
    }

    inline void TaskMonitorBase::resume(void) {
        _startTicks = gyPerfCounter();
    }

    inline void TaskMonitorBase::pause(void) {
        _tickBuffer[_curIndex] += (double)((gyPerfCounter()-_startTicks)*1000.0f)/gyPerfFrequencyCached();
    }

    inline const char* TaskMonitorBase::getName(void) const { return _name; }
    inline double TaskMonitorBase::getAvgTime(void) const { return _avgTime; }

    inline TaskMonitor* TaskMonitorBase::getParent(void) const {
        return _parent;
    }

}

#endif // ELYSIAN_TIMER_MONITOR_HPP

