#ifndef ELYSIAN_MOUSE_DEVICE_HPP
#define ELYSIAN_MOUSE_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <libGyro/gyro_input_api.h>

#define ELYSIAN_MOUSE_BUTTON_DOUBLE_CLICK_TIMEOUT_INTERVAL_SECS    0.1f

namespace elysian {
    class Mouse;

    class MouseBaseEvent: public InputEvent {
    protected:
        const Mouse*    _mouse = nullptr;
    public:
                        MouseBaseEvent(EventType type, const Mouse* mouse);
        const Mouse*    getMouse(void) const;
    };

    class MouseEvent: public MouseBaseEvent {
    public:
        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(MouseEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_MOUSE_BUTTON_PRESSED,   onMousePressEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_MOUSE_BUTTON_RELEASED,  onMouseReleaseEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_MOUSE_MOVED,            onMouseMoveEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END

    private:
        GY_MOUSE_BUTTON     _button;
    public:
                            MouseEvent(EventType type, const Mouse* mouse, GY_MOUSE_BUTTON button=GY_MOUSE_BUT_NONE);

        const GYVector2&    getScreenPos(void) const;
        GY_MOUSE_BUTTON     getButton(void) const;
        GY_BUTTON_STATE     getButtonState(void) const;
        bool                isPressed(void) const;
        bool                isTapped(void) const;
        bool                isDoubleClicked(void) const;
    };

    class MouseWheelEvent: public MouseBaseEvent {
    public:

        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(MouseWheelEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_MOUSE_SCROLLED,         onMouseWheelEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END

    public:
                            MouseWheelEvent(EventType eventType, const Mouse* mouse);
        const GYVector2&    getScrollWheel(void) const;

    };

    class Mouse: public PlayerIODevice {
    private:
        static float        _doubleClickTimeoutIntervalSecs;
        uint64_t            _lastTapTicks[GY_MOUSE_BUT_COUNT]           = { 0 };
        bool                _buttonsDoubleClicked[GY_MOUSE_BUT_COUNT]   = { false };
        GYMouse*            _gyHandle                                   = nullptr;

        void                _updateDerivedData(uint64_t ticks);

    public:

        GY_BUTTON_STATE     getButtonState(const GY_MOUSE_BUTTON button) const;
        const GYVector2&    getScreenPos(void) const;
        const GYVector2&    getScrollWheel(void) const;
        bool                isPressed(const GY_MOUSE_BUTTON button) const;
        bool                wasPressed(const GY_MOUSE_BUTTON button) const;
        void                setPressed(const GY_MOUSE_BUTTON button, const bool pressed) const;
        bool                isTapped(const GY_MOUSE_BUTTON button) const;
        //IMPLEMENT ME, BITCHES!!!
        bool                isDoubleClicked(const GY_MOUSE_BUTTON button) const;

        bool                pollInput(uint64_t ticks);

        static void         setDoubleClickTimeoutIntervalSecs(float secs);
        static float        getDoubleClickTimeoutIntervalSecs(void);
    };



    //====== MOUSE INLINEZ ===========
    inline MouseBaseEvent::MouseBaseEvent(EventType type, const Mouse* mouse):
        InputEvent(type, (int)mouse->getPlayerIndex()),
        _mouse(mouse)
    {
        assert(mouse);
    }
    inline const Mouse* MouseBaseEvent::getMouse(void) const { return _mouse; }

    inline MouseEvent::MouseEvent(EventType type, const Mouse* mouse, GY_MOUSE_BUTTON button):
        MouseBaseEvent(type, mouse),
        _button(button)
    {}
    inline GY_BUTTON_STATE MouseEvent::getButtonState(void) const { return (_button != GY_MOUSE_BUT_NONE)? _mouse->getButtonState(_button) : GY_BUT_STATE_UP; }
    inline bool MouseEvent::isPressed(void) const { return (_button != GY_MOUSE_BUT_NONE)? _mouse->isPressed(_button) : false; }
    inline bool MouseEvent::isTapped(void) const { return (_button != GY_MOUSE_BUT_NONE)? _mouse->isTapped(_button) : false; }
    inline bool MouseEvent::isDoubleClicked(void) const { return (_button != GY_MOUSE_BUT_NONE)? _mouse->isDoubleClicked(_button) : false; }
    inline GY_MOUSE_BUTTON MouseEvent::getButton(void) const { return _button; }
    inline const GYVector2& MouseEvent::getScreenPos(void) const { return _mouse->getScreenPos(); }

    inline MouseWheelEvent::MouseWheelEvent(EventType eventType, const Mouse* mouse):
        MouseBaseEvent(eventType, mouse) {}
    inline const GYVector2& MouseWheelEvent::getScrollWheel(void) const { return _mouse->getScrollWheel(); }

    inline bool Mouse::isPressed(const GY_MOUSE_BUTTON button) const { assert(button < GY_MOUSE_BUT_COUNT); return (_gyHandle->buttons[button] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Mouse::wasPressed(const GY_MOUSE_BUTTON button) const { assert(button < GY_MOUSE_BUT_COUNT); return (_gyHandle->buttons[button] & GY_BUT_FLAGS_WAS_PRESSED); }
    inline bool Mouse::isTapped(const GY_MOUSE_BUTTON button) const { assert(button < GY_MOUSE_BUT_COUNT); return _gyHandle->buttons[button] == GY_BUT_STATE_TAPPED; }
    inline GY_BUTTON_STATE Mouse::getButtonState(const GY_MOUSE_BUTTON button) const { assert(button < GY_MOUSE_BUT_COUNT); return (GY_BUTTON_STATE)_gyHandle->buttons[button]; }
    inline const GYVector2& Mouse::getScrollWheel(void) const { return _gyHandle->scrollWheel; }
    inline const GYVector2& Mouse::getScreenPos(void) const { return _gyHandle->screenPos; }
    inline bool Mouse::isDoubleClicked(GY_MOUSE_BUTTON button) const { assert(button < GY_MOUSE_BUT_COUNT); return _buttonsDoubleClicked[button]; }
    inline void Mouse::setPressed(const GY_MOUSE_BUTTON button, const bool pressed) const {
        _gyHandle->buttons[button] &= (unsigned)~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->buttons[button] |= (unsigned)pressed;
    }
    inline void Mouse::setDoubleClickTimeoutIntervalSecs(float secs) { _doubleClickTimeoutIntervalSecs = secs; }
    inline float Mouse::getDoubleClickTimeoutIntervalSecs(void) { return _doubleClickTimeoutIntervalSecs; }
}

#endif // ELYSIAN_MOUSE_DEVICE_HPP
