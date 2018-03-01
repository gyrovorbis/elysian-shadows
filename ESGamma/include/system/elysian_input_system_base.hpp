#ifndef ELYSIAN_INPUT_SYSTEM_BASE_HPP
#define ELYSIAN_INPUT_SYSTEM_BASE_HPP

#define ELYSIAN_INPUT_PLAYERS_MAX                       4
#define ELYSIAN_INPUT_CONTROLLERS_MAX                   ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_VMUS_MAX                          ELYSIAN_INPUT_PLAYERS_MAX
#define ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT    0

#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN    0.3f
#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MAX    0.7f

#include <system/elysian_system.hpp>
#include <gyro_input_api.h>
#include <gyro_rumble_api.h>
#include <gyro_visual_memory_api.h>
#include <system/elysian_event_system.hpp>

namespace elysian {
    class Mouse;
    class Controller;

    class PlayerIODevice    {
    protected:
        bool        _attached;
        unsigned    _playerIndex;

    public:
        bool        isAttached(void) const;
        unsigned    getPlayerIndex(void) const;
        void        setPlayerIndex(unsigned index);

    };

    class TouchDevice: public PlayerIODevice {

    };

    class KeyboardEventObserver: public EventObserver {
    public:
                        KeyboardEventObserver(void);
        virtual void    onObserveEvent(Event* event, EventTarget* target) override;
        virtual void    onKeyboardEvent(const Keyboard& keybd);
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

    class ControllerEvent: public Event {
    private:
        Controller*         _controller    = nullptr;
        int                  _slot          = 1;
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


        //void setVmuIcon();
        //void rumble();
        //bool hasRumble();
        //bool hasVmu();
        //bool hasRAnalog();
        //bool hasLAnalog();
        //bool isAttached();
        //registerButtonCallback

    };
#if 0
    bool                gyRumbleInit(struct GYRumbleInitParams* params);
    bool                gyRumbleUninit(void);


    struct GYRumble*    gyRumblePoll(unsigned slot);
    void                gyRumbleDeviceDump(const struct GYRumble* dev);
    void                gyRumbleDeviceEffectsDump(const struct GYRumble* dev);

    bool                gyRumblePlay(struct GYRumble* dev, float intensity, float duration);
    bool                gyRumbleStop(struct GYRumble* dev);

    bool                gyRumbleEffectInit(struct GYRumbleEffect* effect, const GYRumbleEffectInitParams* params);
    bool                gyRumbleEffectPlay(struct GYRumble* dev, const struct GYRumbleEffect* effect);
    int                 gyRumbleEffectStatus(const struct GYRumble* dev, const struct GYRumbleEffect* effect); // <0 - error, 0 - stopped, 1 - playing
#endif

#if 0
    class RumbleDevice {
    private:
        GYRumble*   _gyHandle = nullptr;
        bool        _attached = false;
    public:

        bool        pollInput(void);
        bool        isAttached(void) const;
        bool        play(float intensity, float duration);
        bool        playEffect(const RumbleEffect* effect);
        //EFFECT_STATUS getEffectStatus(const RumbleEffect* effect);
        bool        stop();

        void        dumpInfo(void) const;
        void        dumpEffects(void) const;
    };
#endif

    class VisualMemoryUnit;

    class VisualMemoryUnitEvent: public Event {
     private:
        VisualMemoryUnit* _vmu;
    public:
        VisualMemoryUnit* getVMU(void) const;
                            VisualMemoryUnitEvent(EventType type, VisualMemoryUnit* vmu);
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
        Keyboard            _keyboard;
        Mouse               _mouse;

        int                 _keyboardAsControllerPort = ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT;


        void                _onControllerAttached(const unsigned slot);
        void                _onControllerRemoved(const unsigned slot);
        void                _onKeyboardAttached(void);
        void                _onKeyboardRemoved(void);
        void                _onMouseAttached(void);
        void                _onMouseRemoved(void);
        void                _onVMUAttached(unsigned slot);
        void                _onVMURemoved(unsigned slot);

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
        Keyboard*           getKeyboard(void);
        Mouse*              getMouse(void);
        VisualMemoryUnit*   getVMU(const unsigned port);

        void                setKeyboardAsControllerPort(const int port);


    };





    //===== PLAYER I/O DEVICE ==========
    inline bool PlayerIODevice::isAttached(void) const { return _attached; }
    inline void PlayerIODevice::setPlayerIndex(unsigned index) { _playerIndex = index; }
    inline unsigned PlayerIODevice::getPlayerIndex(void) const { return _playerIndex; }


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
        Event(type),
        _slot(controller->getPort())
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



    //======== INPUTSYSTEM INLINEZ =======
    inline Controller* InputSystemBase::getController(const unsigned port) { return &_controllers[port]; }
    inline const Controller* InputSystemBase::getController(const unsigned port) const { return &_controllers[port]; }
    inline Keyboard* InputSystemBase::getKeyboard(void) { return &_keyboard; }
    inline Mouse* InputSystemBase::getMouse(void) { return &_mouse; }
    inline VisualMemoryUnit* InputSystemBase::getVMU(unsigned port) { return &_vmus[port]; }
    inline void InputSystemBase::setKeyboardAsControllerPort(const int port) { _keyboardAsControllerPort = port; }

}



#endif // ELYSIAN_INPUT_SYSTEM_BASE_HPP

