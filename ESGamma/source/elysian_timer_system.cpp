#include "elysian_timer_system.hpp"

#include <gyro.h>

namespace elysian {

TimerSystem::TimerSystem(void): 
	_totalTicks(0),
	_curFrameTicks(0), 
	_prevFrameTicks(0), 
	_deltaTime(0.0f),
	_enableMaxFramerate(ENABLE_MAX_FRAMERATE), 
	_enableMinFramerate(ENABLE_MIN_FRAMERATE), 
	_maxFramerate(1.0f/MAX_FRAMERATE), 
	_minFramerate(1.0f/MIN_FRAMERATE), 
	_buffIndex(0) 
{
	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) {
		_deltaTimeCircBuffer[i] = MAX_FRAMERATE/1000.0f;
	}

}

void TimerSystem::update(void) {
	float delta;

	//MS since last frame
	_prevFrameTicks = _curFrameTicks;
	_curFrameTicks = gyTimerTicks();

	//update running total
	_totalTicks += _curFrameTicks;

	//Calculate delta (sec)
	delta = (_curFrameTicks - _prevFrameTicks)/1000.0f;
    
	//Apply framerate limiting
	if(_enableMaxFramerate && delta < _maxFramerate) {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
		gyTimerSleep(_maxFramerate*1000.0f - _deltaTime*1000.0f);
#endif
		delta = _maxFramerate;
	}
	else if(_enableMinFramerate && delta > _minFramerate) delta = _minFramerate;
	_deltaTimeCircBuffer[_buffIndex] = delta;

	//Calculate weighted deltatime
	_deltaTime = 0.0f;
	for(unsigned i = 0; i < DELTA_TIME_CIRC_BUFFER_SIZE; ++i) _deltaTime += _deltaTimeCircBuffer[i];
	_deltaTime /= DELTA_TIME_CIRC_BUFFER_SIZE;

	//Reset circular buffer when it fills up
	if(++_buffIndex >= DELTA_TIME_CIRC_BUFFER_SIZE) _buffIndex = 0; 
}

}

