#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_visual_memory_api.h>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <ESGamma/device/elysian_keyboard_device.hpp>
#include <ESGamma/device/elysian_mouse_device.hpp>
#include <ESGamma/device/elysian_controller_device.hpp>
#include <ESGamma/device/elysian_touch_device.hpp>
#include <ESGamma/device/elysian_visual_memory_device.hpp>
#include <ESGamma/device/elysian_rumble_device.hpp>
#include <ESGamma/device/elysian_power_supply_device.hpp>

namespace elysian {

KeyEventObserver::KeyEventObserver(unsigned playerIndex):
    _playerIndex(playerIndex)
{
    System::getEventSys()->addEventObserver(this, EVENT_KEYBOARD_KEY_PRESSED);
    System::getEventSys()->addEventObserver(this, EVENT_KEYBOARD_KEY_RELEASED);
}
#if 0
void KeyEventObserver::onObserveEvent(Event* event, EventTarget* target) {
    if(!target) {
        InputEvent* inputEvent = static_cast<InputEvent*>(event);
        switch(event->getEventType()) {
        case EVENT_KEYBOARD_KEY_PRESSED:
            if(inputEvent->getPlayerIndex() == (int)_playerIndex)
                onKeyPressEvent(static_cast<KeyEvent*>(event));
            break;
        case EVENT_KEYBOARD_KEY_RELEASED:
            if(inputEvent->getPlayerIndex() == (int)_playerIndex)
                onKeyReleaseEvent(static_cast<KeyEvent*>(event));
            break;
        default:
            break;
        }
    }
}
#endif

void KeyEventObserver::onKeyPressEvent(KeyEvent*) {}
void KeyEventObserver::onKeyReleaseEvent(KeyEvent*) {}


bool InputSystemBase::_initialized          = false;
bool InputSystemBase::_managedExternally    = false;
const constexpr GYInputInitParams InputSystemBase::_defaultInitParams;

InputSystemBase::InputSystemBase(void) {

    GYVisualMemoryInitParams vmuInitParams = {
        ELYSIAN_INPUT_VMUS_MAX
    };
    gyVisualMemoryInit(&vmuInitParams);

    _portMappings = new InputPortMappings[ELYSIAN_INPUT_PLAYERS_MAX];
    //don't hardcode me!
    _portMappings[0].mapKeyboardToController();

    _powerSupply = new PowerSupply();

    _controllers = new Controller[ELYSIAN_INPUT_CONTROLLERS_MAX];
    for(unsigned c = 0; c < ELYSIAN_INPUT_CONTROLLERS_MAX; ++c) {
        _controllers[c].setPort(c);
        _controllers[c].setPlayerIndex(c);
    }

    _keyboards = new Keyboard[ELYSIAN_INPUT_KEYBOARDS_MAX];
    for(unsigned k = 0; k < ELYSIAN_INPUT_KEYBOARDS_MAX; ++k) {
        _keyboards[k].setPlayerIndex(k);
    }

    _mice = new Mouse[ELYSIAN_INPUT_MICE_MAX];
    for(unsigned m = 0; m < ELYSIAN_INPUT_MICE_MAX; ++m) {
        _mice[m].setPlayerIndex(m);
    }

    _touchDevices = new TouchDevice[ELYSIAN_INPUT_TOUCH_DEVICES_MAX];
    for(unsigned t = 0; t < ELYSIAN_INPUT_TOUCH_DEVICES_MAX; ++t) {
        _touchDevices[t].setPlayerIndex(t);
    }

    _rumbleDevices = new RumbleDevice[ELYSIAN_INPUT_RUMBLE_DEVICES_MAX];
    for(unsigned r = 0; r < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX; ++r) {
        _rumbleDevices[r].setPlayerIndex(r);
    }

    _vmus = new VisualMemoryUnit[ELYSIAN_INPUT_VMUS_MAX];
    for(unsigned v = 0; v < ELYSIAN_INPUT_VMUS_MAX; ++v) {
        _vmus[v].setPlayerIndex(v);
    }

    _deviceAttachedSfx = gyAudWavCreate(ELYSIAN_INPUT_DEVICE_ATTACHED_SFX);
    _deviceRemovedSfx = gyAudWavCreate(ELYSIAN_INPUT_DEVICE_REMOVED_SFX);
    //Initial input poll, to initialize controller/mouse/keyboard handles
    update();
}

InputSystemBase::~InputSystemBase(void) {
    delete[] _rumbleDevices;
    delete[] _vmus;
    delete[] _touchDevices;
    delete[] _mice;
    delete[] _keyboards;
    delete[] _controllers;
    delete[] _powerSupply;
    delete[] _portMappings;

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

Controller* InputSystemBase::getController(const unsigned port) const { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_controllers[port]; }
Keyboard* InputSystemBase::getKeyboard(const unsigned port) const { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_keyboards[port]; }
Mouse* InputSystemBase::getMouse(const unsigned port) const { assert(port < ELYSIAN_INPUT_MICE_MAX); return &_mice[port]; }
VisualMemoryUnit* InputSystemBase::getVMU(unsigned port) const { assert(port < ELYSIAN_INPUT_VMUS_MAX); return &_vmus[port]; }
RumbleDevice* InputSystemBase::getRumbleDevice(const unsigned port) const { assert(port < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX); return &_rumbleDevices[port]; }
TouchDevice* InputSystemBase::getTouchDevice(const unsigned port) const { assert(port < ELYSIAN_INPUT_TOUCH_DEVICES_MAX); return &_touchDevices[port]; }
PowerSupply* InputSystemBase::getPowerSupply(void) const { return _powerSupply; }

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

void InputSystemBase::_onMouseAttached(const unsigned slot) {
    dbgprintf("InputSystem: Mouse Attached! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_MOUSE_ATTACHED, (int)_mice[slot].getPlayerIndex(), InputDeviceEvent::ATTACHED));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onMouseRemoved(const unsigned slot) {
    dbgprintf("InputSystem: Mouse Removed! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_MOUSE_REMOVED, (int)_mice[slot].getPlayerIndex(), InputDeviceEvent::REMOVED));
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onKeyboardAttached(const unsigned slot) {
    dbgprintf("InputSystem: Keyboard Attached! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_KEYBOARD_ATTACHED, (int)_keyboards[slot].getPlayerIndex(), InputDeviceEvent::ATTACHED));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onKeyboardRemoved(const unsigned slot) {
    dbgprintf("InputSystem: Keyboard Removed! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_KEYBOARD_REMOVED, (int)_keyboards[slot].getPlayerIndex(), InputDeviceEvent::REMOVED));
    getAudioSys()->playSfxChannel(-1, *_deviceRemovedSfx);
}

void InputSystemBase::_onTouchDeviceAttached(const unsigned slot) {
    dbgprintf("InputSystem: TouchDevice Attached! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_TOUCH_DEVICE_ATTACHED, (int)_touchDevices[slot].getPlayerIndex(), InputDeviceEvent::ATTACHED));
    getAudioSys()->playSfxChannel(-1, *_deviceAttachedSfx);
}

void InputSystemBase::_onTouchDeviceRemoved(const unsigned slot) {
    dbgprintf("InputSystem: TouchDevice Removed! [Port: %d]", slot);
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_TOUCH_DEVICE_REMOVED, (int)_touchDevices[slot].getPlayerIndex(), InputDeviceEvent::REMOVED));
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
    dbgprintf("InputSystem: PowerSupply State Changed! [%s]", gySystemPowerStateString(_powerSupply->getPowerState()));
}



void InputSystemBase::dumpStats(void) {
    gyInputDeviceDump();
}

void InputSystemBase::update(void) {
    //Update all events
    gyInputPollEvents();

    //Update Keyboards
    for(unsigned k = 0; k < ELYSIAN_INPUT_KEYBOARDS_MAX; ++k) {
        bool wasAttached   = _keyboards[k].isAttached();
        bool isAttached    = _keyboards[k].pollInput();

        if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onKeyboardAttached(k);
        else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onKeyboardRemoved(k);
    }

    //Update Mice
    for(unsigned m = 0; m < ELYSIAN_INPUT_MICE_MAX; ++m) {
        bool wasAttached = _mice[m].isAttached();
        bool isAttached = _mice[m].pollInput();

        if(isAttached && !wasAttached)      _onMouseAttached(m);
        else if(!isAttached && wasAttached) _onMouseRemoved(m);
    }

    //Update TouchDevices
    for(unsigned t = 0; t < ELYSIAN_INPUT_TOUCH_DEVICES_MAX; ++t) {
        bool wasAttached = _touchDevices[t].isAttached();
        bool isAttached = _touchDevices[t].pollInput();

        if(_portMappings[t].isTouchDeviceMappedToMouse())
            _mapTouchDeviceToMouse(t);
        if(_portMappings[t].isMouseMappedToTouchDevice())
            _mapMouseToTouchDevice(t);

        if(isAttached && !wasAttached)      _onTouchDeviceAttached(t);
        else if(!isAttached && wasAttached) _onTouchDeviceRemoved(t);
    }

    //Update Controllers
    for(unsigned c = 0; c < ELYSIAN_INPUT_CONTROLLERS_MAX; ++c) {
        const auto wasAttached  = _controllers[c].isAttached();
        const auto isAttached   = _controllers[c].pollInput();

         if(_portMappings[c].isKeyboardMappedToController())
             _mapKeyboardToController(c);

        _controllers[c].updateDerivedValues();

        if(_portMappings[c].isControllerMappedToMouse())
            _mapControllerToMouse(c);
        if(_portMappings[c].isControllerMappedToTouchDevice())
            _mapControllerToTouchDevice(c);

        if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onControllerAttached(c);
        else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onControllerRemoved(c);
    }

    //update Rumble Devices
    for(unsigned r = 0; r < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX; ++r) {
        const auto wasAttached  = _rumbleDevices[r].isAttached();
        const auto isAttached   = _rumbleDevices[r].pollDevice();

        if(isAttached && !wasAttached) {
            static_cast<InputSystem*>(this)->_onRumbleDeviceAttached(r);
            getEventSys()->broadcastEvent(RumbleDeviceEvent(EVENT_RUMBLE_DEVICE_ATTACHED, &_rumbleDevices[r]));
        } else if(!isAttached && wasAttached) {
            static_cast<InputSystem*>(this)->_onRumbleDeviceRemoved(r);
            getEventSys()->broadcastEvent(RumbleDeviceEvent(EVENT_RUMBLE_DEVICE_REMOVED, &_rumbleDevices[r]));
        }

    }

    //update VMUs
    for(unsigned v = 0; v < ELYSIAN_INPUT_VMUS_MAX; ++v) {
        const auto wasAttached  = _vmus[v].isAttached();
        const auto isAttached   = _vmus[v].pollDevice();

        if(isAttached && !wasAttached) {
            static_cast<InputSystem*>(this)->_onVMUAttached(v);
            getEventSys()->broadcastEvent(VisualMemoryUnitEvent(EVENT_VMU_ATTACHED, &_vmus[v]));
        } else if(!isAttached && wasAttached) {
            static_cast<InputSystem*>(this)->_onVMURemoved(v);
            getEventSys()->broadcastEvent(VisualMemoryUnitEvent(EVENT_VMU_REMOVED, &_vmus[v]));
        }
    }

    //update Power Supply
    GY_SYSTEM_POWER_STATE prevPowerState = _powerSupply->getPowerState();
    if(_powerSupply->pollDevice()) {
        GY_SYSTEM_POWER_STATE newPowerState = _powerSupply->getPowerState();
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

void InputSystemBase::_mapKeyboardToController(const int port) {
    GYVector2 lanalog = _controllers[port].getAnalogStick(GY_CONTROLLER_ANALOG_LEFT);

    if(_keyboards[port].isPressed(GY_KEY_C)) _controllers[port].setPressed(GY_CONTROLLER_BUT_X,            true);
    else if(_keyboards[port].getButtonState(GY_KEY_C) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_X, false);

    if(_keyboards[port].isPressed(GY_KEY_X)) _controllers[port].setPressed(GY_CONTROLLER_BUT_SQUARE,       true);
    else if(_keyboards[port].getButtonState(GY_KEY_X) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_SQUARE, false);

    if(_keyboards[port].isPressed(GY_KEY_D)) _controllers[port].setPressed(GY_CONTROLLER_BUT_CIRCLE,       true);
    else if(_keyboards[port].getButtonState(GY_KEY_D) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_CIRCLE, false);

    if(_keyboards[port].isPressed(GY_KEY_S)) _controllers[port].setPressed(GY_CONTROLLER_BUT_TRIANGLE,     true);
    else if(_keyboards[port].getButtonState(GY_KEY_S) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_TRIANGLE, false);

    if(_keyboards[port].isPressed(GY_KEY_W)) _controllers[port].setPressed(GY_CONTROLLER_BUT_LSHOULDER,    true);
    else if(_keyboards[port].getButtonState(GY_KEY_W) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_LSHOULDER, false);

    if(_keyboards[port].isPressed(GY_KEY_E)) _controllers[port].setPressed(GY_CONTROLLER_BUT_RSHOULDER,    true);
    else if(_keyboards[port].getButtonState(GY_KEY_E) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_RSHOULDER, false);

    if(_keyboards[port].isPressed(GY_KEY_Q)) _controllers[port].setPressed(GY_CONTROLLER_BUT_LANALOG,      true);
    else if(_keyboards[port].getButtonState(GY_KEY_Q) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_LANALOG, false);

    if(_keyboards[port].isPressed(GY_KEY_R)) _controllers[port].setPressed(GY_CONTROLLER_BUT_RANALOG,      true);
    else if(_keyboards[port].getButtonState(GY_KEY_R) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_RANALOG, false);

    if(_keyboards[port].isPressed(GY_KEY_RETURN)) _controllers[port].setPressed(GY_CONTROLLER_BUT_START,      true);
    else if(_keyboards[port].getButtonState(GY_KEY_RETURN) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_START, false);

    if(_keyboards[port].isPressed(GY_KEY_RSHIFT)) _controllers[port].setPressed(GY_CONTROLLER_BUT_SELECT,      true);
    else if(_keyboards[port].getButtonState(GY_KEY_RSHIFT) == GY_BUT_STATE_RELEASED) _controllers[port].setPressed(GY_CONTROLLER_BUT_SELECT, false);

    if(_keyboards[port].getButtonState(GY_KEY_DPAD_U) == GY_BUT_STATE_RELEASED)        lanalog.y = 0.0f;
    else if(_keyboards[port].getButtonState(GY_KEY_DPAD_D) == GY_BUT_STATE_RELEASED)   lanalog.y = 0.0f;
    if(_keyboards[port].getButtonState(GY_KEY_DPAD_L) == GY_BUT_STATE_RELEASED)        lanalog.x = 0.0f;
    else if(_keyboards[port].getButtonState(GY_KEY_DPAD_R) == GY_BUT_STATE_RELEASED)   lanalog.x = 0.0f;

    if(_keyboards[port].isPressed(GY_KEY_DPAD_U)) lanalog.y = -1.0f;
    if(_keyboards[port].isPressed(GY_KEY_DPAD_D)) lanalog.y = 1.0f;
    if(_keyboards[port].isPressed(GY_KEY_DPAD_L)) lanalog.x = -1.0f;
    if(_keyboards[port].isPressed(GY_KEY_DPAD_R)) lanalog.x = 1.0f;

    _controllers[port].setAnalogStick(GY_CONTROLLER_ANALOG_LEFT, lanalog);
}

void InputSystemBase::_mapControllerToMouse(const int) {}
void InputSystemBase::_mapControllerToTouchDevice(const int) {}
void InputSystemBase::_mapMouseToTouchDevice(const int) {}
void InputSystemBase::_mapTouchDeviceToMouse(const int) {}

}
