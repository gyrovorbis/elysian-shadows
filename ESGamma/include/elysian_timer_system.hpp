#ifndef ELYSIAN_TIMER_SYSTEM_HPP
#define ELYSIAN_TIMER_SYSTEM_HPP

#include "elysian_system.hpp"

#define DELTA_TIME_CIRC_BUFFER_SIZE 20
#define MAX_FRAMERATE 60
#define MIN_FRAMERATE 30

#define ENABLE_MAX_FRAMERATE true
#define ENABLE_MIN_FRAMERATE true

namespace elysian {
/*
	struct TimerFrameProfileTask {
		char name[100];
		float startTime;
		float endTime;
		float totalTime;
	};
*/

	class TimerSystem: public System { 
	private:
		unsigned long int _totalTicks; //play-time, essentially
		unsigned long int _prevFrameTicks;
		unsigned long int _curFrameTicks;
		float _deltaTime;
		double _totalTime;
        bool _enableMaxFramerate, _enableMinFramerate;
		float _maxFramerate, _minFramerate;

		float _deltaTimeCircBuffer[DELTA_TIME_CIRC_BUFFER_SIZE];
		unsigned _buffIndex;

	public:
		TimerSystem(void);
		void update(void);

		unsigned long int getTotalTicks(void) const;
		unsigned long int getPreviousFrameTicks(void) const;
		unsigned long int getCurrentFrameTicks(void) const;
		double getTotalTime(void) const;
		float getDeltaTime(void) const;
		float getFramerate(void) const;
       // bool getLimitFramerate(void) const { return _limitFramerate; }
       // void setLimitFramerate(const bool val=true) { _limitFramerate = val; }
        
        //getFramerate();
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

	inline float TimerSystem::getFramerate(void) const { return 1.0f/_deltaTime; }
	
}

#endif
