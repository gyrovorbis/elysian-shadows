/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/device/elysian_keyboard_device.hpp>

namespace elysian {
    bool Keyboard::pollInput(void) {
        _attached = gyKeyboardPoll((unsigned)getPlayerIndex(), &_gyHandle);
        if(_attached && _gyHandle->changed) {
            for(unsigned k = 0; k < GY_KEY_COUNT; ++k) {
                const GY_KEY key = static_cast<GY_KEY>(k);
                if(isPressed(key) != wasPressed(key)) {
                    System::getEventSys()->broadcastEvent(
                                KeyEvent(isPressed(key)? EVENT_KEYBOARD_KEY_PRESSED : EVENT_KEYBOARD_KEY_RELEASED,
                                         this,
                                         key));
                }
            }
        }
        return _attached;
    }

}
