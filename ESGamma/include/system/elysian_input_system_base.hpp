#ifndef ELYSIAN_INPUT_SYSTEM_BASE_HPP
#define ELYSIAN_INPUT_SYSTEM_BASE_HPP

#define ELYSIAN_INPUT_PLAYERS_MAX                       4
#define ELYSIAN_INPUT_CONTROLLERS_MAX                   ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_VMUS_MAX                          ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_RUMBLE_DEVICES_MAX                ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT   0
#define ELYSIAN_INPUT_DEVICE_ATTACHED_SFX               "/assets/audio/sfx/UI/device_connect.wav"
#define ELYSIAN_INPUT_DEVICE_REMOVED_SFX                "/assets/audio/sfx/UI/device_disconnect.wav"

#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN    0.3f
#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MAX    0.7f

#include <system/elysian_system.hpp>
#include <device/elysian_rumble_device.hpp>
#include <gyro_input_api.h>
#include <gyro_visual_memory_api.h>
#include <system/elysian_event_system.hpp>

struct GYWav;

namespace elysian {
    class Mouse;
    class Controller;

    class InputDeviceEvent: public Event {
    public:
        enum DEVICE_EVENT_TYPE {
            ATTACHED,
            REMOVED,
            STATE_CHANGED,
            UNKNOWN
        };
    private:
        int                  _playerIndex       = -1;
        DEVICE_EVENT_TYPE    _deviceEventType   = UNKNOWN;
    public:
                            InputDeviceEvent(EventType eventType, int playerIndex, DEVICE_EVENT_TYPE devType);
        int                 getPlayerIndex(void) const;
        DEVICE_EVENT_TYPE   getDeviceEventType(void) const;
    };


    class TouchDevice: public PlayerIODevice {

    };

    class KeyboardEventObserver: public EventObserver {
    private:
        unsigned        _playerIndex = 0;
    public:
                        KeyboardEventObserver(unsigned playerIndex=0);
        virtual void    onObserveEvent(Event* event, EventTarget* target) override;
        virtual void    onKeyboardEvent(const Keyboard& keybd);
    };

    class KeyboardEvent: public InputDeviceEvent {
    private:


    };


    class Keyboard: public PlayerIODevice {
    private:
        GYKeyboard*         _gyHandle   = nullptr;

    public:
        GY_BUTTON_STATE     getButtonState(const GY_KEY key) const;
        bool                isPressed(const GY_KEY key) const;
        void                setPressed(const GY_KEY key, const bool pressed) const;
        bool                isTapped(const GY_KEY key) const;

        bool                pollInput(void);
    };


    class MouseEvent: public Event {
    private:
        const Mouse*    _mouse = nullptr;
    public:
                        MouseEvent(EventType type, const Mouse* mouse);
        const Mouse*    getMouse(void) const;
    };

    class Mouse: public PlayerIODevice {
    private:
        GYMouse*            _gyHandle = nullptr;

    public:
        GY_BUTTON_STATE     getButtonState(const GY_MOUSE_BUTTON button) const;
        const GYVector2&    getScreenPos(void) const;
        const GYVector2&    getScrollWheel(void) const;
        bool                isPressed(const GY_MOUSE_BUTTON button) const;
        void                setPressed(const GY_MOUSE_BUTTON button, const bool pressed) const;
        bool                isTapped(const GY_MOUSE_BUTTON button) const;

        bool                pollInput(void);

    };

    class VirtualDPad {
    private:
        unsigned            _dpad[4]    = { 0 };

        static unsigned     _toIndex(const GY_CONTROLLER_BUTTON button);

    public:
                            VirtualDPad(void) = default;
                            VirtualDPad(const GYVector2& src);

        GY_BUTTON_STATE     getButtonState(const GY_CONTROLLER_BUTTON button) const;
        bool                isPressed(const GY_CONTROLLER_BUTTON button) const;
        bool                isTapped(const GY_CONTROLLER_BUTTON button) const;

        void                updateState(const GYVector2& analog);
    };

    class ControllerEvent: public InputDeviceEvent {
    private:
        Controller*         _controller     = nullptr;
    public:
                             ControllerEvent(EventType type, const Controller* controller);
                             ControllerEvent(EventType type, int slot);
        Controller*          getController(void) const;
    };

    class Controller: public PlayerIODevice {
    private:
        GYVector2           _analogDirs[GY_CONTROLLER_ANALOG_COUNT]     = { { 0.0f, 0.0f } };
        VirtualDPad         _virtualDPads[GY_CONTROLLER_ANALOG_COUNT];
        float               _analogMags[GY_CONTROLLER_ANALOG_COUNT]     = { 0.0f };
        float               _analogAngles[GY_CONTROLLER_ANALOG_COUNT]   = { 0.0f };
        GYController*       _gyHandle                                   = nullptr;
        unsigned            _port                                       = 0;

        void                _clearDerivedValues(void);

    public:
        bool                pollInput(void);
        void                updateDerivedValues(void);

        const GYVector2&    getAnalogStick(const GY_CONTROLLER_ANALOG stick) const;
        void                setAnalogStick(const GY_CONTROLLER_ANALOG stick, const GYVector2& vec);
        const GYVector2&    getAnalogDir(const GY_CONTROLLER_ANALOG stick) const;
        float               getAnalogMag(const GY_CONTROLLER_ANALOG stick) const;
        float               getAnalogAngle(const GY_CONTROLLER_ANALOG stick) const;
        const VirtualDPad&  getVirtualDPad(const GY_CONTROLLER_ANALOG stick) const;

        GY_BUTTON_STATE     getButtonState(const GY_CONTROLLER_BUTTON button) const;
        bool                isPressed(const GY_CONTROLLER_BUTTON button) const;
        void                setPressed(const GY_CONTROLLER_BUTTON button, const bool pressed);
        bool                isTapped(const GY_CONTROLLER_BUTTON button) const;

        unsigned            getPort(void) const;
        void                setPort(const unsigned port);



        //bool hasRAnalog();
        //bool hasLAnalog();

    };


    class VisualMemoryUnit;

    class VisualMemoryUnitEvent: public Event {
     private:
        VisualMemoryUnit* _vmu;
    public:
                        VisualMemoryUnitEvent(EventType type, VisualMemoryUnit* vmu);
    VisualMemoryUnit*   getVMU(void) const;
    };


    class VisualMemoryUnit: public PlayerIODevice {
    public:
        enum VMU_TYPE {
            VMU_NONE,
            VMU_PHYSICAL,
            VMU_VIRTUAL
        };
    private:
        uint8_t                     _frameBuffer[32][6]     = { { 0 } };
        GYVisualMemoryTCPSettings*  _tcpSettings            = nullptr;
        GYVisualMemoryDevice*       _gyHandle               = nullptr;
        VMU_TYPE                    _vmuType                = VMU_NONE;
        bool                        _fbChanged              = false;
    public:
        bool                        readBlock(uint16_t blockNum, uint8_t* buffer) const;
        bool                        writeBlock(uint16_t blockNum, const uint8_t* buffer);
        bool                        beep(uint8_t pulseLength, uint8_t dutyCycle);
        bool                        setFramebuffer(const char* buff);
        bool                        loadIcon(const char* filePath);
        bool                        frameBufferReadPixel(uint8_t x, uint8_t y) const;
        void                        frameBufferWritePixel(uint8_t x, uint8_t y, bool pixelVal);
        bool                        pollDevice(void);
        bool                        isRemoteConnected(void) const;
        VMU_TYPE                    getVmuType(void) const;
        bool                        connectToEVMU(const char* hostName, unsigned port);
        bool                        disconnectFromEVMU(void);
    };

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
        unsigned                getBatterySecondsRemaining(void) const;
        GY_SYSTEM_POWER_STATE   getPowerState(void) const;
        bool                    isBattery(void) const;
    };

    class InputSystemBase: public System {
    protected:

        constexpr const static
        GYInputInitParams _defaultInitParams =
#if GYRO_PLATFORM == GYRO_PLATFORM_Qt
        {
            nullptr,
            nullptr,
            ELYSIAN_INPUT_CONTROLLERS_MAX
        };
#else
        { 'a' };
#endif

        static bool         _initialized;
        static bool         _managedExternally;

        Controller          _controllers[ELYSIAN_INPUT_CONTROLLERS_MAX];
        VisualMemoryUnit    _vmus[ELYSIAN_INPUT_VMUS_MAX];
        RumbleDevice        _rumbleDevices[ELYSIAN_INPUT_RUMBLE_DEVICES_MAX];
        Keyboard            _keyboard;
        Mouse               _mouse;
        PowerSupply         _powerSupply;
        char                _vmuDefaultIcon[GYRO_PATH_MAX_SIZE]         = { '\0' };
        GYWav*              _deviceAttachedSfx                          = nullptr;
        GYWav*              _deviceRemovedSfx                           = nullptr;

        int                 _keyboardAsControllerPort = ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT;


        void                _onControllerAttached(const unsigned slot);
        void                _onControllerRemoved(const unsigned slot);
        void                _onKeyboardAttached(void);
        void                _onKeyboardRemoved(void);
        void                _onMouseAttached(void);
        void                _onMouseRemoved(void);
        void                _onVMUAttached(unsigned slot);
        void                _onVMURemoved(unsigned slot);
        void                _onRumbleDeviceAttached(unsigned slot);
        void                _onRumbleDeviceRemoved(unsigned slot);
        void                _onPowerStateChanged(void);

        void                _mapKeyboardToController(const int port);

    public:

                            InputSystemBase(void);
                            ~InputSystemBase(void);

        //allow external software (ESTk) to initialize input outside the engine with custom parameters
        static bool         initialize(const GYInputInitParams*const params=nullptr);
        static bool         uninitialize(void);


        void                update(void);
        void                dumpStats(void);

        Controller*         getController(const unsigned port);
        auto                getController(const unsigned port) const -> const Controller*;
        Keyboard*           getKeyboard(const unsigned port);
        Mouse*              getMouse(const unsigned port);
        VisualMemoryUnit*   getVMU(const unsigned port);
        RumbleDevice*       getRumbleDevice(const unsigned port);
        PowerSupply*        getPowerSupply(void);

        void                setKeyboardAsControllerPort(const int port);

        void                setDefaultVMUIcon(const char* path);
        void                broadcastVMUIcon(const char* path);
        void                broadcastVMUBeep(uint8_t pulseWidth, uint8_t dutyCycle);

    };

    //===== EVENT INLINEZ =======
    inline InputDeviceEvent::InputDeviceEvent(EventType eventType, int playerIndex, DEVICE_EVENT_TYPE devType):
        Event(eventType), _playerIndex(playerIndex), _deviceEventType(devType) {}
    inline int InputDeviceEvent::getPlayerIndex(void) const { return _playerIndex; }
    inline auto InputDeviceEvent::getDeviceEventType(void) const -> DEVICE_EVENT_TYPE { return _deviceEventType; }

    //====== KEYBOARD INLINEZ ===========
    inline GY_BUTTON_STATE Keyboard::getButtonState(const GY_KEY key) const { return (GY_BUTTON_STATE)_gyHandle->keys[key]; }
    inline bool Keyboard::isPressed(const GY_KEY key) const { return (_gyHandle->keys[key] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Keyboard::isTapped(const GY_KEY key) const { return _gyHandle->keys[key] == GY_BUT_STATE_TAPPED; }
    inline void Keyboard::setPressed(const GY_KEY key, const bool pressed) const {
        _gyHandle->keys[key] &= ~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->keys[key] |= pressed;
    }

    //====== MOUSE INLINEZ ===========
    inline MouseEvent::MouseEvent(EventType type, const Mouse* mouse):
        Event(type),
        _mouse(mouse)
    {}
    inline const Mouse* MouseEvent::getMouse(void) const { return _mouse; }

    inline bool Mouse::isPressed(const GY_MOUSE_BUTTON button) const { return (_gyHandle->buttons[button] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Mouse::isTapped(const GY_MOUSE_BUTTON button) const { return _gyHandle->buttons[button] == GY_BUT_STATE_TAPPED; }
    inline GY_BUTTON_STATE Mouse::getButtonState(const GY_MOUSE_BUTTON button) const { return (GY_BUTTON_STATE)_gyHandle->buttons[button]; }
    inline const GYVector2& Mouse::getScrollWheel(void) const { return _gyHandle->scrollWheel; }
    inline const GYVector2& Mouse::getScreenPos(void) const { return _gyHandle->screenPos; }
    inline void Mouse::setPressed(const GY_MOUSE_BUTTON button, const bool pressed) const {
        _gyHandle->buttons[button] &= (unsigned)~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->buttons[button] |= (unsigned)pressed;
    }

    //====== VIRTUAL DPAD INLINEZ ===========
    inline VirtualDPad::VirtualDPad(const GYVector2 &src) { updateState(src); }
    inline unsigned VirtualDPad::_toIndex(const GY_CONTROLLER_BUTTON button) {
        switch(button) {
        case GY_CONTROLLER_BUT_DPAD_UP:     return 0;
        case GY_CONTROLLER_BUT_DPAD_DOWN:   return 1;
        case GY_CONTROLLER_BUT_DPAD_LEFT:   return 2;
        case GY_CONTROLLER_BUT_DPAD_RIGHT:  return 3;
        default:                            return 0; //Don't be a dumbass, because I ain't gonna save you.
        }
    }
    inline GY_BUTTON_STATE VirtualDPad::getButtonState(const GY_CONTROLLER_BUTTON button) const { return (GY_BUTTON_STATE)_dpad[_toIndex(button)]; }
    inline bool VirtualDPad::isPressed(const GY_CONTROLLER_BUTTON button) const { return (_dpad[_toIndex(button)] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool VirtualDPad::isTapped(const GY_CONTROLLER_BUTTON button) const { return _dpad[_toIndex(button)] == GY_BUT_STATE_TAPPED; }

    //====== CONTROLLER INLINEZ ===========
    inline ControllerEvent::ControllerEvent(EventType type, const Controller* controller):
        InputDeviceEvent(type, controller->getPlayerIndex(),
                         (type == EVENT_CONTROLLER_ATTACHED? InputDeviceEvent::ATTACHED : (type == EVENT_CONTROLLER_REMOVED)? InputDeviceEvent::REMOVED : InputDeviceEvent::STATE_CHANGED ))
    {}


    inline Controller* ControllerEvent::getController(void) const { return _controller; }

    inline unsigned Controller::getPort(void) const { return _port; }
    inline void Controller::setPort(const unsigned port) { _port = port; }
    inline bool Controller::isPressed(const GY_CONTROLLER_BUTTON button) const { return (_gyHandle->buttons[button] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Controller::isTapped(const GY_CONTROLLER_BUTTON button) const { return _gyHandle->buttons[button] == GY_BUT_STATE_TAPPED; }
    inline GY_BUTTON_STATE Controller::getButtonState(const GY_CONTROLLER_BUTTON button) const { return (GY_BUTTON_STATE)_gyHandle->buttons[button]; }
    inline const GYVector2& Controller::getAnalogStick(const GY_CONTROLLER_ANALOG stick) const { return _gyHandle->analogSticks[stick]; }
    inline const GYVector2& Controller::getAnalogDir(const GY_CONTROLLER_ANALOG stick) const { return _analogDirs[stick]; }
    inline const VirtualDPad& Controller::getVirtualDPad(const GY_CONTROLLER_ANALOG stick) const { return _virtualDPads[stick]; }
    inline float Controller::getAnalogMag(const GY_CONTROLLER_ANALOG stick) const { return _analogMags[stick]; }
    inline float Controller::getAnalogAngle(const GY_CONTROLLER_ANALOG stick) const { return _analogAngles[stick]; }
    inline void Controller::setPressed(const GY_CONTROLLER_BUTTON button, const bool pressed) {
        _gyHandle->buttons[button] &= ~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->buttons[button] |= pressed;
    }
    inline void Controller::setAnalogStick(const GY_CONTROLLER_ANALOG stick, const GYVector2 &vec) {
        _gyHandle->analogSticks[stick] = vec;
    }


    //====== VMU INLINEZ =======

    inline VisualMemoryUnit* VisualMemoryUnitEvent::getVMU(void) const { return _vmu; }
    inline VisualMemoryUnitEvent::VisualMemoryUnitEvent(EventType type, VisualMemoryUnit* vmu):
        Event(type),
        _vmu(vmu)
    {}


    //======= POWER SUPPLY INLINEZ ======
    inline PowerSupplyStateChangedEvent::PowerSupplyStateChangedEvent(GY_SYSTEM_POWER_STATE state):
        Event(EVENT_POWER_SUPPLY_STATE_CHANGED), _state(state) {}

    inline GY_SYSTEM_POWER_STATE PowerSupplyStateChangedEvent::getState(void) const {
        return _state;
    }

    inline float PowerSupply::getBatteryLevel(void) const {
        return (float)_gyPowerInfo.batteryPercentageRemaining/100.0f;
    }

    inline unsigned PowerSupply::getBatterySecondsRemaining(void) const {
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



    //======== INPUTSYSTEM INLINEZ =======
    inline Controller* InputSystemBase::getController(const unsigned port) { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_controllers[port]; }
    inline const Controller* InputSystemBase::getController(const unsigned port) const { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); return &_controllers[port]; }
    inline Keyboard* InputSystemBase::getKeyboard(const unsigned port) { return (port == _keyboard.getPlayerIndex())? &_keyboard : nullptr; }
    inline Mouse* InputSystemBase::getMouse(const unsigned port) { return (port == _mouse.getPlayerIndex())? &_mouse : nullptr; }
    inline VisualMemoryUnit* InputSystemBase::getVMU(unsigned port) { assert(port < ELYSIAN_INPUT_VMUS_MAX); return &_vmus[port]; }
    inline RumbleDevice* InputSystemBase::getRumbleDevice(const unsigned port) { assert(port < ELYSIAN_INPUT_RUMBLE_DEVICES_MAX); return &_rumbleDevices[port]; }
    inline PowerSupply* InputSystemBase::getPowerSupply(void) { return &_powerSupply; }
    inline void InputSystemBase::setKeyboardAsControllerPort(const int port) { assert(port < ELYSIAN_INPUT_CONTROLLERS_MAX); _keyboardAsControllerPort = port; }
    inline void InputSystemBase::setDefaultVMUIcon(const char* path) { strcpy(_vmuDefaultIcon, path); }

}



#endif // ELYSIAN_INPUT_SYSTEM_BASE_HPP

