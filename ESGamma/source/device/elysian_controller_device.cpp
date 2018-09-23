#include <ESGamma/device/elysian_controller_device.hpp>

namespace elysian {

void VirtualDPad::updateState(const GYVector2 &analog) {
    bool l = false;
    bool r = false;
    bool d = false;
    bool u = false;

    if(analog.x < -ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN)     l = true;
    else if(analog.x > ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN) r = true;
    if(analog.y < -ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN)     u = true;
    else if(analog.y > ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN) d = true;

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
    for(unsigned i = 0; i < GY_CONTROLLER_ANALOG_COUNT; ++i) {
        _analogDirs[i]      = _gyHandle->analogSticks[i];
        _analogMags[i]      = gyVec2MagAndNormalize(&_analogDirs[i]);
        //_analogAngles[i]    = atan2(_analogDirs[i].y, _analogDirs[i].x);
        _virtualDPads[i].updateState(_gyHandle->analogSticks[i]);
    }
}

bool Controller::pollInput(void) {
    _attached = gyControllerPoll(_port, &_gyHandle);
    //_updateDerivedValues();
    return _attached;
}

}
