#ifndef ELYSIAN_CONTROLLER_DEVICE_HPP
#define ELYSIAN_CONTROLLER_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <libGyro/gyro_input_api.h>

#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MIN             0.3f
#define ELYSIAN_INPUT_CONTROLLER_ANALOG_MAX             0.7f

namespace elysian {
    class Controller;
    class VirtualDPad;

    //Base event of all Controller events
    class ControllerEvent: public InputEvent {
    protected:
        const Controller*   _controller     = nullptr;
    public:
                            ControllerEvent(EventType type, const Controller* controller);
        const Controller*   getController(void) const;
    };


    class ControllerButtonEvent: public ControllerEvent {
    public:
        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(ControllerButtonEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_CONTROLLER_BUTTON_PRESSED,   onControllerButtonPressEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_CONTROLLER_BUTTON_RELEASED,  onControllerButtonReleaseEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END
    private:

        GY_CONTROLLER_BUTTON    _button         = GY_CONTROLLER_BUT_COUNT;
    public:
                                ControllerButtonEvent(EventType type, const Controller* controller, GY_CONTROLLER_BUTTON button);

        GY_CONTROLLER_BUTTON    getButton(void) const;
        GY_BUTTON_STATE         getButtonState(void) const;
        bool                    isPressed(void) const;
        bool                    isTapped(void) const;

    };

    class ControllerAnalogEvent: public ControllerEvent {
    public:
        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(ControllerAnalogEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_CONTROLLER_ANALOG_MOVED,     onControllerAnalogEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END
    private:
        GY_CONTROLLER_ANALOG    _analogStick    = GY_CONTROLLER_ANALOG_COUNT;

    public:
                                ControllerAnalogEvent(EventType type, const Controller* controller, GY_CONTROLLER_ANALOG analogStick);
        GY_CONTROLLER_ANALOG    getAnalogStickType(void) const;
        const GYVector2&        getAnalogStick(void) const;
        const GYVector2&        getAnalogDir(void) const;
        float                   getAnalogMag(void) const;
        const VirtualDPad&      getVirtualDPad(void) const;
    };

    class ControllerTriggerEvent: public ControllerEvent {
    public:
        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(ControllerTriggerEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_CONTROLLER_TRIGGER_MOVED,    onControllerTriggerEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END
    private:
        GY_CONTROLLER_TRIGGER   _trigger        = GY_CONTROLLER_TRIGGER_COUNT;
    public:
                                ControllerTriggerEvent(EventType type, const Controller* controller, GY_CONTROLLER_TRIGGER trigger);
        GY_CONTROLLER_TRIGGER   getTrigger(void) const;
        float                   getTriggerPressure(void) const;

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


    class Controller: public PlayerIODevice {
    private:
        GYVector2           _analogDirs[GY_CONTROLLER_ANALOG_COUNT]     = { { 0.0f, 0.0f } };
        VirtualDPad         _virtualDPads[GY_CONTROLLER_ANALOG_COUNT];
        float               _analogMags[GY_CONTROLLER_ANALOG_COUNT]     = { 0.0f };
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
        const VirtualDPad&  getVirtualDPad(const GY_CONTROLLER_ANALOG stick) const;
        float               getTriggerPressure(const GY_CONTROLLER_TRIGGER trigger) const;

        GY_BUTTON_STATE     getButtonState(const GY_CONTROLLER_BUTTON button) const;
        bool                isPressed(const GY_CONTROLLER_BUTTON button) const;
        void                setPressed(const GY_CONTROLLER_BUTTON button, const bool pressed);
        bool                isTapped(const GY_CONTROLLER_BUTTON button) const;

        unsigned            getPort(void) const;
        void                setPort(const unsigned port);

    };




    //====== CONTROLLER INLINEZ ===========

    //ControllerEvent (base)
    inline ControllerEvent::ControllerEvent(EventType type, const Controller* controller):
        InputEvent(type, (int)controller->getPlayerIndex()),
        _controller(controller)
    {}
    inline const Controller* ControllerEvent::getController(void) const { return _controller; }


    //ControllerButtonEvent
    inline ControllerButtonEvent::ControllerButtonEvent(EventType type, const Controller* controller, GY_CONTROLLER_BUTTON button):
        ControllerEvent(type, controller),
        _button(button)
    {
        assert(_controller && _button < GY_CONTROLLER_BUT_COUNT);
    }
    inline GY_CONTROLLER_BUTTON ControllerButtonEvent::getButton(void) const { return _button; }
    inline GY_BUTTON_STATE ControllerButtonEvent::getButtonState(void) const { return _controller->getButtonState(_button); }
    inline bool ControllerButtonEvent::isPressed(void) const { return _controller->isPressed(_button); }
    inline bool ControllerButtonEvent::isTapped(void) const { return _controller->isTapped(_button); }

    //ControllerAnalogEvent
    inline ControllerAnalogEvent::ControllerAnalogEvent(EventType type, const Controller* controller, GY_CONTROLLER_ANALOG analogStick):
        ControllerEvent(type, controller),
        _analogStick(analogStick)
    {
        assert(_controller && _analogStick < GY_CONTROLLER_ANALOG_COUNT);
    }
    inline GY_CONTROLLER_ANALOG ControllerAnalogEvent::getAnalogStickType(void) const { return _analogStick; }
    inline const GYVector2& ControllerAnalogEvent::getAnalogStick(void) const { return _controller->getAnalogStick(_analogStick); }
    inline const GYVector2& ControllerAnalogEvent::getAnalogDir(void) const { return _controller->getAnalogDir(_analogStick); }
    inline float ControllerAnalogEvent::getAnalogMag(void) const { return _controller->getAnalogMag(_analogStick); }
    inline const VirtualDPad& ControllerAnalogEvent::getVirtualDPad(void) const { return _controller->getVirtualDPad(_analogStick); }

    //ControllerTriggerEvent
    inline ControllerTriggerEvent::ControllerTriggerEvent(EventType type, const Controller* controller, GY_CONTROLLER_TRIGGER trigger):
        ControllerEvent(type, controller),
        _trigger(trigger)
    {
        assert(_controller && _trigger < GY_CONTROLLER_TRIGGER_COUNT);
    }
    inline GY_CONTROLLER_TRIGGER ControllerTriggerEvent::getTrigger(void) const { return _trigger; }
    inline float ControllerTriggerEvent::getTriggerPressure(void) const { return _controller->getTriggerPressure(_trigger); }

    //VirtualDpad
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

    //Controller
    inline unsigned Controller::getPort(void) const { return _port; }
    inline void Controller::setPort(const unsigned port) { _port = port; }
    inline bool Controller::isPressed(const GY_CONTROLLER_BUTTON button) const { return (_gyHandle->buttons[button] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Controller::isTapped(const GY_CONTROLLER_BUTTON button) const { return _gyHandle->buttons[button] == GY_BUT_STATE_TAPPED; }
    inline GY_BUTTON_STATE Controller::getButtonState(const GY_CONTROLLER_BUTTON button) const { return (GY_BUTTON_STATE)_gyHandle->buttons[button]; }
    inline const GYVector2& Controller::getAnalogStick(const GY_CONTROLLER_ANALOG stick) const { return _gyHandle->analogSticks[stick]; }
    inline const GYVector2& Controller::getAnalogDir(const GY_CONTROLLER_ANALOG stick) const { return _analogDirs[stick]; }
    inline const VirtualDPad& Controller::getVirtualDPad(const GY_CONTROLLER_ANALOG stick) const { return _virtualDPads[stick]; }
    inline float Controller::getAnalogMag(const GY_CONTROLLER_ANALOG stick) const { return _analogMags[stick]; }
    inline float Controller::getTriggerPressure(const GY_CONTROLLER_TRIGGER trigger) const { return 0.0f; }
    inline void Controller::setPressed(const GY_CONTROLLER_BUTTON button, const bool pressed) {
        _gyHandle->buttons[button] &= ~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->buttons[button] |= pressed;
    }
    inline void Controller::setAnalogStick(const GY_CONTROLLER_ANALOG stick, const GYVector2 &vec) {
        _gyHandle->analogSticks[stick] = vec;
    }


}

#endif // ELYSIAN_CONTROLLER_DEVICE_HPP
