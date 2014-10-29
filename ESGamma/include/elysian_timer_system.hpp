#ifndef ELYSIAN_TIMER_SYSTEM_HPP
#define ELYSIAN_TIMER_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_subprocess_manager.hpp"

#define DELTA_TIME_CIRC_BUFFER_SIZE                 20
#define MAX_FRAMERATE                               60.0f
#define MIN_FRAMERATE                               30.0f
#define ENABLE_MAX_FRAMERATE                        true
#define ENABLE_MIN_FRAMERATE                        true
#define ELYSIAN_TIMER_SYSTEM_TIMER_EVENT_POOL_SIZE  50

namespace elysian {
/*
	struct TimerFrameProfileTask {
		char name[100];
		float startTime;
		float endTime;
		float totalTime;
	};
*/
    class TimerSystem;

    class TimerEventSubprocess:
            public MonitorableSubProcess<TimerEventSubprocess, TimerSystem>
    {
    private:
        float _elapsedTime  = 0.0f;
        float _duration     = 0.0f;

    public:
        void _start(TimerSystem* sys);
        void _stop(TimerSystem* sys);
        void _update(TimerSystem* sys, const float deltaTime);

        float getElapsedTime(void) const;
        void setDuration(const float time);
    };

    typedef MonitorableSubProcessPool<TimerEventSubprocess,
            TimerSystem,
            ELYSIAN_TIMER_SYSTEM_TIMER_EVENT_POOL_SIZE>
        TimerEventPool;

    class TimerSystem:
            public SubProcessManager<TimerSystem, TimerEventPool>,
            public System {
	private:
        enum SUBPROCS { SUBPROC_TIMER_EVENT_POOL };

        unsigned long   _totalTicks; //play-time, essentially
        unsigned long   _prevFrameTicks;
        unsigned long   _curFrameTicks;
        float           _deltaTime;
        float           _realDeltaTime;
        double          _totalTime;
        bool            _enableMaxFramerate;
        bool            _enableMinFramerate;
        float           _maxFramerate;
        float           _minFramerate;

        float           _deltaTimeCircBuffer[DELTA_TIME_CIRC_BUFFER_SIZE];
        float           _realDeltaCircBuffer[DELTA_TIME_CIRC_BUFFER_SIZE];
        unsigned        _buffIndex;

	public:
                        TimerSystem(void);
        void            update(void);

        unsigned long   getTotalTicks(void) const;
        unsigned long   getPreviousFrameTicks(void) const;
        unsigned long   getCurrentFrameTicks(void) const;
        double          getTotalTime(void) const;
        float           getDeltaTime(void) const;
        float           getFramerate(void) const;

        void            startTimerEvent(const float duration, EventMonitor* monitor);

	};




	//===== INLINEZ ======

	inline unsigned long int TimerSystem::getTotalTicks(void) const { return _totalTicks; }
	inline double TimerSystem::getTotalTime(void) const { return _totalTime; }
	inline unsigned long int TimerSystem::getPreviousFrameTicks(void) const {
		return _prevFrameTicks;
	}
	inline unsigned long int TimerSystem::getCurrentFrameTicks(void) const {
		return _curFrameTicks;
	}
	inline float TimerSystem::getDeltaTime(void) const {
		return _deltaTime;
	}
	inline float TimerSystem::getFramerate(void) const { return 1.0f/_realDeltaTime; }
    inline void TimerSystem::startTimerEvent(const float duration, EventMonitor *monitor) {
        auto& proc = get<SUBPROC_TIMER_EVENT_POOL> ( static_cast<SubProcessManager<TimerSystem,
                                                  TimerEventPool>& >(*this));

        auto& subProc = proc.claimIdleSubProcess();
        subProc.setDuration(duration);
        subProc.setEventMonitor(monitor);
        subProc.start(this);
    }


    inline void TimerEventSubprocess::_start(TimerSystem* /*sys*/) { _elapsedTime = 0.0f; }
    inline void TimerEventSubprocess::_stop(TimerSystem* /*sys*/) {}
    inline void TimerEventSubprocess::_update(TimerSystem* sys, const float deltaTime) {
        _elapsedTime += deltaTime;
        if(_elapsedTime > _duration) stop(sys);
    }
    inline float TimerEventSubprocess::getElapsedTime(void) const { return _elapsedTime; }
    inline void TimerEventSubprocess::setDuration(const float time) { _duration = time; }
	
}

#endif
