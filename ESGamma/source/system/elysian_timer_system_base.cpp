/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/crtp/elysian_timer_system.hpp>

#include <libGyro/gyro_timer_api.h>
#include <cstring>
#include <cstdio>
#include <sys/time.h>
#include <ESGamma/crtp/elysian_task_monitor.hpp>

#if 0
/* copied from mpbench */
#define TIMER_CLEAR  (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0)
#define TIMER_START   gettimeofday(&tv1, (struct timezone*)0)
#define TIMER_ELAPSED ((tv2.tv_usec-tv1.tv_usec)+((tv2.tv_sec-tv1.tv_sec)*1000000))
#define TIMER_STOP    gettimeofday(&tv2, (struct timezone*)0)
struct timeval tv1,tv2;
#endif

namespace elysian {

float TimerSystemBase::ticksToSecs(uint64_t ticks) {
    return ((float)ticks/1000000000.0f);
}

//INSTANCE
void TimerSystemBase::startTimerEvent(const float duration, EventMonitor *monitor) {
    auto& proc = get<SUBPROC_TIMER_EVENT_POOL> ( static_cast<SubProcessManager<TimerSystem,
                                              TimerEventPool>& >(*this));

    auto& subProc = proc.claimIdleSubProcess();
    subProc.setDuration(duration);
    subProc.setEventMonitor(monitor);
    subProc.start(static_cast<TimerSystem*>(this));
}

TimerSystemBase::TimerSystemBase(void)
{
    gyPerfInit(nullptr);

    auto& proc = get<SUBPROC_TIMER_EVENT_POOL> ( static_cast<SubProcessManager<TimerSystem,
                                              TimerEventPool>& >(*this));

    proc.start(static_cast<TimerSystem*>(this));

	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) {
		_deltaTimeCircBuffer[i] = _realDeltaCircBuffer[i] = _minFramerate;
	}

    //TIMER_CLEAR;
    //TIMER_START;
}

TimerSystemBase::~TimerSystemBase(void) {
    gyPerfUninit();
}

void TimerSystemBase::update(void) {
    double delta;

    //TIMER_STOP;

	//MS since last frame
    _prevFrameCounter = _curFrameCounter;
    _curFrameCounter = gyPerfCounter();
    //_curFrameTicks =
	//TIMER_START;
			//gyTimerTicks();

	//update running total
	//_totalTicks += _curFrameTicks;

	//Calculate delta (sec)
	//_deltaTime = (_curFrameTicks - _prevFrameTicks)/1000.0f;
#if 1
    delta = ((double)(_curFrameCounter-_prevFrameCounter)/gyPerfFrequencyCached());//1000000000.0;
#else
  //  delta = 1.0f/60.0f;
#endif
    //TIMER_CLEAR;
    //TIMER_START;
	//return;

	_realDeltaCircBuffer[_buffIndex] = delta;

#if 0
	//Apply framerate limiting
	if(_enableMaxFramerate &&  delta < _maxFramerate) {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
//		gyTimerSleep(_maxFramerate*1000.0f - delta*1000.0f);
#endif
        delta = _maxFramerate;
	}
    else if(_enableMinFramerate && delta > _minFramerate) delta = _minFramerate;
#endif

	_deltaTimeCircBuffer[_buffIndex] = delta;
    //_deltaTime = delta;

	//_totalTicks += delta*1000.0f;

	//Calculate weighted deltatime
    _deltaTime = 0;
    _realDeltaTime = 0;
	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) {
        _deltaTime += _deltaTimeCircBuffer[i];
		_realDeltaTime += _realDeltaCircBuffer[i];
	}
    _deltaTime /= (float)DELTA_TIME_CIRC_BUFFER_SIZE;
	_realDeltaTime /= (float)DELTA_TIME_CIRC_BUFFER_SIZE;

	//Reset circular buffer when it fills up
	if(++_buffIndex >= DELTA_TIME_CIRC_BUFFER_SIZE) _buffIndex = 0; 

	_totalTime += _deltaTime;

    _deltaTime *= _speedMultiplier;

#if 1
   // _deltaTime = _deltaTime/((1.0f/60.0f));
#endif

    updateSubProcesses(_deltaTime);

    if(_topLevelTask) _topLevelTask->startNewFrame();
}


bool TimerSystemBase::pushTaskMonitor(TaskMonitor*const task) {
    bool added = true;
    if(!_currentTask) {
        _topLevelTask = _currentTask = task;
    } else if(task != _topLevelTask){
        added = _currentTask->addChild(task);
        _currentTask = task;
    }
    task->resume();
    return added;
}

void TimerSystemBase::popTaskMonitor(TaskMonitor*const task) {
    assert(_currentTask == task);
    task->pause();
    if(task == _topLevelTask) {
        //_topLevelTask = task->getParent();
    } else _currentTask = task->getParent();
}

void TimerSystemBase::printTaskStats(void) {
    if(_topLevelTask) {
        _topLevelTask->sortChildrenBySlowest();
        _topLevelTask->print();
    }
}




}

