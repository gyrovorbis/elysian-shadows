#ifndef ELYSIAN_EVENT_HPP
#define ELYSIAN_EVENT_HPP

#include <stdint.h>
#include <utility/elysian_cpp_lua_callback.hpp>

namespace elysian {

    class EventSystem;

    enum EVENT_TYPE {
        EVENT_KEYBOARD_ATTACHED,
        EVENT_KEYBOARD_REMOVED,
        EVENT_MOUSE_ATTACHED,
        EVENT_MOUSE_REMOVED,
        EVENT_CONTROLLER_ATTACHED,
        EVENT_CONTROLLER_REMOVED,
        EVENT_KEYBOARD_KEY,
        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_BUTTON,
        EVENT_MOUSE_WHEEL,
        EVENT_COLLISION_ENTER,
        EVENT_COLLISION_EXIT,
        EVENT_ENTITY_ACTIVE,
        EVENT_AREA_ENTER,
        EVENT_AREA_EXIT,
        EVENT_AREA_CHANGED,
        EVENT_PARTY_SLOT,
        EVENT_PARTY_ACTIVE,
        EVENT_WIDGET_ACTIVE,
        EVENT_WIDGET_FOCUS,
        EVENT_WIDGET_MENU_CANCEL,
        EVENT_WIDGET_MENU_CURRENT,
        EVENT_WIDGET_MENU_SELECTION,
        EVENT_LUA,
        EVENT_TYPE_COUNT,
        EVENT_TYPE_INVALID=EVENT_TYPE_COUNT
    };

    enum EVENT_RECEIVER_TYPE {
        EVENT_RECEIVER_INVALID,
        EVENT_RECEIVER_ENTITY,
        EVENT_RECEIVER_WIDGET,
        EVENT_RECEIVER_BEHAVIOR,
        EVENT_RECEIVER_COUNT
    };

    enum EVENT_STATE {
        EVENT_STATE_PENDING,
        EVENT_STATE_ACCEPTED,
        EVENT_STATE_IGNORED
    };

    using EventType = uint32_t;

    class Event {
    public:

                        Event(EventType type);

        EventType       getEventType(void) const;
        EVENT_STATE     getEventState(void) const;
    private:
        EventType   _eventType  = EVENT_TYPE_INVALID;
        EVENT_STATE _eventState = EVENT_STATE_PENDING;

    };

    //eventrecv_cast<TYPE> which is a templated static cast that looks at the getReceiverType to ensure it's typesafe!!!!
    //I guess we have to handle when these fuckers get deleted... or does EventREceiver/EventMonitor's destructors also emit
    //some kinda signal that tells this bitch?

    class EventReceiver {
        friend class EventSystem;
    public:
        using Callback = CppLuaCallback<int, void*>;//EVENT FIXME!!!!!!!!

                        EventReceiver(void) = default;
                        EventReceiver(Callback callback);
                        ~EventReceiver(void) = default; //remove any references from EventSystem!
        virtual void    onReceiveEvent(Event* event);
        virtual auto    getEventReceiverType(void) const -> EVENT_RECEIVER_TYPE;
    protected:
        void            _onReceiveEvent(Event* event);
    private:
        Callback        _callback;
    };


    class EventObserver {
        friend class EventSystem;
    protected:
        void            _onObserveEvent(Event* event, EventReceiver* receiver);
    public:
                        EventObserver(void) = default;
                        ~EventObserver(void) = default; //remove any references from EventSystem!
        virtual void    onObserveEvent(Event* event, EventReceiver* receiver);


    };


}


#endif // ELYSIAN_EVENT_HPP
