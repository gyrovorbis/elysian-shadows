#include <system/elysian_input_system.hpp>
#include <crtp/elysian_debug.hpp>

namespace elysian {



bool Keyboard::pollInput(void) {
    _attached = gyKeyboardPoll(&_gyHandle);
    return _attached;
}

bool Mouse::pollInput(void) {
    _attached = gyMousePoll(&_gyHandle);
    return _attached;
}

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

void Controller::_updateDerivedValues(void) {
    for(unsigned i = 0; i < GY_CONTROLLER_ANALOG_COUNT; ++i) {
        _analogDirs[i]      = _gyHandle->analogSticks[i];
        _analogMags[i]      = gyVec2MagAndNormalize(&_analogDirs[i]);
        _analogAngles[i]    = atan2(_analogDirs[i].y, _analogDirs[i].x);
        _virtualDPads[i].updateState(_gyHandle->analogSticks[i]);
    }
}

bool Controller::pollInput(void) {
    _attached = gyControllerPoll(_port, &_gyHandle);
    _updateDerivedValues();
    return _attached;
}


bool InputSystem::_initialized = false;
bool InputSystem::_managedExternally = false;
const constexpr GYInputInitParams InputSystem::_defaultInitParams;

InputSystem::InputSystem(void) {
    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) _controllers[i].setPort(i);
}

bool InputSystem::initialize(const GYInputInitParams*const params) {
    if(_initialized) return true;

    bool success = true;

    success &= gyInputInit(params? params : &_defaultInitParams);


    if(params != &_defaultInitParams) _managedExternally = true;

    _initialized = true;
    return success;
}

bool InputSystem::uninitialize(void) {
    if(!_initialized || _managedExternally) return true;

    bool success = true;

    success &= gyInputUninit();

    _initialized = false;
    return success;
}


void InputSystem::_onControllerAttached(const unsigned slot) {
    dbgprintf("InputSystem: Controller Attached! [Port: %d]", slot);
}

void InputSystem::_onControllerRemoved(const unsigned slot) {
    dbgprintf("InputSystem: Controller Removed! [Port: %d]", slot);
}

void InputSystem::_onMouseAttached(void) {
    dbgprintf("InputSystem: Mouse Attached!");
}

void InputSystem::_onMouseRemoved(void) {
    dbgprintf("InputSystem: Mouse Removed!");
}

void InputSystem::_onKeyboardAttached(void) {
    dbgprintf("InputSystem: Keyboard Attached!");
}

void InputSystem::_onKeyboardRemoved(void) {
    dbgprintf("InputSystem: Keyboard Removed!");
}

void InputSystem::dumpStats(void) {
    gyInputDeviceDump();
}

void InputSystem::update(void) {

    //update keyboard
    bool wasAttached   = _keyboard.isAttached();
    bool isAttached    = _keyboard.pollInput();

    if(isAttached && !wasAttached)      _onKeyboardAttached();
    else if(!isAttached && wasAttached) _onKeyboardRemoved();

    //update mouse
 //   wasAttached = _mouse.isAttached();
 //   isAttached = _mouse.pollInput();

   // if(isAttached && !wasAttached)      _onMouseAttached();
   // else if(!isAttached && wasAttached) _onMouseRemoved();

    //update controllers
    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) {
        const auto wasAttached  = _controllers[i].isAttached();
        const auto isAttached   = _controllers[i].pollInput();

        if(isAttached && !wasAttached)      _onControllerAttached(i);
        else if(!isAttached && wasAttached) _onControllerRemoved(i);
    }
    
    
}

}
