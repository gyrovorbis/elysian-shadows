#ifndef ELYSIAN_INPUT_SYSTEM_BASE_HPP
#define ELYSIAN_INPUT_SYSTEM_BASE_HPP

#define ELYSIAN_INPUT_PLAYERS_MAX                       4
#define ELYSIAN_INPUT_KEYBOARDS_MAX                     ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_MICE_MAX                          ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_TOUCH_DEVICES_MAX                 ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_CONTROLLERS_MAX                   ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_VMUS_MAX                          ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_RUMBLE_DEVICES_MAX                ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT   0
#define ELYSIAN_INPUT_DEVICE_ATTACHED_SFX               "/assets/audio/sfx/UI/device_connect.wav"
#define ELYSIAN_INPUT_DEVICE_REMOVED_SFX                "/assets/audio/sfx/UI/device_disconnect.wav"

#include <ESGamma/system/elysian_system.hpp>
#include <libGyro/gyro_input_api.h>
#include <libGyro/gyro_defines.h>

struct GYWav;

namespace elysian {
    class Keyboard;
    class Mouse;
    class Controller;
    class TouchDevice;
    class VisualMemoryUnit;
    class PowerSupply;
    class RumbleDevice;

    class InputPortMappings {
    private:
        bool _mapKeyboardToController        = false;
        bool _mapControllerToMouse           = false;
        bool _mapControllerToTouchDevice     = false;
        bool _mapMouseToTouchDevice          = false;
        bool _mapTouchDeviceToMouse          = false;
    public:

        bool    isKeyboardMappedToController(void) const;
        bool    isControllerMappedToMouse(void) const;
        bool    isControllerMappedToTouchDevice(void) const;
        bool    isMouseMappedToTouchDevice(void) const;
        bool    isTouchDeviceMappedToMouse(void) const;

        void    mapKeyboardToController(const bool enabled=true);
        void    mapControllerToMouse(const bool enabled=true);
        void    mapControllerToTouchDevice(const bool enabled=true);
        void    mapMouseToTouchDevice(const bool enabled=true);
        void    mapTouchDeviceToMouse(const bool enabled=true);
    };

    //TODO: Logic to emulate Mouse via Controller
    //TODO: Logic to emulate TouchEvents via Mouse
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

        char                _vmuDefaultIcon[GYRO_PATH_MAX_SIZE] = { '\0' };
        Controller*         _controllers                        = nullptr;
        VisualMemoryUnit*   _vmus                               = nullptr;
        RumbleDevice*       _rumbleDevices                      = nullptr;
        Keyboard*           _keyboards                          = nullptr;
        Mouse*              _mice                               = nullptr;
        TouchDevice*        _touchDevices                       = nullptr;
        PowerSupply*        _powerSupply                        = nullptr;
        GYWav*              _deviceAttachedSfx                  = nullptr;
        GYWav*              _deviceRemovedSfx                   = nullptr;
        InputPortMappings*  _portMappings                       = nullptr;

        void                _onControllerAttached(const unsigned slot);
        void                _onControllerRemoved(const unsigned slot);
        void                _onKeyboardAttached(const unsigned slot);
        void                _onKeyboardRemoved(const unsigned slot);
        void                _onMouseAttached(const unsigned slot);
        void                _onMouseRemoved(const unsigned slot);
        void                _onTouchDeviceAttached(const unsigned slot);
        void                _onTouchDeviceRemoved(const unsigned slot);
        void                _onVMUAttached(unsigned slot);
        void                _onVMURemoved(unsigned slot);
        void                _onRumbleDeviceAttached(unsigned slot);
        void                _onRumbleDeviceRemoved(unsigned slot);
        void                _onPowerStateChanged(void);

        void                _mapKeyboardToController(const int port);
        void                _mapControllerToMouse(const int port);
        void                _mapControllerToTouchDevice(const int port);
        void                _mapMouseToTouchDevice(const int port);
        void                _mapTouchDeviceToMouse(const int port);

    public:
                            InputSystemBase(void);
                            ~InputSystemBase(void);

        //allow external software (ESTk) to initialize input outside the engine with custom parameters
        static bool         initialize(const GYInputInitParams*const params=nullptr);
        static bool         uninitialize(void);


        void                update(void);
        void                dumpStats(void);

        //Per-Player Devices
        Controller*         getController(const unsigned port) const;
        Keyboard*           getKeyboard(const unsigned port) const;
        Mouse*              getMouse(const unsigned port) const;
        TouchDevice*        getTouchDevice(const unsigned port) const;
        VisualMemoryUnit*   getVMU(const unsigned port) const;
        RumbleDevice*       getRumbleDevice(const unsigned port) const;

        //Global Devices
        PowerSupply*        getPowerSupply(void) const;

        //Device utility functions
        InputPortMappings*  getPortMappings(const unsigned port) const;
        void                setDefaultVMUIcon(const char* path);
        void                broadcastVMUIcon(const char* path);
        void                broadcastVMUBeep(uint8_t pulseWidth, uint8_t dutyCycle);


    };


    //======== INPUTSYSTEM INLINEZ =======
    //InputDevicePortMappings
    inline bool InputPortMappings::isKeyboardMappedToController(void) const { return _mapKeyboardToController; }
    inline bool InputPortMappings::isControllerMappedToMouse(void) const { return _mapControllerToMouse; }
    inline bool InputPortMappings::isControllerMappedToTouchDevice(void) const { return _mapControllerToTouchDevice; }
    inline bool InputPortMappings::isMouseMappedToTouchDevice(void) const { return _mapMouseToTouchDevice; }
    inline bool InputPortMappings::isTouchDeviceMappedToMouse(void) const { return _mapTouchDeviceToMouse; }

    inline void InputPortMappings::mapKeyboardToController(const bool enabled) { _mapKeyboardToController = enabled; }
    inline void InputPortMappings::mapControllerToMouse(const bool enabled) { _mapControllerToMouse = enabled; }
    inline void InputPortMappings::mapControllerToTouchDevice(const bool enabled) { _mapControllerToTouchDevice = enabled; }
    inline void InputPortMappings::mapMouseToTouchDevice(const bool enabled) { _mapMouseToTouchDevice = enabled; }
    inline void InputPortMappings::mapTouchDeviceToMouse(const bool enabled) { _mapTouchDeviceToMouse = enabled; }

    //InputSystemBase
    inline void InputSystemBase::setDefaultVMUIcon(const char* path) { strcpy(_vmuDefaultIcon, path); }
    inline InputPortMappings* InputSystemBase::getPortMappings(const unsigned port) const { assert(port < ELYSIAN_INPUT_PLAYERS_MAX); return &_portMappings[port]; }
}



#endif // ELYSIAN_INPUT_SYSTEM_BASE_HPP

