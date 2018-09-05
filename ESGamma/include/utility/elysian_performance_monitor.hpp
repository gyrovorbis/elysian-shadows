#ifndef ELYSIAN_PERFORMANCE_MONITOR_HPP
#define ELYSIAN_PERFORMANCE_MONITOR_HPP

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <gyro_performance_api.h>
#include <gyro_system_api.h>
#include <cassert>

#define ELYSIAN_PERFORMANCE_MONITOR_TASK_NAME_SIZE  100
#define ELYSIAN_PERFORMANCE_MONITOR_AVG_FRAMES      10

namespace elysian {

    struct PerformanceMonitor {
        std::vector<PerformanceMonitor*>
                    _children;
        uint64_t    _avgTicks[ELYSIAN_PERFORMANCE_MONITOR_AVG_FRAMES] = { 0 };
        char        _taskName[ELYSIAN_PERFORMANCE_MONITOR_TASK_NAME_SIZE];
        uint64_t    _startTicks     = 0;
        uint64_t    _endTicks       = 0;
        uint64_t    _deltaTicks     = 0;
        uint64_t    _worstTicks     = 0;
        uint64_t    _bestTicks      = 0;
        uint64_t    _frameCount     = 0;
        bool        _running        = false;

                    PerformanceMonitor(const char* name);

        uint64_t     getWorstTicks(void) const;
        uint64_t     getBestTicks(void) const;
        uint64_t    getAvgTicks(void) const;
        uint64_t    getFrameCount(void) const;
        uint64_t    getTicks(void) const;

        void        start(void);
        void        stop(void);
        void        resume(void);
        void        setTicks(uint64_t ticks);

        void        print(void);
        void        addChild(PerformanceMonitor* monitor);

        static void print(PerformanceMonitor* base);

    };


    //======== INLINEZ ===========
    inline PerformanceMonitor::PerformanceMonitor(const char* name) {
        strcpy(_taskName, name);
    }
    inline uint64_t PerformanceMonitor::getFrameCount(void) const { return _frameCount; }
    inline uint64_t PerformanceMonitor::getWorstTicks(void) const { return _worstTicks; }
    inline uint64_t PerformanceMonitor::getBestTicks(void) const { return _bestTicks; }
    inline uint64_t PerformanceMonitor::getTicks(void) const { return _deltaTicks; }
    inline uint64_t PerformanceMonitor::getAvgTicks(void) const {
        if(!_frameCount) return 0;
        int count = (_frameCount > ELYSIAN_PERFORMANCE_MONITOR_AVG_FRAMES)? ELYSIAN_PERFORMANCE_MONITOR_AVG_FRAMES : (int)_frameCount;
        uint64_t avg = 0;
        for(int i = 0; i < count; ++i) avg += _avgTicks[i];
        avg /= (unsigned)count;
        return avg;
    }

    inline void PerformanceMonitor::setTicks(uint64_t ticks) {
        _deltaTicks = ticks;
        if(_frameCount) {
            if(_deltaTicks < _bestTicks) _bestTicks = _deltaTicks;
            if(_deltaTicks > _worstTicks) _worstTicks = _deltaTicks;
        } else {
            _bestTicks = _worstTicks = _deltaTicks;
        }
        _avgTicks[_frameCount%ELYSIAN_PERFORMANCE_MONITOR_AVG_FRAMES] = _deltaTicks;
        ++_frameCount;
    }

    inline void PerformanceMonitor::start(void) {
       setTicks(_deltaTicks);
        _deltaTicks = 0;
        resume();
    }

    inline void PerformanceMonitor::stop(void) {
        assert(_running);
        _running = false;
        _deltaTicks += ((gyPerfCounter()-_startTicks)/((double)gyPerfFrequencyCached()))*1000000000.0;
    }

    inline void PerformanceMonitor::resume(void) {
        assert(!_running);
        _running = true;
        _startTicks = gyPerfCounter();
    }

    inline void PerformanceMonitor::print(void) {
        gyPrintf("DELTA TIME - %" PRIu64, _deltaTicks);
    }

    inline void PerformanceMonitor::addChild(PerformanceMonitor* monitor) {
        _children.push_back(monitor);
    }

    inline void PerformanceMonitor::print(PerformanceMonitor* base) {

        std::function<void(PerformanceMonitor*, PerformanceMonitor*)> recurse = [&](PerformanceMonitor* current, PerformanceMonitor* parent) {
            if(parent) {
                double percent = (parent->_deltaTicks != 0)? (double)current->_deltaTicks / (double)parent->_deltaTicks : 0.0;
                _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRIu64 " ns : %10lf", current->_taskName, current->_deltaTicks, percent);
            } else {
                _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRIu64 " ns", current->_taskName, current->_deltaTicks);
            }

            //Bubble-sort children shittiest to best deltatime
            if(current->_children.size()) {
                for(unsigned i = 0; i < current->_children.size()-1; ++i)
                    for(unsigned j = i+1; j < current->_children.size(); ++j)
                        if(current->_children[j]->_deltaTicks > current->_children[i]->_deltaTicks) {
                            auto* temp = current->_children[i];
                            current->_children[i] = current->_children[j];
                            current->_children[j] = temp;
                        }



               uint64_t totalChildTicks = 0;
                _gyPush();
                for(unsigned i = 0; i < current->_children.size(); ++i) {
                    recurse(current->_children[i], current);
                    totalChildTicks += current->_children[i]->_deltaTicks;
                }

                if(current->_deltaTicks > totalChildTicks) {
                    uint64_t remainingTicks = current->_deltaTicks - totalChildTicks;
                    double percent = (current->_deltaTicks != 0)? (double)remainingTicks / (double)current->_deltaTicks : 0.0;
                    if(remainingTicks > 0) {
                        _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRIu64 " ns : %10lf", "other", remainingTicks, percent);
                    }
                    _gyPop(1);
                }
            }

        };

        recurse(base, nullptr);

    }



}

#endif // ELYSIAN_PERFORMANCE_MONITOR_HPP
