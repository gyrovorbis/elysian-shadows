#ifndef ELYSIAN_EVENT_HPP
#define ELYSIAN_EVENT_HPP

#include <stdint.h>
#include <utility/elysian_cpp_lua_callback.hpp>

namespace elysian {

    class EventSystem;

    enum EVENT_TYPE {
        EVENT_ENTITY_ACTIVATE,
        EVENT_ENTITY_DEACTIVATE,
        EVENT_KEYBOARD_ATTACHED,
        EVENT_KEYBOARD_REMOVED,
        EVENT_KEYBOARD_KEY_PRESSED,
        EVENT_KEYBOARD_KEY_RELEASED,
        EVENT_KEYBOARD_KEY,
        EVENT_MOUSE_ATTACHED,
        EVENT_MOUSE_REMOVED,
        EVENT_RUMBLE_DEVICE_ATTACHED,
        EVENT_RUMBLE_DEVICE_REMOVED,
        EVENT_VMU_ATTACHED,
        EVENT_VMU_REMOVED,
        EVENT_CONTROLLER_ATTACHED,
        EVENT_CONTROLLER_REMOVED,
        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_BUTTON,
        EVENT_MOUSE_BUTTON_PRESSED,
        EVENT_MOUSE_BUTTON_RELEASED,
        EVENT_MOUSE_WHEEL,
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
        EVENT_WIDGET_ACTIVE,
        EVENT_WIDGET_FOCUS,
        EVENT_WIDGET_MENU_CANCEL,
        EVENT_WIDGET_MENU_CURRENT,
        EVENT_WIDGET_MENU_SELECTION,
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


    class Event {
    public:
                        Event(EventType type);

        EventType       getEventType(void) const;
        EVENT_STATE     getEventState(void) const;
        void            setEventState(EVENT_STATE state);
    private:
        EventType       _eventType  = EVENT_TYPE_INVALID;
        EVENT_STATE     _eventState = EVENT_STATE_PENDING;

    };

    class LuaEvent: public Event {
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
