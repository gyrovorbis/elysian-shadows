#include <crtp/elysian_input_system.hpp>
#include <crtp/elysian_debug.hpp>
#include <system/elysian_event_system.hpp>
#include <gyro_file_api.h>

namespace elysian {

KeyboardEventObserver::KeyboardEventObserver(void) {
    System::getEventSys()->addEventObserver(this, EVENT_KEYBOARD_KEY);
}

void KeyboardEventObserver::onObserveEvent(Event* event, EventTarget* target) {
    if(!target && event->getEventType() == EVENT_KEYBOARD_KEY) {
        onKeyboardEvent(*System::getInputSys()->getKeyboard());
    }
}

void KeyboardEventObserver::onKeyboardEvent(const Keyboard&) {

}

bool Keyboard::pollInput(void) {
    _attached = gyKeyboardPoll(&_gyHandle);
    if(_attached && _gyHandle->changed) System::getEventSys()->broadcastEvent(Event(EVENT_KEYBOARD_KEY));
    return _attached;
}

bool Mouse::pollInput(void) {
    _attached = gyMousePoll(&_gyHandle);
    if(_attached && _gyHandle) {
        if(_gyHandle->buttonsChanged)       System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_BUTTON, this));
        if(_gyHandle->screenPosChanged)     System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_MOVE, this));
        if(_gyHandle->scrollWheelChanged)   System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_WHEEL, this));
    }
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

void Controller::updateDerivedValues(void) {
    for(unsigned i = 0; i < GY_CONTROLLER_ANALOG_COUNT; ++i) {
        _analogDirs[i]      = _gyHandle->analogSticks[i];
        _analogMags[i]      = gyVec2MagAndNormalize(&_analogDirs[i]);
        _analogAngles[i]    = atan2(_analogDirs[i].y, _analogDirs[i].x);
        _virtualDPads[i].updateState(_gyHandle->analogSticks[i]);
    }
}

bool Controller::pollInput(void) {
    _attached = gyControllerPoll(_port, &_gyHandle);
    //_updateDerivedValues();
    return _attached;
}

bool VisualMemoryUnit::readBlock(uint16_t blockNum, uint8_t* buffer) const {
    return _gyHandle? gyVisualMemoryDeviceBlockRead(_gyHandle, buffer, blockNum) : false;
}

bool VisualMemoryUnit::writeBlock(uint16_t blockNum, const uint8_t* buffer) {
    return _gyHandle? gyVisualMemoryDeviceBlockWrite(_gyHandle, buffer, blockNum) : false;
}
bool VisualMemoryUnit::beep(uint8_t pulseLength, uint8_t dutyCycle) {
    return _gyHandle? gyVisualMemoryDeviceBeep(_gyHandle, pulseLength, dutyCycle) : false;
}
bool VisualMemoryUnit::loadIcon(const char* filePath) {
    GYFile *fp;
    bool success = true;
    dbgprintf("VisualMemoryUnit[%d]::loadIcon(%s)", getPlayerIndex(), filePath);
    dbgpush();

    if (!gyFileOpen(filePath, "rb", &fp)) {
        Debug::printf(Debug::LOG_ERROR, "FAILURE!");
        success = false;
    } else {
        size_t bytesRead;
        gyFileRead(fp, _frameBuffer, 192, &bytesRead);
        if(bytesRead != 192) {
            errprintf("Could only read %d of %d bytes from file!", bytesRead, 192);
            success = false;
        }
        if(bytesRead > 0) _fbChanged = true;
    }
    dbgpop();
    return success;
}

bool VisualMemoryUnit::pollDevice(void) {
    _attached = gyVisualMemoryDevicePoll(_playerIndex, &_gyHandle);
    if(_attached && _gyHandle) {
        if(_fbChanged) {
            gyVisualMemoryDeviceDisplayImage(_gyHandle, (uint8_t*)_frameBuffer);
            _fbChanged = false;
        }
    }
    return _attached;
}

bool VisualMemoryUnit::connectToEVMU(const char* hostName, unsigned port) {
    _tcpSettings = (GYVisualMemoryTCPSettings*)malloc(sizeof(GYVisualMemoryTCPSettings));
    strcpy(_tcpSettings->hostname, hostName);
    _tcpSettings->port = port;
    return gyVisualMemoryDeviceRemoteConnect(_playerIndex, _tcpSettings);
}

bool VisualMemoryUnit::disconnectFromEVMU(void) {
    if(_gyHandle) gyVisualMemoryDeviceRemoteDisconnect(_gyHandle);
    free(_tcpSettings);
    _tcpSettings = nullptr;
}



bool InputSystemBase::_initialized = false;
bool InputSystemBase::_managedExternally = false;
const constexpr GYInputInitParams InputSystemBase::_defaultInitParams;

InputSystemBase::InputSystemBase(void) {
    GYVisualMemoryInitParams vmuInitParams = {
        4
    };
    gyVisualMemoryInit(&vmuInitParams);

    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) {
        _controllers[i].setPort(i);
        _controllers[i].setPlayerIndex(i);
    }
    _keyboard.setPlayerIndex(0);
    _mouse.setPlayerIndex(0);
    for(unsigned i = 0; i < ELYSIAN_INPUT_VMUS_MAX; ++i) {
        _vmus[i].setPlayerIndex(i);
    }
    //Initial input poll, to initialize controller/mouse/keyboard handles
    update();
}

InputSystemBase::~InputSystemBase(void) {
    gyVisualMemoryUninit();
}

bool InputSystemBase::initialize(const GYInputInitParams*const params) {
    bool success = true;

    if(!_initialized) {
        success &= gyInputInit(params? params : &_defaultInitParams);
        if(params != &_defaultInitParams) _managedExternally = true;
        _initialized = true;
    }

    return success;
}

bool InputSystemBase::uninitialize(void) {
    if(!_initialized || _managedExternally) return true;

    bool success = true;

    success &= gyInputUninit();

    _initialized = false;
    return success;
}

void InputSystemBase::_onControllerAttached(const unsigned slot) {
    dbgprintf("InputSystem: Controller Attached! [Port: %d]", slot);
    getEventSys()->broadcastEvent(ControllerEvent(EVENT_CONTROLLER_ATTACHED, &_controllers[slot]));
}

void InputSystemBase::_onControllerRemoved(const unsigned slot) {
    dbgprintf("InputSystem: Controller Removed! [Port: %d]", slot);
    getEventSys()->broadcastEvent(ControllerEvent(EVENT_CONTROLLER_REMOVED, &_controllers[slot]));
}

void InputSystemBase::_onMouseAttached(void) {
    dbgprintf("InputSystem: Mouse Attached!");
}

void InputSystemBase::_onMouseRemoved(void) {
    dbgprintf("InputSystem: Mouse Removed!");
}

void InputSystemBase::_onKeyboardAttached(void) {
    dbgprintf("InputSystem: Keyboard Attached!");
}

void InputSystemBase::_onKeyboardRemoved(void) {
    dbgprintf("InputSystem: Keyboard Removed!");
}

void InputSystemBase::_onVMUAttached(unsigned slot) {
    dbgprintf("InputSystem: VMU Attached! [Port: %d]", slot);
}

void InputSystemBase::_onVMURemoved(unsigned slot) {
    dbgprintf("InputSystem: VMU Removed! [Port: %d]", slot);
}

void InputSystemBase::_mapKeyboardToController(const int port) {
    GYVector2 lanalog = _controllers[port].getAnalogStick(GY_CONTROLLER_ANALOG_LEFT);

    if(_keyboard.isPressed(GY_KEY_C)) _controllers[port].setPressed(GY_CONTROLLER_BUT_X,            true);
    else if(_keyboard.getButtonState(GY_KEY_C) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_X, false);

    if(_keyboard.isPressed(GY_KEY_X)) _controllers[port].setPressed(GY_CONTROLLER_BUT_SQUARE,       true);
    else if(_keyboard.getButtonState(GY_KEY_X) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_SQUARE, false);


    if(_keyboard.isPressed(GY_KEY_D)) _controllers[port].setPressed(GY_CONTROLLER_BUT_CIRCLE,       true);
    else if(_keyboard.getButtonState(GY_KEY_D) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_CIRCLE, false);


    if(_keyboard.isPressed(GY_KEY_S)) _controllers[port].setPressed(GY_CONTROLLER_BUT_TRIANGLE,     true);
    else if(_keyboard.getButtonState(GY_KEY_S) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_TRIANGLE, false);


    if(_keyboard.isPressed(GY_KEY_W)) _controllers[port].setPressed(GY_CONTROLLER_BUT_LSHOULDER,    true);
    else if(_keyboard.getButtonState(GY_KEY_W) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_LSHOULDER, false);

    if(_keyboard.isPressed(GY_KEY_E)) _controllers[port].setPressed(GY_CONTROLLER_BUT_RSHOULDER,    true);
    else if(_keyboard.getButtonState(GY_KEY_E) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_RSHOULDER, false);

    if(_keyboard.isPressed(GY_KEY_Q)) _controllers[port].setPressed(GY_CONTROLLER_BUT_LANALOG,      true);
    else if(_keyboard.getButtonState(GY_KEY_Q) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_LANALOG, false);

    if(_keyboard.isPressed(GY_KEY_R)) _controllers[port].setPressed(GY_CONTROLLER_BUT_RANALOG,      true);
    else if(_keyboard.getButtonState(GY_KEY_R) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_RANALOG, false);

    if(_keyboard.isPressed(GY_KEY_RETURN)) _controllers[port].setPressed(GY_CONTROLLER_BUT_START,      true);
    else if(_keyboard.getButtonState(GY_KEY_RETURN) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_START, false);

    if(_keyboard.isPressed(GY_KEY_RSHIFT)) _controllers[port].setPressed(GY_CONTROLLER_BUT_SELECT,      true);
    else if(_keyboard.getButtonState(GY_KEY_RSHIFT) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_SELECT, false);

    if(_keyboard.getButtonState(GY_KEY_DPAD_U) == GY_BUT_STATE_RELEASED)        lanalog.y = 0.0f;
    else if(_keyboard.getButtonState(GY_KEY_DPAD_D) == GY_BUT_STATE_RELEASED)   lanalog.y = 0.0f;
    if(_keyboard.getButtonState(GY_KEY_DPAD_L) == GY_BUT_STATE_RELEASED)        lanalog.x = 0.0f;
    else if(_keyboard.getButtonState(GY_KEY_DPAD_R) == GY_BUT_STATE_RELEASED)   lanalog.x = 0.0f;

    if(_keyboard.isPressed(GY_KEY_DPAD_U)) lanalog.y = -1.0f;
    if(_keyboard.isPressed(GY_KEY_DPAD_D)) lanalog.y = 1.0f;
    if(_keyboard.isPressed(GY_KEY_DPAD_L)) lanalog.x = -1.0f;
    if(_keyboard.isPressed(GY_KEY_DPAD_R)) lanalog.x = 1.0f;

    _controllers[port].setAnalogStick(GY_CONTROLLER_ANALOG_LEFT, lanalog);
}

void InputSystemBase::dumpStats(void) {
    gyInputDeviceDump();
}

void InputSystemBase::update(void) {
    //update keyboard
    bool wasAttached   = _keyboard.isAttached();
    bool isAttached    = _keyboard.pollInput();

    if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onKeyboardAttached();
    else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onKeyboardRemoved();

    //update mouse
    wasAttached = _mouse.isAttached();
    isAttached = _mouse.pollInput();

    if(isAttached && !wasAttached)      _onMouseAttached();
    else if(!isAttached && wasAttached) _onMouseRemoved();

    //update controllers
    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) {
        const auto wasAttached  = _controllers[i].isAttached();
        const auto isAttached   = _controllers[i].pollInput();

         if((int)i == _keyboardAsControllerPort) _mapKeyboardToController(_keyboardAsControllerPort);
        _controllers[i].updateDerivedValues();

        if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onControllerAttached(i);
        else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onControllerRemoved(i);
    }

    //update VMUs
    for(unsigned i = 0; i < ELYSIAN_INPUT_VMUS_MAX; ++i) {
        const auto wasAttached  = _vmus[i].isAttached();
        const auto isAttached   = _vmus[i].pollDevice();

        if(isAttached && !wasAttached) {
            static_cast<InputSystem*>(this)->_onVMUAttached(i);
            getEventSys()->broadcastEvent(VisualMemoryUnitEvent(EVENT_VMU_ATTACHED, &_vmus[i]));
        } else if(!isAttached && wasAttached) {
            static_cast<InputSystem*>(this)->_onVMURemoved(i);
            getEventSys()->broadcastEvent(VisualMemoryUnitEvent(EVENT_VMU_REMOVED, &_vmus[i]));
        }
    }


}

}
