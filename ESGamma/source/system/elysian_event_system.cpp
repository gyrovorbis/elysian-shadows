#include <system/elysian_event_system.hpp>
#include <proxy/elysian_event_proxy.hpp>

namespace elysian {

void EventSystem::removeEventObserver(EventObserver* monitor, EventType type, EventTarget* target) {
    if(type < EVENT_TYPE_COUNT) {
        std::vector<EventObserverEntry>& monitors = _monitors[type];
        EventObserverEntry entry { monitor, target };
        for(size_t i = 0; i < monitors.size(); ++i) {
            if(monitors[i] == entry) {
                monitors.erase(monitors.begin() + i);
                return;
            }
        }
    } else errprintf("EventSystem::removeEventObserver() - Tried to remove monitor for invalid event type: %d", type);
}

void EventSystem::addEventObserverLua(EventObserver* monitor, EventType type, LuaVariant target) {
    EventTarget* targetPtr = nullptr;
    if(!target.isNil()) {
        targetPtr = target.getValue<EventTarget*>();
    }
    addEventObserver(monitor, type, targetPtr);
}

void EventSystem::removeEventObserverLua(EventObserver* monitor, EventType type, LuaVariant target) {
    EventTarget* targetPtr = nullptr;
    if(!target.isNil()) {
        targetPtr = target.getValue<EventTarget*>();
    }
    removeEventObserver(monitor, type, targetPtr);

}


}
