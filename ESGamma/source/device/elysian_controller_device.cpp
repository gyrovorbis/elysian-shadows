/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/device/elysian_controller_device.hpp>

namespace elysian {

void VirtualDPad::updateState(const GYVector2 &analog) {
    bool l = false;
    bool r = false;
    bool d = false;
    bool u = false;

    if(analog.x < -ELYSIAN_CONTROLLER_ANALOG_TO_DPAD_MIN)     l = true;
    else if(analog.x > ELYSIAN_CONTROLLER_ANALOG_TO_DPAD_MIN) r = true;
    if(analog.y < -ELYSIAN_CONTROLLER_ANALOG_TO_DPAD_MIN)     u = true;
    else if(analog.y > ELYSIAN_CONTROLLER_ANALOG_TO_DPAD_MIN) d = true;

    auto _pushState = [](unsigned& state, bool pressed) {
        state <<= 1;
        state |= pressed;
        state &= 0x3;
    };

    _pushState(_dpad[_toIndex(GY_CONTROLLER_BUT_DPAD_DOWN)],    d);
    _pushState(_dpad[_toIndex(GY_CONTROLLER_BUT_DPAD_LEFT)],    l);
    _pushState(_dpad[_toIndex(GY_CONTROLLER_BUT_DPAD_RIGHT)],   r);
    _pushState(_dpad[_toIndex(GY_CONTROLLER_BUT_DPAD_UP)],      u);
}

void Controller::updateDerivedValues(void) {
    float mags[GY_CONTROLLER_ANALOG_COUNT];
    GYVector2 dirs[GY_CONTROLLER_ANALOG_COUNT];

    for(unsigned i = 0; i < GY_CONTROLLER_ANALOG_COUNT; ++i) {
        if(_gyHandle->analogsChanged[i]) {

            memcpy(&_analogDirs[i], &_gyHandle->analogSticks[i], sizeof(GYVector2));
            _analogMags[i]      = gyVec2MagAndNormalize(&_analogDirs[i]);

            if(_analogMags[i] < _analogDeadZoneMinThreshold) {         //below minimum threshold
                _analogMags[i] = 0.0f;
            } else if(_analogMags[i] > _analogDeadZoneMaxThreshold) {  //saturated above maximum threshold
                _analogMags[i] = 1.0f;
            } else {                                            //normalize to live range
                _analogMags[i] = ((_analogMags[i] - _analogDeadZoneMinThreshold)
                           / (1.0f - (_analogDeadZoneMinThreshold+(1.0f-_analogDeadZoneMaxThreshold))));
            }

            GYVector2 newAnalogVec;
            gyVec2Scale(&newAnalogVec, &_analogDirs[i], _analogMags[i]);

            if(newAnalogVec.x != _analogSticks[i].x ||
                    newAnalogVec.y != _analogSticks[i].y) {

                 memcpy(&_analogSticks[i], &newAnalogVec, sizeof(GYVector2));

                _virtualDPads[i].updateState(_analogSticks[i]);
                _gyHandle->analogsChanged[i] = 1;

            } else {
                _gyHandle->analogsChanged[i] = 0;
            }
        }

    }
}

bool Controller::pollInput(void) {
    _attached = gyControllerPoll(_port, &_gyHandle);
    //_updateDerivedValues();


    return _attached;
}

}
