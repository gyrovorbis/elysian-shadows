#include <level/elysian_event.hpp>

namespace elysian {

EVENT_RECEIVER_TYPE EventReceiver::getEventReceiverType(void) const {
    return EVENT_RECEIVER_INVALID;
}

void EventReceiver::onReceiveEvent(Event *event) {}

void EventObserver::onObserveEvent(Event *event, EventReceiver *receiver) {}

}
