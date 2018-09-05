#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/system/elysian_event_system.hpp>
#include <libGyro/gyro_file_api.h>
#include <ESGamma/system/elysian_audio_system.hpp>

namespace elysian {

KeyboardEventObserver::KeyboardEventObserver(unsigned playerIndex):
    _playerIndex(playerIndex)
{
    System::getEventSys()->addEventObserver(this, EVENT_KEYBOARD_KEY);
}

void KeyboardEventObserver::onObserveEvent(Event* event, EventTarget* target) {
    if(!target && event->getEventType() == EVENT_KEYBOARD_KEY) {
        onKeyboardEvent(*System::getInputSys()->getKeyboard(_playerIndex));
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
        if(_gyHandle->buttonsChanged)       System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_BUTTON,    this));
        if(_gyHandle->screenPosChanged)     System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_MOVE,      this));
        if(_gyHandle->scrollWheelChanged)   System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_WHEEL,     this));
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

bool VisualMemoryUnit::frameBufferReadPixel(uint8_t x, uint8_t y) const {
    if(x >= 48 || y >= 32) {
        errprintf("Attempting to read oustide of VMU framebuffer boundaries: <%d, %d>", x, y);
        return false;
    }
    const int byte  = x / 8;
    const int bit   = x % 8;
    return (bool)(_frameBuffer[y][byte]>>bit)&0x1;
}
void VisualMemoryUnit::frameBufferWritePixel(uint8_t x, uint8_t y, bool pixelVal) {
    if(x >= 48 || y >= 32) {
        errprintf("Attempting to write oustide of VMU framebuffer boundaries: <%d, %d>", x, y);
        return;
    }
    const uint8_t byte  = x / 8;
    const uint8_t bit   = x % 8;
    const uint8_t prevVal = _frameBuffer[y][byte];
    if(pixelVal) {
        _frameBuffer[y][byte] |= (0x1<<bit);
    } else {
        _frameBuffer[y][byte] &= ~(0x1<<bit);
    }
    if(_frameBuffer[y][byte] != prevVal) _fbChanged = true;
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
    GYFile *fp = NULL;
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
    bool success = true;
    if(_gyHandle) success = gyVisualMemoryDeviceRemoteDisconnect(_gyHandle);
    free(_tcpSettings);
    _tcpSettings = nullptr;
    return success;
}


bool PowerSupply::pollDevice(void) {
    return gySystemPowerQueryInfo(&_gyPowerInfo);
}

bool InputSystemBase::_initialized          = false;
bool InputSystemBase::_managedExternally    = false;
const constexpr GYInputInitParams InputSystemBase::_defaultInitParams;

InputSystemBase::InputSystemBase(void) {

    GYVisualMemoryInitParams vmuInitParams = {
        ELYSIAN_INPUT_VMUS_MAX
    };
    gyVisualMemoryInit(&vmuInitParams);

    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) {
        _controllers[i].setPort(i);
        _controllers[i].setPlayerIndex(i);
    }
    _keyboard.setPlayerIndex(0);
    _mouse.setPlayerIndex(0);
    for(unsigned i = 0; i < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX; ++i) {
        _rumbleDevices[i].setPlayerIndex(i);
    }
    for(unsigned i = 0; i < ELYSIAN_INPUT_VMUS_MAX; ++i) {
        _vmus[i].setPlayerIndex(i);
    }

    _deviceAttachedSfx = gyAudWavCreate(ELYSIAN_INPUT_DEVICE_ATTACHED_SFX);
    _deviceRemovedSfx = gyAudWavCreate(ELYSIAN_INPUT_DEVICE_REMOVED_SFX);
    //Initial input poll, to initialize controller/mouse/keyboard handles
    update();
}

InputSystemBase::~InputSystemBase(void) {
    gyVisualMemoryUninit();
    gyAudWavDestroy(_deviceAttachedSfx);
    gyAudWavDestroy(_deviceRemovedSfx);
}

bool InputSystemBase::initialize(const GYInputInitParams*const params) {
    bool success = true;

    if(!_initialized) {
        success &= gyInputInit(params? params : &_defaultInitParams);
        if(params != &_defaultInitParams) _managedExternally = true;

        GYRumbleInitParams rumbleInitParams = {
            ELYSIAN_INPUT_RUMBLE_DEVICES_MAX
        };
        gyRumbleInit(&rumbleInitParams);
        _initialized = true;
    }

    return success;
}

bool InputSystemBase::uninitialize(void) {
    if(!_initialized || _managedExternally) return true;

    bool success = true;

    success &= gyInputUninit();
    success &= gyRumbleUninit();

    _initialized = false;
    return success;
}

void InputSystemBase::_onControllerAttached(const unsigned slot) {
    dbgprintf("InputSystem: Controller Attached! [Port: %d]", slot);
    getEventSys()->broadcastEvent(ControllerEvent(EVENT_CONTROLLER_ATTACHED, &_controllers[slot]));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onControllerRemoved(const unsigned slot) {
    dbgprintf("InputSystem: Controller Removed! [Port: %d]", slot);
    getEventSys()->broadcastEvent(ControllerEvent(EVENT_CONTROLLER_REMOVED, &_controllers[slot]));
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onMouseAttached(void) {
    dbgprintf("InputSystem: Mouse Attached!");
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_MOUSE_ATTACHED, 0, InputDeviceEvent::ATTACHED));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onMouseRemoved(void) {
    dbgprintf("InputSystem: Mouse Removed!");
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_MOUSE_REMOVED, 0, InputDeviceEvent::REMOVED));
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onKeyboardAttached(void) {
    dbgprintf("InputSystem: Keyboard Attached!");
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_KEYBOARD_ATTACHED, 0, InputDeviceEvent::ATTACHED));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onKeyboardRemoved(void) {
    dbgprintf("InputSystem: Keyboard Removed!");
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_KEYBOARD_REMOVED, 0, InputDeviceEvent::REMOVED));
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onVMUAttached(unsigned slot) {
    dbgprintf("InputSystem: VMU Attached! [Port: %d]", slot);
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
    getVMU(slot)->beep(2, 1);
#endif
    if(strlen(_vmuDefaultIcon)) {
        getVMU(slot)->loadIcon(_vmuDefaultIcon);
    }
    //Play Dreamcast-y style plug-in beep
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onVMURemoved(unsigned slot) {
    dbgprintf("InputSystem: VMU Removed! [Port: %d]", slot);
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onRumbleDeviceAttached(unsigned slot) {
    dbgprintf("InputSystem: RumbleDevice Attached! [Port: %d]", slot);
 //   getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onRumbleDeviceRemoved(unsigned slot) {
    dbgprintf("InputSystem: RumbleDevice Removed! [Port: %d]", slot);
}

void InputSystemBase::_onPowerStateChanged(void) {
    dbgprintf("InputSystem: PowerSupply State Changed! [%s]", gySystemPowerStateString(_powerSupply.getPowerState()));
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
    //Update all events
    gyInputPollEvents();

    //Update Keyboard
    bool wasAttached   = _keyboard.isAttached();
    bool isAttached    = _keyboard.pollInput();

    if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onKeyboardAttached();
    else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onKeyboardRemoved();

    //Update Mouse
    wasAttached = _mouse.isAttached();
    isAttached = _mouse.pollInput();

    if(isAttached && !wasAttached)      _onMouseAttached();
    else if(!isAttached && wasAttached) _onMouseRemoved();

    //Update Controllers
    for(unsigned i = 0; i < ELYSIAN_INPUT_CONTROLLERS_MAX; ++i) {
        const auto wasAttached  = _controllers[i].isAttached();
        const auto isAttached   = _controllers[i].pollInput();

         if((int)i == _keyboardAsControllerPort) _mapKeyboardToController(_keyboardAsControllerPort);
        _controllers[i].updateDerivedValues();

        if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onControllerAttached(i);
        else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onControllerRemoved(i);
    }

    //update Rumble Devices
    for(unsigned i = 0; i < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX; ++i) {
        const auto wasAttached  = _rumbleDevices[i].isAttached();
        const auto isAttached   = _rumbleDevices[i].pollDevice();

        if(isAttached && !wasAttached) {
            static_cast<InputSystem*>(this)->_onRumbleDeviceAttached(i);
            getEventSys()->broadcastEvent(RumbleDeviceEvent(EVENT_RUMBLE_DEVICE_ATTACHED, &_rumbleDevices[i]));
        } else if(!isAttached && wasAttached) {
            static_cast<InputSystem*>(this)->_onRumbleDeviceRemoved(i);
            getEventSys()->broadcastEvent(RumbleDeviceEvent(EVENT_RUMBLE_DEVICE_REMOVED, &_rumbleDevices[i]));
        }

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

    //update Power Supply
    GY_SYSTEM_POWER_STATE prevPowerState = _powerSupply.getPowerState();
    if(_powerSupply.pollDevice()) {
        GY_SYSTEM_POWER_STATE newPowerState = _powerSupply.getPowerState();
        if(newPowerState != newPowerState && newPowerState != GY_SYSTEM_POWER_STATE_UNKNOWN) {
            _onPowerStateChanged();
        }
    }

}

void InputSystemBase::broadcastVMUBeep(uint8_t pulseWidth, uint8_t dutyCycle) {
    for(unsigned i = 0; i < ELYSIAN_INPUT_VMUS_MAX; ++i) {
        if(getVMU(i)->isAttached()) getVMU(i)->beep(pulseWidth, dutyCycle);
    }
}

void InputSystemBase::broadcastVMUIcon(const char* path) {
    for(unsigned i = 0; i < ELYSIAN_INPUT_VMUS_MAX; ++i) {
        if(getVMU(i)->isAttached()) getVMU(i)->loadIcon(path);
    }
}

}
