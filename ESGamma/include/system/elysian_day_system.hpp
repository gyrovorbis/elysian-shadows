#ifndef ELYSIAN_DAY_SYSTEM_HPP
#define ELYSIAN_DAY_SYSTEM_HPP

#include <system/elysian_system.hpp>
#include <system/elysian_event_system.hpp>
#include <gyro_matrix_api.h>
#include <memory>

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(LightSource)

#define ELYSIAN_DAY_SYSTEM_HOUR_DAWN        6
#define ELYSIAN_DAY_SYSTEM_HOUR_NOON        12
#define ELYSIAN_DAY_SYSTEM_HOUR_DUSK        18
#define ELYSIAN_DAY_SYSTEM_HOUR_MIDNIGHT    24

namespace elysian {

	class LightAttrib;

    class DaySystem: public System, public EventObserver {
    public:
        enum TIME_HOUR {
            TIME_HOUR_DAWN       = 6,
            TIME_HOUR_NOON       = 12,
            TIME_HOUR_DUSK       = 18,
            TIME_HOUR_MIDNIGHT   = 24
        };

        enum TIME_INTERVAL {
            TIME_INTERVAL_DAY_AM,     //6-12
            TIME_INTERVAL_DAY_PM,     //12-18
            TIME_INTERVAL_NIGHT_PM,   //18-24
            TIME_INTERVAL_NIGHT_AM,   //24-6
        };

	private:
        GYVector3                       _axis			= GYVector3 { 0.0f, 1.0f, -1.0f };
        GYVector4                       _baseDir		= GYVector4 { -1.0f, 0.0f, 0.0f, 1.0f };
//		float                           _angle			= 0.0f;
        float                           _time			= 0.0f;

		//time configuration
//		float                           _dayDutyCycle	= 0.65f;
        float                           _hoursPerDay	= 24.0f;

		//cached internal shit
        float                           _secsPerHour	= 60.0f*60.0f;
        float                           _secsPerDay		= _secsPerHour*_hoursPerDay;
        float                           _curSecs		= 0.0f;

		//running game time
        unsigned                        _days			= 0;
        unsigned                        _hours			= 0;
        unsigned                        _minutes		= 0;

        float                           _timeSpeed		= 60.0f;

        Entity*                         _sunEntity		= nullptr;
        LightSource*                    _sunLightSrc	= nullptr;
        std::shared_ptr<LightAttrib>	_sunLightAttrib	= nullptr;
//		bool                            _sunEnabled		= true;
        bool                            _sunActive		= false;
        TIME_INTERVAL                   _timeInterval   = TIME_INTERVAL_NIGHT_PM;

        void            _updateTimeInterval(void);


	public:
						DaySystem(void);
						~DaySystem(void);

        auto            getSunLightAttrib(void) const -> std::shared_ptr<LightAttrib>;
        //void			setSunEnabled(const bool val);
		void			setTimeDilation(const float multiplier);
        void            setTimeOfDay(const float time);
		void			addSeconds(const float seconds);
		unsigned		getHours(void) const;
		unsigned		getMinutes(void) const;
		unsigned		getDays(void) const;
        bool            isDay(void) const;
        TIME_INTERVAL   getTimeInterval(void) const;

		void			update(void);

        virtual void    onObserveEvent(Event* event, EventTarget* target) override;

        void			onAreaEnter(void);
        void			onAreaExit(void);

	};







	//===== INLINEZ =====

    inline std::shared_ptr<LightAttrib> DaySystem::getSunLightAttrib(void) const {
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

    inline bool DaySystem::isDay(void) const { return (_timeInterval == TIME_INTERVAL_DAY_AM || _timeInterval == TIME_INTERVAL_DAY_PM); }
    inline auto DaySystem::getTimeInterval(void) const -> TIME_INTERVAL { return _timeInterval; }


}

#endif // ELYSIAN_DAY_SYSTEM_HPP
