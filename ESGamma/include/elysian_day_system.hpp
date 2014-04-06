#ifndef ELYSIAN_DAY_SYSTEM_HPP
#define ELYSIAN_DAY_SYSTEM_HPP

#include "elysian_system.hpp"

namespace elysian {

	class Entity;
	class LightSource;
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

		float			_timeSpeed		= 600.0f;

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


}

#endif // ELYSIAN_DAY_SYSTEM_HPP
