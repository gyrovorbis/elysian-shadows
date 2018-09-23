#ifndef ELYSIAN_POWER_SUPPLY_DEVICE_HPP
#define ELYSIAN_POWER_SUPPLY_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>

namespace elysian {

    class PowerSupplyStateChangedEvent: public Event {
    private:
        GY_SYSTEM_POWER_STATE   _state = GY_SYSTEM_POWER_STATE_UNKNOWN;
    public:
                                PowerSupplyStateChangedEvent(GY_SYSTEM_POWER_STATE state);
        GY_SYSTEM_POWER_STATE   getState(void) const;
    };


    class PowerSupply {
    private:
        GYSystemPowerInfo _gyPowerInfo = {
            GY_SYSTEM_POWER_STATE_UNKNOWN,
            0,
            0
        };
    public:

        bool                    pollDevice(void);

        float                   getBatteryLevel(void) const; //normalized
        unsigned                getBatteryRemainingSecs(void) const;
        GY_SYSTEM_POWER_STATE   getPowerState(void) const;
        bool                    isBattery(void) const;
    };


    //======= POWER SUPPLY INLINEZ ======
    inline PowerSupplyStateChangedEvent::PowerSupplyStateChangedEvent(GY_SYSTEM_POWER_STATE state):
        Event(EVENT_POWER_SUPPLY_STATE_CHANGED), _state(state) {}

    inline GY_SYSTEM_POWER_STATE PowerSupplyStateChangedEvent::getState(void) const {
        return _state;
    }

    inline float PowerSupply::getBatteryLevel(void) const {
        return (float)_gyPowerInfo.batteryPercentageRemaining/100.0f;
    }

    inline unsigned PowerSupply::getBatteryRemainingSecs(void) const {
        return _gyPowerInfo.batterySecondsRemaining;
    }

    inline GY_SYSTEM_POWER_STATE PowerSupply::getPowerState(void) const {
        return _gyPowerInfo.state;
    }

    inline bool PowerSupply::isBattery(void) const {
        switch(_gyPowerInfo.state) {
        case GY_SYSTEM_POWER_STATE_AC:
        case GY_SYSTEM_POWER_STATE_UNKNOWN:
            return false;
        default:
            return true;
        }
    }

    inline bool PowerSupply::pollDevice(void) {
        return gySystemPowerQueryInfo(&_gyPowerInfo);
    }



}

#endif // ELYSIAN_POWER_SUPPLY_DEVICE_HPP
