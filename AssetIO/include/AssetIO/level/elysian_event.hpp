#ifndef ELYSIAN_EVENT_HPP
#define ELYSIAN_EVENT_HPP

#include <stdint.h>
#include <AssetIO//utility/elysian_cpp_lua_callback.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <type_traits>



#define ELYSIAN_LUA_PROXY_FIELD_READ_WRITE(f, g, s) { #f, #g, #s, nullptr, elysian::LuaTableFieldProxy::FLAG_NONE },
#define ELYSIAN_LUA_PROXY_FIELD_READ_ONLY(f, g) { #f, #g, nullptr, nullptr, elysian::LuaTableFieldProxy::FLAG_NONE },

#define ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_FIELD(...) ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(f) { #f, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_BEGIN },
#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END() { nullptr, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_END },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE(f, g, s, c) { #f, #g, #s, #c, LuaTableFieldProxy::FLAG_ARRAY },
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY(f, g, c) { #f, #g, nullptr, #c, LuaTableFieldProxy::FLAG_ARRAY },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD(...) ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY)(__VA_ARGS__)


#define ELYSIAN_EVENT_HANDLER_DECL_BEGIN(...)


#define ELYSIAN_EVENT_HANDLER_LIST_BEGIN(NAME)                      \
    using OUTER_CLASS = NAME;                                       \
    template<typename P>                                            \
    struct Sink {                                                   \
         bool _onEventObserve(Event* event) {                       \
            switch(event->getEventType()) {

#define ELYSIAN_EVENT_HANDLER(EVENT_TYPE, EVENT_HANDLER)            \
            case EVENT_TYPE:                                        \
                static_cast<P*>(this)->                             \
                    EVENT_HANDLER(static_cast<OUTER_CLASS*>(event));\
                return true;

#define ELYSIAN_EVENT_HANDLER_LIST_END                             \
            default: return false;                                 \
            }                                                      \
        }                                                          \
    };


namespace elysian {

    class EventSystem;

    enum EVENT_TYPE {
        EVENT_ENTITY_ACTIVATE,
        EVENT_ENTITY_DEACTIVATE,
        EVENT_ENTITY_DELETED,
        EVENT_ENTITY_COMPONENT_ATTACHED,
        EVENT_ENTITY_COMPONENT_REMOVED,
        EVENT_ENTITY_COMBAT_ENTER,
        EVENT_ENTITY_COMBAT_EXIT,
        EVENT_ENTITY_COMBAT_DEATH,
        EVENT_KEYBOARD_ATTACHED,
        EVENT_KEYBOARD_REMOVED,
        EVENT_KEYBOARD_KEY_PRESSED,
        EVENT_KEYBOARD_KEY_RELEASED,
        EVENT_KEYBOARD_KEY,
        EVENT_MOUSE_ATTACHED,
        EVENT_MOUSE_REMOVED,
        EVENT_MOUSE_MOVED,
        EVENT_MOUSE_BUTTON,
        EVENT_MOUSE_BUTTON_PRESSED,
        EVENT_MOUSE_BUTTON_RELEASED,
        EVENT_MOUSE_SCROLLED,
        EVENT_CONTROLLER_ATTACHED,
        EVENT_CONTROLLER_REMOVED,
        EVENT_CONTROLLER_BUTTON_PRESSED,
        EVENT_CONTROLLER_BUTTON_RELEASED,
        EVENT_CONTROLLER_ANALOG_MOVED,
        EVENT_CONTROLLER_TRIGGER_MOVED,
        EVENT_TOUCH_DEVICE_ATTACHED,
        EVENT_TOUCH_DEVICE_REMOVED,
        EVENT_RUMBLE_DEVICE_ATTACHED,
        EVENT_RUMBLE_DEVICE_REMOVED,
        EVENT_VMU_ATTACHED,
        EVENT_VMU_REMOVED,
        EVENT_POWER_SUPPLY_STATE_CHANGED,
        EVENT_ACCELEROMETER_ATTACHED,
        EVENT_ACCELEROMETER_REMOVED,
        EVENT_GYROSCOPE_ATTACHED,
        EVENT_GYROSCOPE_REMOVED,
        EVENT_MAGNETOMETER_ATTACHED,
        EVENT_MAGNETOMETER_REMOVED,
        EVENT_ALTIMETER_ATTACHED,
        EVENT_ALTIMETER_REMOVED,
        EVENT_PEDOMETER_ATTACHED,
        EVENT_PEDOMETER_REMOVED,
        EVENT_SCALE_ATTACHED,
        EVENT_SCALE_REMOVED,
        EVENT_COLLISION_ENTER,
        EVENT_COLLISION_EXIT,
        EVENT_AREA_ENTER,
        EVENT_AREA_EXIT,
        EVENT_AREA_CHANGED,
        EVENT_GAME_TIME_DAWN,
        EVENT_GAME_TIME_NOON,
        EVENT_GAME_TIME_DUSK,
        EVENT_GAME_TIME_MIDNIGHT,
        EVENT_PARTY_SLOT,
        EVENT_PARTY_ACTIVE,
        EVENT_VIEWPORT_SIZE_CHANGED,
        EVENT_WIDGET_ACTIVE,
        EVENT_WIDGET_FOCUS,
        EVENT_WIDGET_MENU_CANCEL,
        EVENT_WIDGET_MENU_CURRENT,
        EVENT_WIDGET_MENU_SELECTION,
        EVENT_PLAYER_INDEX_CHANGED,
        EVENT_LUA,
        EVENT_TYPE_COUNT,
        EVENT_TYPE_INVALID=EVENT_TYPE_COUNT
    };

    enum EVENT_TARGET_TYPE {
        EVENT_TARGET_INVALID,
        EVENT_TARGET_ENTITY,
        EVENT_TARGET_WIDGET,
        EVENT_TARGET_BEHAVIOR,
        EVENT_TARGET_LUA,
        EVENT_TARGET_COUNT
    };

    enum EVENT_STATE {
        EVENT_STATE_PENDING,
        EVENT_STATE_ACCEPTED,
        EVENT_STATE_IGNORED
    };

    using EventType = uint32_t;


    class Event: public LuaTableProxy<Event> {
    public:
                        Event(EventType type);

        EventType       getEventType(void) const;
        EVENT_STATE     getEventState(void) const;
        void            setEventState(EVENT_STATE state);
    private:
        EventType       _eventType  = EVENT_TYPE_INVALID;
        EVENT_STATE     _eventState = EVENT_STATE_PENDING;

    };

    class LuaEvent: public Event, public LuaTableProxy<LuaEvent, LuaEvent, Event> {
        LuaVariant  _luaVariant;
    public:
                    LuaEvent(void);
                    LuaEvent(LuaVariant v);
        LuaVariant  getVariant(void) const;
        void        setVariant(LuaVariant v);
    };



    //eventrecv_cast<TYPE> which is a templated static cast that looks at the getReceiverType to ensure it's typesafe!!!!
    //I guess we have to handle when these fuckers get deleted... or does EventTarget/EventMonitor's destructors also emit
    //some kinda signal that tells this bitch?

    class EventTarget {
        friend class EventSystem;
    public:
        using Callback = CppLuaCallback<int, void*>;//EVENT FIXME!!!!!!!!

                        EventTarget(void) = default;
                        EventTarget(Callback callback);
                        ~EventTarget(void) = default; //remove any references from EventSystem!
        virtual void    onReceiveEvent(Event* event);
        virtual auto    getEventTargetType(void) const -> EVENT_TARGET_TYPE;
    protected:
        void            _onReceiveEvent(Event* event);
    private:
        Callback        _callback;
    };


    class EventObserver {
        friend class EventSystem;
    protected:
        void            _onObserveEvent(Event* event, EventTarget* target);
    public:
                        EventObserver(void) = default;
                        ~EventObserver(void) = default; //remove any references from EventSystem!
        virtual void    onObserveEvent(Event* event, EventTarget* target);

    };

    template<class CRTP_Derived, class... Events> class EventSinkBase;

    template<class CRTP_Derived,
             class First_Event,
             class... Other_Events>
    class EventSinkBase<CRTP_Derived, First_Event, Other_Events...>:
            public First_Event::template Sink<CRTP_Derived>,
            public EventSinkBase<CRTP_Derived, Other_Events...>
    {
    protected:

        bool _onEventObserve(Event* event) {
            if(!First_Event::template Sink<CRTP_Derived>::_onEventObserve(event)) {
                return EventSinkBase<CRTP_Derived, Other_Events...>::_onEventObserve(event);
            }
            return true;
        }
    };

    template<class CRTP_Derived, class First_Event>
    class EventSinkBase<CRTP_Derived, First_Event>:
        public First_Event::template Sink<CRTP_Derived>
    {
    protected:
        bool _onEventObserve(Event* event) {
            return First_Event::template Sink<CRTP_Derived>::_onEventObserve(event);
        }
    };


    template<typename CRTP_Derived,
             typename First_Event,
             typename... Other_Events>
    class EventSink:
            public EventSinkBase<CRTP_Derived, First_Event, Other_Events...>,
            public EventObserver
    {
    public:

        virtual void onObserveEvent(Event* event, EventTarget* target) override {
            if(!target) EventSinkBase<CRTP_Derived, First_Event, Other_Events...>::_onEventObserve(event);
        }
    };









    //========== INLINEZ ==========

    //----Event----
    inline void Event::setEventState(EVENT_STATE state) { _eventState = state; }
    inline Event::Event(EventType type): _eventType(type) {}
    inline EventType Event::getEventType(void) const { return _eventType; }
    inline auto Event::getEventState(void) const -> EVENT_STATE { return _eventState; }

    //----LuaEvent----
    inline LuaEvent::LuaEvent(void): Event(EVENT_LUA) {}
    inline LuaEvent::LuaEvent(LuaVariant v): Event(EVENT_LUA), _luaVariant(std::move(v)) {}
    inline LuaVariant LuaEvent::getVariant(void) const { return _luaVariant; }
    inline void LuaEvent::setVariant(LuaVariant v) { _luaVariant = std::move(v); }

    //-----EventTarget----
    inline EventTarget::EventTarget(Callback callback): _callback(std::move(callback)) {}
    inline void EventTarget::_onReceiveEvent(Event *event) {
        if(_callback) _callback((void*)event); //fixme!!!
        else onReceiveEvent(event);
    }

    //----EventObserver----
    inline void EventObserver::_onObserveEvent(Event *event, EventTarget *target) { onObserveEvent(event, target); }


 //========== /INLINEZ ==========

}


#endif // ELYSIAN_EVENT_HPP
