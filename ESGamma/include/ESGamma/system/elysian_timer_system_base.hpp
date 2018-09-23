#ifndef ELYSIAN_TIMER_SYSTEM_BASE_HPP
#define ELYSIAN_TIMER_SYSTEM_BASE_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/utility/elysian_subprocess_manager.hpp>
#include <libGyro/gyro_timer_api.h>

#define DELTA_TIME_CIRC_BUFFER_SIZE                 20
#define MAX_FRAMERATE                               60.0f
#define MIN_FRAMERATE                               30.0f
#define ENABLE_MAX_FRAMERATE                        true
#define ENABLE_MIN_FRAMERATE                        true
#define ELYSIAN_TIMER_SYSTEM_TIMER_EVENT_POOL_SIZE  50
#define ELYSIAN_PERFORMANCE_MONITORING              true

ELYSIAN_ASSET_TYPEDEF(TaskMonitor)
ELYSIAN_ASSET_TYPEDEF(TimerSystem)

namespace elysian {

    class TimerSystemBase;

    class TimerEventSubprocess:
            public MonitorableSubProcess<TimerEventSubprocess, TimerSystem>
    {
    private:
        float   _elapsedTime  = 0.0f;
        float   _duration     = 0.0f;

    public:
        void    _start(TimerSystem* sys);
        void    _stop(TimerSystem* sys);
        void    _update(TimerSystem* sys, const float deltaTime);

        float   getElapsedTime(void) const;
        void    setDuration(const float time);
    };




    typedef MonitorableSubProcessPool<TimerEventSubprocess,
            TimerSystem,
            ELYSIAN_TIMER_SYSTEM_TIMER_EVENT_POOL_SIZE>
        TimerEventPool;

    class TimerSystemBase:
            public SubProcessManager<TimerSystem, TimerEventPool>,
            public System {
    protected:
        enum SUBPROCS { SUBPROC_TIMER_EVENT_POOL };

        int64_t         _totalTicks				= 0; //play-time essentially
        uint64_t        _prevFrameCounter		= 0;
        uint64_t        _curFrameCounter		= 0;
		float           _deltaTime				= 0.0f;
		float           _realDeltaTime			= 0.0f;
        float           _speedMultiplier        = 1.0f;
		double          _totalTime				= 0.0f;
		bool            _enableMaxFramerate		= ENABLE_MAX_FRAMERATE;
		bool            _enableMinFramerate		= ENABLE_MIN_FRAMERATE;
		float           _maxFramerate			= 1.0f/MAX_FRAMERATE;
		float           _minFramerate			= 1.0f/MIN_FRAMERATE;

        float           _deltaTimeCircBuffer[DELTA_TIME_CIRC_BUFFER_SIZE];
        float           _realDeltaCircBuffer[DELTA_TIME_CIRC_BUFFER_SIZE];
		unsigned        _buffIndex				= 0;

        TaskMonitor*    _topLevelTask           = nullptr;
        TaskMonitor*    _currentTask            = nullptr;

	public:
                        TimerSystemBase(void);
                        ~TimerSystemBase(void);
        void            update(void);


        unsigned long   getTotalTicks(void) const;
        unsigned long   getPreviousFrameTicks(void) const;
        unsigned long   getCurrentFrameTicks(void) const;
        float           getCurrentFrameTime(void) const;
        double          getTotalTime(void) const;
        float           getDeltaTime(void) const;
        float           getFramerate(void) const;
        float           getSpeedMultiplier(void) const;
        void            setSpeedMultiplier(float mult);

        void            startTimerEvent(const float duration, EventMonitor* monitor);


        TaskMonitor*    getTopLevelTask(void) const;
        bool            pushTaskMonitor(TaskMonitor*const task);
        void            popTaskMonitor(TaskMonitor*const task);
        void            printTaskStats(void);


        static float    ticksToSecs(uint64_t ticks);

	};




	//===== INLINEZ ======

    inline TaskMonitor* TimerSystemBase::getTopLevelTask(void) const { return _topLevelTask; }
    inline unsigned long int TimerSystemBase::getTotalTicks(void) const { return _totalTicks; }
    inline double TimerSystemBase::getTotalTime(void) const { return _totalTime; }
    inline unsigned long int TimerSystemBase::getPreviousFrameTicks(void) const {
        return _prevFrameCounter;
	}
    inline unsigned long int TimerSystemBase::getCurrentFrameTicks(void) const {
        return _curFrameCounter;
    }
    inline float TimerSystemBase::getCurrentFrameTime(void) const {
        return (float)((gyPerfCounter()-_prevFrameCounter)/gyPerfFrequencyCached())/10000000000.0f;
    }
    inline float TimerSystemBase::getDeltaTime(void) const {
		return _deltaTime;
	}
    inline float TimerSystemBase::getFramerate(void) const { return 1.0f/_realDeltaTime; }



    inline void TimerEventSubprocess::_start(TimerSystem* /*sys*/) { _elapsedTime = 0.0f; }
    inline void TimerEventSubprocess::_stop(TimerSystem* /*sys*/) {}
    inline void TimerEventSubprocess::_update(TimerSystem* sys, const float deltaTime) {
        _elapsedTime += deltaTime;
        if(_elapsedTime > _duration) stop(sys);
    }
    inline float TimerEventSubprocess::getElapsedTime(void) const { return _elapsedTime; }
    inline void TimerEventSubprocess::setDuration(const float time) { _duration = time; }

    inline float TimerSystemBase::getSpeedMultiplier(void) const { return _speedMultiplier; }
    inline void  TimerSystemBase::setSpeedMultiplier(float mult) { _speedMultiplier = mult; }
	
}

#endif
