#include <level/elysian_event.hpp>

namespace elysian {

EVENT_TARGET_TYPE EventTarget::getEventTargetType(void) const {
    return EVENT_TARGET_INVALID;
}

void EventTarget::onReceiveEvent(Event *event) {}

void EventObserver::onObserveEvent(Event *event, EventTarget *receiver) {}

}
