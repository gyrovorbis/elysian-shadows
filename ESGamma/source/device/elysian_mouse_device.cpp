#include <ESGamma/device/elysian_mouse_device.hpp>
#include <ESGamma/crtp/elysian_timer_system.hpp>

namespace elysian {

float Mouse::_doubleClickTimeoutIntervalSecs = ELYSIAN_MOUSE_BUTTON_DOUBLE_CLICK_TIMEOUT_INTERVAL_SECS;

bool Mouse::pollInput(uint64_t ticks) {
    _attached = gyMousePoll((unsigned)getPlayerIndex(), &_gyHandle);
    if(_attached && _gyHandle) {
        if(_gyHandle->buttonsChanged) {
            for(int b = 0; b < GY_MOUSE_BUT_COUNT; ++b) {
                const GY_MOUSE_BUTTON mouseButton = static_cast<GY_MOUSE_BUTTON>(b);
                if(isPressed(mouseButton) != wasPressed(mouseButton)) {
                    System::getEventSys()->broadcastEvent(
                                MouseEvent(isPressed(mouseButton)? EVENT_MOUSE_BUTTON_PRESSED : EVENT_MOUSE_BUTTON_RELEASED,
                                         this,
                                         mouseButton));
                }
            }
        }
        if(_gyHandle->screenPosChanged)     System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_MOVED, this, GY_MOUSE_BUT_NONE));
        if(_gyHandle->scrollWheelChanged)   System::getEventSys()->broadcastEvent(MouseWheelEvent(EVENT_MOUSE_SCROLLED,  this));

        _updateDerivedData(ticks);
    }
    return _attached;
}


void Mouse::_updateDerivedData(uint64_t ticks) {
   //Detect double-clicks
    for(unsigned b = 0; b < GY_MOUSE_BUT_COUNT; ++b) {
        if(isTapped((GY_MOUSE_BUTTON)b)) {
            //If this is the second tap AND it happened within the timeout interval
            if(_lastTapTicks[b] &&
                    (TimerSystem::ticksToSecs(ticks - _lastTapTicks[b]) < _doubleClickTimeoutIntervalSecs))
            {
                _buttonsDoubleClicked[b] = true;
                _lastTapTicks[b] = 0;

            } else { //This is the first tap and/or we were to fuckin' slow
                _lastTapTicks[b] = ticks;
            }
        }
    }
}

}
