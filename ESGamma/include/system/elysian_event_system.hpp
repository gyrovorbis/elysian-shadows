#ifndef ELYSIAN_EVENT_SYSTEM_HPP
#define ELYSIAN_EVENT_SYSTEM_HPP

#include <system/elysian_system.hpp>
#include <vector>
#include <utility/elysian_cpp_lua_callback.hpp>
#include <utility/elysian_bit_mask.hpp>


namespace elysian {
    class CollisionContact;
    class Collidable;


    enum EVENT_TYPE {
        EVENT_COLLISION_ENTER,
        EVENT_COLLISION_EXIT,
        EVENT_TYPE_COUNT
    };


    using EventTypeFlags    = BitMask32;

    class Event {
        EventTypeFlags  _eventType;
    public:
                        Event(EventTypeFlags type);
        EventTypeFlags& getEventType(void);
        auto            getEventType(void) const -> const EventTypeFlags&;
    };


    class CollisionEvent: public Event {
        const CollisionContact* _contact;
    public:
        CollisionEvent(const CollisionContact* contact);
        auto getContact(void) const -> const CollisionContact*;
    };


    class EventMonitor {
    public:
        using Callback = CppLuaCallback<int, void*>;//EVENT FIXME!!!!!!!!

                        EventMonitor(void) = default;
                        EventMonitor(Callback callback);
        void            processEvent(Event* event);
    protected:
        virtual void    _processEvent(Event* event);
    private:
        Callback                    _callback;
        EventTypeFlags              _flags;

    };


    template <typename Derived>
    class CollisionEventSink {
        void processEvent(Event* event);  //delegate AND LOOK FOR SPECIAL SHIT YOU WANT TO COMPARE BEFORE TRIGGER
        void collisonEnterEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact); //downcast and call in child
        void collisionExitEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact);
        void collisionEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact);
    };

    template<typename Derived>
    class CollisionEventMonitor:
            public EventMonitor,
            public CollisionEventSink<Derived>
    {
        virtual void processEvent(Event* event) override;




    };





    class EventSystem: public System {
    private:
        std::vector<EventMonitor*> _monitors[EVENT_TYPE_COUNT];
    public:
        EventSystem(void) = default;
        void triggerEvent(Event* event) const;
        void registerEventMonitor(EventMonitor* monitor);
        void removeEventMonitor(EventMonitor* monitor);
        void clearEventMonitors(void);

    };


    //======= INLINE ========
    inline void EventSystem::triggerEvent(Event *event) const {
        const auto& monitors = _monitors[event->getEventType().getValue()];
        for(auto& m: monitors) {
            m->processEvent(event);
        }
    }



    inline Event::Event(EventTypeFlags type): _eventType(type) {}
    inline EventTypeFlags& Event::getEventType(void) { return _eventType; }
    inline auto Event::getEventType(void) const -> const EventTypeFlags& { return _eventType; }
    inline CollisionEvent::CollisionEvent(const CollisionContact* contact): Event(EVENT_COLLISION_ENTER),
        _contact(contact) {}
    inline const CollisionContact* CollisionEvent::getContact(void) const { return _contact; }
    inline EventMonitor::EventMonitor(Callback callback): _callback(std::move(callback)) {}
    inline void EventMonitor::processEvent(Event *event) {
        if(_callback) _callback((void*)event); //fixme!!!
        else _processEvent(event);
    }







}


#endif // ELYSIAN_EVENT_SYSTEM_HPP
