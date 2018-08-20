#ifndef ELYSIAN_PERFORMANCE_MONITOR_HPP
#define ELYSIAN_PERFORMANCE_MONITOR_HPP

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
#include <gyro_timer_api.h>
#include <gyro_system_api.h>
#include <cassert>

#define ELYSIAN_PERFORMANCE_MONITOR_TASK_NAME_SIZE 100

namespace elysian {

    struct PerformanceMonitor {

        std::vector<PerformanceMonitor*> _children;
        char        _taskName[ELYSIAN_PERFORMANCE_MONITOR_TASK_NAME_SIZE];
        int64_t    _startTicks     = 0;
        int64_t    _endTicks       = 0;
        int64_t    _deltaTicks     = 0;
        bool        _running        = false;

        PerformanceMonitor(const char* name) {
            strcpy(_taskName, name);

        }

        void start(void) {
            _deltaTicks = 0;
            resume();
        }

        void stop(void) {
            assert(_running);
            _running = false;
            _deltaTicks += gyTimerTicks()-_startTicks;
        }

        void resume(void) {
            assert(!_running);
            _running = true;
            _startTicks = gyTimerTicks();
        }

        void print(void) {
            gyPrintf("DELTA TIME - %" PRId64, _deltaTicks);
        }

        void addChild(PerformanceMonitor* monitor) {
            _children.push_back(monitor);
        }

        static void print(PerformanceMonitor* base) {

            std::function<void(PerformanceMonitor*, PerformanceMonitor*)> recurse = [&](PerformanceMonitor* current, PerformanceMonitor* parent) {
                if(parent) {
                    float percent = (float)current->_deltaTicks / (float)parent->_deltaTicks;
                    _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRId64 " : %10f", current->_taskName, current->_deltaTicks, percent);
                } else {
                    _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRId64, current->_taskName, current->_deltaTicks);
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



                   int64_t totalChildTicks = 0;
                    _gyPush();
                    for(unsigned i = 0; i < current->_children.size(); ++i) {
                        recurse(current->_children[i], current);
                        totalChildTicks += current->_children[i]->_deltaTicks;

                    }

                    int64_t remainingTicks = current->_deltaTicks - totalChildTicks;
                    float percent = (float)remainingTicks / (float)current->_deltaTicks;
                    if(remainingTicks > 0) {
                        _gyLog(GY_DEBUG_VERBOSE, "%-40s : %20" PRId64 " : %10f", "other", remainingTicks, percent);
                    }
                    _gyPop(1);
                }

            };

            recurse(base, nullptr);

        }

    };

}

#endif // ELYSIAN_PERFORMANCE_MONITOR_HPP
