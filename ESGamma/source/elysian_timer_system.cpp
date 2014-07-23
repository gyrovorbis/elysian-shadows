#include "elysian_timer_system.hpp"

#include <gyro.h>
#include <sys/time.h>

/* copied from mpbench */
#define TIMER_CLEAR  (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0)
#define TIMER_START   gettimeofday(&tv1, (struct timezone*)0)
#define TIMER_ELAPSED ((tv2.tv_usec-tv1.tv_usec)+((tv2.tv_sec-tv1.tv_sec)*1000000))
#define TIMER_STOP    gettimeofday(&tv2, (struct timezone*)0)
struct timeval tv1,tv2;

namespace elysian {

TimerSystem::TimerSystem(void): 
	_totalTime(0.0f),
	_totalTicks(0),
	_prevFrameTicks(0),
	_curFrameTicks(0),
	_deltaTime(0.0f),
	_enableMaxFramerate(ENABLE_MAX_FRAMERATE), 
	_enableMinFramerate(ENABLE_MIN_FRAMERATE), 
	_maxFramerate(1.0f/MAX_FRAMERATE),
	_minFramerate(1.0f/MIN_FRAMERATE), 
	_buffIndex(0) 
{

    auto& proc = get<SUBPROC_TIMER_EVENT_POOL> ( static_cast<SubProcessManager<TimerSystem,
                                              TimerEventPool>& >(*this));

    proc.start(this);

	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) {
		_deltaTimeCircBuffer[i] = _realDeltaCircBuffer[i] = _minFramerate;
	}

	TIMER_CLEAR;
	TIMER_START;
}

void TimerSystem::update(void) {
	float delta;

	TIMER_STOP;

	//MS since last frame
	_prevFrameTicks = _curFrameTicks;
	//_curFrameTicks =
	//TIMER_START;
			//gyTimerTicks();

	//update running total
	//_totalTicks += _curFrameTicks;

	//Calculate delta (sec)
	//_deltaTime = (_curFrameTicks - _prevFrameTicks)/1000.0f;
#if 0
	delta = ((float)TIMER_ELAPSED)/1000000.0f;
#else
	delta = 1/60.0f;
#endif
	TIMER_CLEAR;
	TIMER_START;
	//return;

	_realDeltaCircBuffer[_buffIndex] = delta;

    
	//Apply framerate limiting
	if(_enableMaxFramerate &&  delta < _maxFramerate) {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
		gyTimerSleep(_maxFramerate*1000.0f - delta*1000.0f);
#endif
		delta = _maxFramerate;
	}
	else if(_enableMinFramerate && delta > _minFramerate) delta = _minFramerate;


	_deltaTimeCircBuffer[_buffIndex] = delta;

	//_totalTicks += delta*1000.0f;

	//Calculate weighted deltatime
	_deltaTime = 0.0f;
	_realDeltaTime = 0.0f;
	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) {
		_deltaTime += _deltaTimeCircBuffer[i];
		_realDeltaTime += _realDeltaCircBuffer[i];
	}
	_deltaTime /= (float)DELTA_TIME_CIRC_BUFFER_SIZE;
	_realDeltaTime /= (float)DELTA_TIME_CIRC_BUFFER_SIZE;

	//Reset circular buffer when it fills up
	if(++_buffIndex >= DELTA_TIME_CIRC_BUFFER_SIZE) _buffIndex = 0; 

	_totalTime += _deltaTime;

    updateSubProcesses(_deltaTime);
}

}

