#ifndef ELYSIAN_INPUT_SYSTEM_BASE_HPP
#define ELYSIAN_INPUT_SYSTEM_BASE_HPP

#define ELYSIAN_INPUT_CONTROLLERS_MAX                    4
#define ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT    0

#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN    0.3f
#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MAX    0.7f

#include <system/elysian_system.hpp>
#include <gyro_input_api.h>

namespace elysian {

#if 0
    class TouchScreen {

    };
#endif

    class Keyboard {
    private:
        GYKeyboard*         _gyHandle   = nullptr;
        bool                _attached   = false;

    public:
        GY_BUTTON_STATE     getButtonState(const GY_KEY key) const;
        bool                isPressed(const GY_KEY key) const;
        void                setPressed(const GY_KEY key, const bool pressed) const;
        bool                isTapped(const GY_KEY key) const;
        bool                isAttached(void) const;

        bool                pollInput(void);
    };


    class Mouse {
    private:
        GYMouse*            _gyHandle = nullptr;
        bool                _attached = false;

    public:
        GY_BUTTON_STATE     getButtonState(const GY_MOUSE_BUTTON button) const;
        const GYVector2&    getScreenPos(void) const;
        const GYVector2&    getScrollWheel(void) const;
        bool                isPressed(const GY_MOUSE_BUTTON button) const;
        void                setPressed(const GY_MOUSE_BUTTON button, const bool pressed) const;
        bool                isTapped(const GY_MOUSE_BUTTON button) const;
        bool                isAttached(void) const;

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

    class Controller {
    private:
        GYVector2           _analogDirs[GY_CONTROLLER_ANALOG_COUNT]     = { { 0.0f, 0.0f } };
        VirtualDPad         _virtualDPads[GY_CONTROLLER_ANALOG_COUNT];
        float               _analogMags[GY_CONTROLLER_ANALOG_COUNT]     = { 0.0f };
        float               _analogAngles[GY_CONTROLLER_ANALOG_COUNT]   = { 0.0f };
        GYController*       _gyHandle                                   = nullptr;
        unsigned            _port                                       = 0;
        bool                _attached                                   = false;

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
        bool                isAttached(void) const;


        //void setVmuIcon();
        //void rumble();
        //bool hasRumble();
        //bool hasVmu();
        //bool hasRAnalog();
        //bool hasLAnalog();
        //bool isAttached();
        //registerButtonCallback

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

        static bool     _initialized;
        static bool     _managedExternally;

        Controller      _controllers[ELYSIAN_INPUT_CONTROLLERS_MAX];
        Keyboard        _keyboard;
        Mouse           _mouse;

        int             _keyboardAsControllerPort = ELYSIAN_INPUT_MAP_KEYBOARD_TO_CONTROLLER_PORT;


        void            _onControllerAttached(const unsigned slot);
        void            _onControllerRemoved(const unsigned slot);
        void            _onKeyboardAttached(void);
        void            _onKeyboardRemoved(void);
        void            _onMouseAttached(void);
        void            _onMouseRemoved(void);

        void            _mapKeyboardToController(const int port);

    public:

                        InputSystemBase(void);

        //allow external software (ESTk) to initialize input outside the engine with custom parameters
        static bool     initialize(const GYInputInitParams*const params=nullptr);
        static bool     uninitialize(void);


        void            update(void);
        void            dumpStats(void);

        Controller*     getController(const unsigned port);
        Keyboard*       getKeyboard(void);
        Mouse*          getMouse(void);


        void            setKeyboardAsControllerPort(const int port);


    };





    //====== KEYBOARD INLINEZ ===========
    inline bool Keyboard::isAttached(void) const { return _attached; }
    inline GY_BUTTON_STATE Keyboard::getButtonState(const GY_KEY key) const { return (GY_BUTTON_STATE)_gyHandle->keys[key]; }
    inline bool Keyboard::isPressed(const GY_KEY key) const { return (_gyHandle->keys[key] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Keyboard::isTapped(const GY_KEY key) const { return _gyHandle->keys[key] == GY_BUT_STATE_TAPPED; }
    inline void Keyboard::setPressed(const GY_KEY key, const bool pressed) const {
        _gyHandle->keys[key] &= ~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->keys[key] |= pressed;
    }

    //====== MOUSE INLINEZ ===========
    inline bool Mouse::isAttached(void) const { return _attached; }
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
    inline bool Controller::isAttached(void) const { return _attached; }
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

    //======== INPUTSYSTEM INLINEZ =======
    inline Controller* InputSystemBase::getController(const unsigned port) { return &_controllers[port]; }
    inline Keyboard* InputSystemBase::getKeyboard(void) { return &_keyboard; }
    inline Mouse* InputSystemBase::getMouse(void) { return &_mouse; }
    inline void InputSystemBase::setKeyboardAsControllerPort(const int port) { _keyboardAsControllerPort = port; }

}



#endif // ELYSIAN_INPUT_SYSTEM_BASE_HPP

