#ifndef ELYSIAN_TOUCH_DEVICE_HPP
#define ELYSIAN_TOUCH_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <libGyro/gyro_input_api.h>

namespace elysian {

    class TouchDevice: public PlayerIODevice {
    public:

        bool pollInput(void) const;
    };



    //====TOUCHDEVICE INLINEZ=======
    inline bool TouchDevice::pollInput(void) const { return false; }

}

#endif // ELYSIAN_TOUCH_DEVICE_HPP
