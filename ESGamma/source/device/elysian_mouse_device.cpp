#include <ESGamma/device/elysian_mouse_device.hpp>

namespace elysian {

bool Mouse::isDoubleClicked(GY_MOUSE_BUTTON button) const {
    return false;
}


bool Mouse::pollInput(void) {
    _attached = gyMousePoll(&_gyHandle);
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
        if(_gyHandle->screenPosChanged)     System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_MOVED,     this, GY_MOUSE_BUT_NONE));
        if(_gyHandle->scrollWheelChanged)   System::getEventSys()->broadcastEvent(MouseEvent(EVENT_MOUSE_SCROLLED,  this, GY_MOUSE_BUT_NONE));
    }
    return _attached;
}


}
