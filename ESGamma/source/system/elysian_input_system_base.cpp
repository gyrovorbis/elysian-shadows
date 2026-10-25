/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_visual_memory_api.h>
#include <libGyro/gyro_sensor_api.h>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/crtp/elysian_timer_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <ESGamma/device/elysian_keyboard_device.hpp>
#include <ESGamma/device/elysian_mouse_device.hpp>
#include <ESGamma/device/elysian_controller_device.hpp>
#include <ESGamma/device/elysian_touch_device.hpp>
#include <ESGamma/device/elysian_visual_memory_device.hpp>
#include <ESGamma/device/elysian_rumble_device.hpp>
#include <ESGamma/device/elysian_power_supply_device.hpp>
#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
#   include <ESGamma/device/elysian_sensor_device.hpp>
#endif

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


//====== STATICZ =====

bool InputSystemBase::_initialized          = false;
bool InputSystemBase::_managedExternally    = false;
const constexpr GYInputInitParams InputSystemBase::_defaultInitParams;
const constexpr GYRumbleInitParams InputSystemBase::_defaultRumbleInitParams;
#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
const constexpr GYSensorInitParams InputSystemBase::_defaultSensorInitParams;
#endif

bool InputSystemBase::importControllerButtonMappings(const char*const filepath) {
    return (!_initialized)? false : gyControllerLoadMappingsFile(filepath);
}

//====== INSTANCED =====
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

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
    _accelerometers = new Accelerometer[ELYSIAN_INPUT_ACCELEROMETERS_MAX];
    for(int a = 0; a < ELYSIAN_INPUT_ACCELEROMETERS_MAX; ++a) {
        _accelerometers[a].setPlayerIndex(a);
    }

    _gyroscopes = new Gyroscope[ELYSIAN_INPUT_GYROSCOPES_MAX];
    for(int g = 0; g < ELYSIAN_INPUT_GYROSCOPES_MAX; ++g) {
        _gyroscopes[g].setPlayerIndex(g);
    }

    _magnetometers = new Magnetometer[ELYSIAN_INPUT_MAGNETOMETERS_MAX];
    for(int m = 0; m < ELYSIAN_INPUT_MAGNETOMETERS_MAX; ++m) {
        _magnetometers[m].setPlayerIndex(m);
    }

    _altimeters = new Altimeter[ELYSIAN_INPUT_ALTIMETERS_MAX];
    for(int a = 0; a < ELYSIAN_INPUT_ALTIMETERS_MAX; ++a) {
        _altimeters[a].setPlayerIndex(a);
    }

    _pedometers = new Pedometer[ELYSIAN_INPUT_PEDOMETERS_MAX];
    for(int p = 0; p < ELYSIAN_INPUT_PEDOMETERS_MAX; ++p) {
        _pedometers[p].setPlayerIndex(p);
    }

    _scales = new Scale[ELYSIAN_INPUT_SCALES_MAX];
    for(int s = 0; s < ELYSIAN_INPUT_SCALES_MAX; ++s) {
        _scales[s].setPlayerIndex(s);
    }
#endif

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

        success &= gyRumbleInit(&_defaultRumbleInitParams);

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
        success &= gySensorInit(&_defaultSensorInitParams);

        auto sensors = gySensorSupportQuery();
        Accelerometer::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_ACCELEROMETER);
        Gyroscope::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_GYROSCOPE);
        Magnetometer::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_MAGNETOMETER);
        Altimeter::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_ALTIMETER);
        Pedometer::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_PEDOMETER);
        Scale::setSupported(sensors & GY_SENSOR_DEVICE_SUPPORT_SCALE);
#endif
        _initialized = true;
    }

    return success;
}

bool InputSystemBase::uninitialize(void) {
    if(!_initialized || _managedExternally) return true;

    bool success = true;

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
    success &= gySensorUninit();
#endif
    success &= gyRumbleUninit();
    success &= gyInputUninit();

    _initialized = false;
    return success;
}

Controller*         InputSystemBase::getController(const unsigned port) const { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_controllers[port]; }
Keyboard*           InputSystemBase::getKeyboard(const unsigned port) const { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_keyboards[port]; }
Mouse*              InputSystemBase::getMouse(const unsigned port) const { assert(port < ELYSIAN_INPUT_MICE_MAX); return &_mice[port]; }
VisualMemoryUnit*   InputSystemBase::getVMU(unsigned port) const { assert(port < ELYSIAN_INPUT_VMUS_MAX); return &_vmus[port]; }
RumbleDevice*       InputSystemBase::getRumbleDevice(const unsigned port) const { assert(port < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX); return &_rumbleDevices[port]; }
TouchDevice*        InputSystemBase::getTouchDevice(const unsigned port) const { assert(port < ELYSIAN_INPUT_TOUCH_DEVICES_MAX); return &_touchDevices[port]; }
#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
Accelerometer*      InputSystemBase::getAccelerometer(const unsigned port) const { assert(port < ELYSIAN_INPUT_ACCELEROMETERS_MAX); return &_accelerometers[port]; }
Gyroscope*          InputSystemBase::getGyroscope(const unsigned port) const { assert(port < ELYSIAN_INPUT_GYROSCOPES_MAX); return &_gyroscopes[port]; }
Magnetometer*       InputSystemBase::getMagnetometer(const unsigned port) const { assert(port < ELYSIAN_INPUT_MAGNETOMETERS_MAX); return &_magnetometers[port]; }
Altimeter*          InputSystemBase::getAltimeter(const unsigned port) const { assert(port < ELYSIAN_INPUT_ALTIMETERS_MAX); return &_altimeters[port]; }
Pedometer*          InputSystemBase::getPedometer(const unsigned port) const { assert(port < ELYSIAN_INPUT_PEDOMETERS_MAX); return &_pedometers[port]; }
Scale*              InputSystemBase::getScale(const unsigned port) const { assert(port < ELYSIAN_INPUT_SCALES_MAX); return &_scales[port]; }
#endif
PowerSupply*        InputSystemBase::getPowerSupply(void) const { return _powerSupply; }

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

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
#define ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(EVENT, MEMBER, STR) \
void InputSystemBase::_on##STR##Removed(const unsigned slot) {      \
    dbgprintf("InputSystem: " #STR " Removed! [Port: %d]", slot);   \
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_##EVENT##_REMOVED, MEMBER[slot].getPlayerIndex(), InputDeviceEvent::REMOVED)); \
} \
void InputSystemBase::_on##STR##Attached(const unsigned slot) { \
    dbgprintf("InputSystem: " #STR "Attached! [Port: %d]", slot);   \
    getEventSys()->broadcastEvent(InputDeviceEvent(EVENT_##EVENT##_ATTACHED, MEMBER[slot].getPlayerIndex(), InputDeviceEvent::ATTACHED)); \
}

ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(ACCELEROMETER,    _accelerometers,    Accelerometer)
ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(GYROSCOPE,        _gyroscopes,        Gyroscope)
ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(MAGNETOMETER,     _magnetometers,     Magnetometer)
ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(ALTIMETER,        _altimeters,        Altimeter)
ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(PEDOMETER,        _pedometers,        Pedometer)
ELYSIAN_INPUT_SYSTEM_SENSOR_EVENT_GENERIC(SCALE,            _scales,            Scale)


#endif

void InputSystemBase::_onPowerStateChanged(void) {
    dbgprintf("InputSystem: PowerSupply State Changed! [%s]", gySystemPowerStateString(_powerSupply->getPowerState()));
}

#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED

#endif



void InputSystemBase::dumpStats(void) {
    gyInputDeviceDump();
}

void InputSystemBase::update(void) {
#define INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(MEMBER, METHOD, COUNT)    \
    for(unsigned d = 0; d < COUNT; ++d) {                                   \
        bool wasAttached   = MEMBER[d].isAttached();                        \
        bool isAttached    = MEMBER[d].pollInput();                         \
        if(isAttached && !wasAttached)                                      \
            static_cast<InputSystem*>(this)->_on##METHOD##Attached(d);      \
        else if(!isAttached && wasAttached)                                 \
            static_cast<InputSystem*>(this)->_on##METHOD##Removed(d);       \
    }

    //Update all events
    gyInputPollEvents();

    //Update Keyboards
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_keyboards, Keyboard, ELYSIAN_INPUT_KEYBOARDS_MAX)

    //Update Mice
    for(unsigned m = 0; m < ELYSIAN_INPUT_MICE_MAX; ++m) {
        auto& mouse = *getMouse((int)m);
        bool wasAttached = mouse.isAttached();
        bool isAttached = mouse.pollInput(getTimerSys()->getTotalTicks());

        if(isAttached && !wasAttached)      _onMouseAttached(m);
        else if(!isAttached && wasAttached) _onMouseRemoved(m);

    }

    //Update TouchDevices
    for(unsigned t = 0; t < ELYSIAN_INPUT_TOUCH_DEVICES_MAX; ++t) {
        bool wasAttached = _touchDevices[t].isAttached();
        bool isAttached = _touchDevices[t].pollInput();

        if(getPortMappings(t)->isTouchDeviceMappedToMouse())
            _mapTouchDeviceToMouse(t);
        if(getPortMappings(t)->isMouseMappedToTouchDevice())
            _mapMouseToTouchDevice(t);

        if(isAttached && !wasAttached)      _onTouchDeviceAttached(t);
        else if(!isAttached && wasAttached) _onTouchDeviceRemoved(t);
    }

    //Update Controllers
    for(int c = 0; c < ELYSIAN_INPUT_CONTROLLERS_MAX; ++c) {
        auto& cont = *getController((unsigned)c);
        const auto wasAttached  = cont.isAttached();
        const auto isAttached   = cont.pollInput();

         if(getPortMappings(c)->isKeyboardMappedToController())
             _mapKeyboardToController(c);

        cont.updateDerivedValues();

        if(getPortMappings(c)->isControllerMappedToMouse())
            _mapControllerToMouse(c);
        if(getPortMappings(c)->isControllerMappedToTouchDevice())
            _mapControllerToTouchDevice(c);

        if(isAttached && !wasAttached)      static_cast<InputSystem*>(this)->_onControllerAttached(c);
        else if(!isAttached && wasAttached) static_cast<InputSystem*>(this)->_onControllerRemoved(c);

        //Check for input events!
        if(isAttached) {
            if(cont.didAnyButtonChange()) {
                for(int b = 0; b < GY_CONTROLLER_BUT_COUNT; ++b) {
                    GY_CONTROLLER_BUTTON button = static_cast<GY_CONTROLLER_BUTTON>(b);
                    bool pressed = cont.isPressed(button);
                    if(pressed != cont.wasPressed(button)) {
                        getEventSys()->broadcastEvent(
                                    ControllerButtonEvent(pressed? EVENT_CONTROLLER_BUTTON_PRESSED :
                                                                   EVENT_CONTROLLER_BUTTON_RELEASED, &cont, button));
                    }
                }
            }

            for(int a = 0; a < GY_CONTROLLER_ANALOG_COUNT; ++a) {
                GY_CONTROLLER_ANALOG alog = static_cast<GY_CONTROLLER_ANALOG>(a);
                if(cont.didAnalogChange(alog)) {
                    getEventSys()->broadcastEvent(
                                ControllerAnalogEvent(EVENT_CONTROLLER_ANALOG_MOVED, &cont, alog));
                }
            }

            for(int t = 0; t < GY_CONTROLLER_TRIGGER_COUNT; ++t) {
                GY_CONTROLLER_TRIGGER trig = static_cast<GY_CONTROLLER_TRIGGER>(t);
                if(cont.didTriggerChange(trig)) {
                    getEventSys()->broadcastEvent(
                                ControllerTriggerEvent(EVENT_CONTROLLER_TRIGGER_MOVED, &cont, trig));
                }
            }
        }
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

    //update Sensors
#ifdef ELYSIAN_INPUT_SYSTEM_SENSORS_ENABLED
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_accelerometers, Accelerometer,  ELYSIAN_INPUT_ACCELEROMETERS_MAX)
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_gyroscopes,     Gyroscope,      ELYSIAN_INPUT_GYROSCOPES_MAX)
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_magnetometers,  Magnetometer,   ELYSIAN_INPUT_MAGNETOMETERS_MAX)
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_altimeters,     Altimeter,      ELYSIAN_INPUT_ALTIMETERS_MAX)
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_pedometers,     Pedometer,      ELYSIAN_INPUT_PEDOMETERS_MAX)
    INPUT_SYSTEM_UPDATE_DEVICE_GENERIC(_scales,         Scale,          ELYSIAN_INPUT_SCALES_MAX)
#endif

#undef INPUT_SYSTEM_UPDATE_GENERIC_DEVICE_UPDATE

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
