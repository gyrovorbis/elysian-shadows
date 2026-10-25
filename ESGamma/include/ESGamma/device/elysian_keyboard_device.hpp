/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_KEYBOARD_DEVICE_HPP
#define ELYSIAN_KEYBOARD_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <libGyro/gyro_input_api.h>

namespace elysian {
    class KeyEvent: public InputEvent {
    public:
        ELYSIAN_EVENT_HANDLER_LIST_BEGIN(KeyEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_KEYBOARD_KEY_PRESSED, onKeyPressEvent)
            ELYSIAN_EVENT_HANDLER(EVENT_KEYBOARD_KEY_RELEASED, onKeyReleaseEvent)
        ELYSIAN_EVENT_HANDLER_LIST_END

    private:
        Keyboard*           _keybd;
        GY_KEY              _key;
    public:
                            KeyEvent(EventType eventType, Keyboard* keybd, GY_KEY key);
        Keyboard*           getKeyboard(void) const;

        bool                isPressed(void) const;
        bool                wasPressed(void) const;
        bool                isTapped(void) const;
        GY_BUTTON_STATE     getState(void) const;
        GY_KEY              getKey(void) const;
    };

    class KeyEventObserver: public EventSink<KeyEventObserver, KeyEvent> {
    private:
        unsigned        _playerIndex = 0;
    public:
                        KeyEventObserver(unsigned playerIndex=0);
        virtual void    onKeyPressEvent(KeyEvent* event);
        virtual void    onKeyReleaseEvent(KeyEvent* event);
    };


    class Keyboard: public PlayerIODevice {
    private:
        GYKeyboard*         _gyHandle   = nullptr;

    public:
        GY_BUTTON_STATE     getButtonState(const GY_KEY key) const;
        bool                isPressed(const GY_KEY key) const;
        bool                wasPressed(const GY_KEY key) const;
        void                setPressed(const GY_KEY key, const bool pressed) const;
        bool                isTapped(const GY_KEY key) const;

        bool                pollInput(void);
    };



    //====== KEYBOARD INLINEZ ===========
    inline GY_BUTTON_STATE Keyboard::getButtonState(const GY_KEY key) const { return (GY_BUTTON_STATE)_gyHandle->keys[key]; }
    inline bool Keyboard::isPressed(const GY_KEY key) const { return (_gyHandle->keys[key] & GY_BUT_FLAGS_IS_PRESSED); }
    inline bool Keyboard::wasPressed(const GY_KEY key) const { return (_gyHandle->keys[key] & GY_BUT_FLAGS_WAS_PRESSED); }
    inline bool Keyboard::isTapped(const GY_KEY key) const { return _gyHandle->keys[key] == GY_BUT_STATE_TAPPED; }
    inline void Keyboard::setPressed(const GY_KEY key, const bool pressed) const {
        _gyHandle->keys[key] &= ~GY_BUT_FLAGS_IS_PRESSED;
        _gyHandle->keys[key] |= pressed;
    }
    inline KeyEvent::KeyEvent(EventType eventType, Keyboard* keybd, GY_KEY key):
        InputEvent(eventType, (int)keybd->getPlayerIndex()),
        _keybd(keybd),
        _key(key)
    {}
    inline Keyboard* KeyEvent::getKeyboard(void) const { return _keybd; }
    inline bool KeyEvent::isPressed(void) const { return _keybd->isPressed(_key); }
    inline bool KeyEvent::wasPressed(void) const { return _keybd->wasPressed(_key); }
    inline bool KeyEvent::isTapped(void) const { return _keybd->isTapped(_key); }
    inline auto KeyEvent::getState(void) const -> GY_BUTTON_STATE { return _keybd->getButtonState(_key); }
    inline GY_KEY KeyEvent::getKey(void) const { return _key; }


}

#endif // ELYSIAN_KEYBOARD_DEVICE_HPP
