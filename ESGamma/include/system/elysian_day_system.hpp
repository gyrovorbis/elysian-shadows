#ifndef ELYSIAN_DAY_SYSTEM_HPP
#define ELYSIAN_DAY_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include <gyro_matrix_api.h>

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(LightSource)

namespace elysian {

	class LightAttrib;

	class DaySystem: public System {
	private:
		GYVector3		_axis			= GYVector3 { 0.0f, 1.0f, -1.0f };
        GYVector4		_baseDir		= GYVector4 { -1.0f, 0.0f, 0.0f, 1.0f };
//		float			_angle			= 0.0f;
		float			_time			= 0.0f;

		//time configuration
//		float			_dayDutyCycle	= 0.65f;
		float			_hoursPerDay	= 24.0f;

		//cached internal shit
		float			_secsPerHour	= 60.0f*60.0f;
		float			_secsPerDay		= _secsPerHour*_hoursPerDay;
		float			_curSecs		= 0.0f;

		//running game time
		unsigned		_days			= 0;
		unsigned		_hours			= 0;
		unsigned		_minutes		= 0;

		float			_timeSpeed		= 60.0f;

		Entity*			_sunEntity		= nullptr;
		LightSource*	_sunLightSrc	= nullptr;
		LightAttrib*	_sunLightAttrib	= nullptr;
		bool			_sunEnabled		= true;
		bool			_sunActive		= false;


	public:
						DaySystem(void);
						~DaySystem(void);

		LightAttrib*	getSunLightAttrib(void) const;
		void			setSunEnabled(const bool val);
		void			setTimeDilation(const float multiplier);
        void            setTimeOfDay(const float time);
		void			addSeconds(const float seconds);
		unsigned		getHours(void) const;
		unsigned		getMinutes(void) const;
		unsigned		getDays(void) const;

		void			update(void);
		void			onAreaEnter(void);
		void			onAreaExit(void);

	};

	//===== INLINEZ =====

	inline LightAttrib* DaySystem::getSunLightAttrib(void) const {
		return _sunLightAttrib;
	}

	inline void DaySystem::setTimeDilation(const float multiplier) {
		_timeSpeed = multiplier;
	}

	inline unsigned DaySystem::getDays(void) const { return _days; }
	inline unsigned DaySystem::getHours(void) const { return _hours; }
	inline unsigned DaySystem::getMinutes(void) const { return _minutes; }
	inline void DaySystem::addSeconds(const float seconds) { _curSecs += seconds; }

    inline void DaySystem::setTimeOfDay(const float time) {
        const float normTime = time/_hoursPerDay;

        //We have to advance a day!
        if(normTime < _time) {
            _time += (_secsPerDay-_time) + normTime;
        } else {
            _time += (normTime-_time);
        }

        //update second counter
        _curSecs = _time*_secsPerDay;
    }


}

#endif // ELYSIAN_DAY_SYSTEM_HPP
