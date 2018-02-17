#ifndef ELYSIAN_EVENT_SYSTEM_HPP
#define ELYSIAN_EVENT_SYSTEM_HPP

#include <system/elysian_system.hpp>
#include <vector>
#include <utility/elysian_bit_mask.hpp>
#include <level/elysian_event.hpp>

namespace elysian {

    class EventSystem: public System {
    private:
        struct EventObserverEntry {
            EventObserver*   monitor;
            EventReceiver*  receiver = nullptr;
            bool            operator==(const EventObserverEntry& rhs) const {
                return monitor == rhs.monitor && receiver == rhs.receiver;
            }
        };

        std::vector<EventObserverEntry> _monitors[EVENT_TYPE_COUNT];

        /* Right now we're assuming "No receiver" (nullptr) means you give a fuck about ALL messages
         * to all receivers, which may not actually be the case if a particular message can actually be
         * both global and recipient-based? Or is that just fucking stupid anyway? If that ever matters,
         * we have to make an EventReceiver::any static receiver to differentiate "all" receivers from
         * "no" receivers.
         */
        void    _processEvent(Event* event, EventReceiver* receiver, bool triggerEvent) {
            GY_ASSERT(event);
            const auto eventType = event->getEventType();
            GY_ASSERT(eventType < EVENT_TYPE_COUNT);
            auto& monitors = _monitors[eventType];
            if(receiver && triggerEvent) {
                _notifyReceiver(receiver, event);
                if(event->getEventState() == EVENT_STATE_IGNORED) return;
            }
            for(auto&& it: monitors) {
                if(!it.receiver || (it.receiver == receiver)) {
                    _notifyMonitor(it.monitor, event, receiver);
                }
            }
        }

        //Compile-time override me in ESTk code for logging!
        void    _notifyReceiver(EventReceiver* receiver, Event* event) {
            receiver->_onReceiveEvent(event);
        }

        //Compile-time override me in ESTk code for logging!
        void    _notifyMonitor(EventObserver* monitor, Event* event, EventReceiver* receiver) {
            monitor->_onObserveEvent(event, receiver);
        }

    public:
                EventSystem(void) = default;

                ~EventSystem(void) {
                    clearEventObservers();
                }


        //calls receiver's event listener and any active monitors
        void    sendEvent(Event* event, EventReceiver* receiver) {
            _processEvent(event, receiver, true);
        }
        void    sendEvent(Event&& event, EventReceiver* receiver) {
            sendEvent(&event, receiver);
        }
        //same as sendEvent, but does not cause the even to be fired on receiver!
        void    notifyEvent(Event* event, EventReceiver* receiver) {
            _processEvent(event, receiver, false);
        }
        void    notifyEvent(Event&& event, EventReceiver* receiver) {
            notifyEvent(&event, receiver);
        }

        //same as calling sendEvent with receiver == nullptr
        void    broadcastEvent(Event* event) {
            _processEvent(event, nullptr, false);
        }
        void    broadcastEvent(Event&& event) {
            broadcastEvent(&event);
        }

        void    addEventObserver(EventObserver* monitor, EventType type, EventReceiver* receiver=nullptr) {
            if(type < EVENT_TYPE_COUNT) {
                _monitors[type].push_back(EventObserverEntry{ monitor, receiver });
            } else errprintf("EventSystem::addEventObserver() - Tried to register monitor for invalid event type: %d", type);
        }

        void    removeEventObserver(EventObserver* monitor, EventType type, EventReceiver* receiver=nullptr) {
            if(type < EVENT_TYPE_COUNT) {
                std::vector<EventObserverEntry>& monitors = _monitors[type];
                EventObserverEntry entry { monitor, receiver };
                for(size_t i = 0; i < monitors.size(); ++i) {
                    if(monitors[i] == entry) {
                        monitors.erase(monitors.begin() + i);
                        return;
                    }
                }
            } else errprintf("EventSystem::removeEventObserver() - Tried to remove monitor for invalid event type: %d", type);
        }

        void clearEventObservers(void) {
            for(size_t i = 0; i < EVENT_TYPE_COUNT; ++i) {
                _monitors[i].clear();
            }
        }

    };


    //======= INLINE ========



    inline Event::Event(EventType type): _eventType(type) {}
    inline EventType Event::getEventType(void) const { return _eventType; }


    inline EventReceiver::EventReceiver(Callback callback): _callback(std::move(callback)) {}
    inline void EventReceiver::_onReceiveEvent(Event *event) {
        if(_callback) _callback((void*)event); //fixme!!!
        else onReceiveEvent(event);
    }

    inline auto Event::getEventState(void) const -> EVENT_STATE { return _eventState; }

    inline void EventObserver::_onObserveEvent(Event *event, EventReceiver *receiver) { onObserveEvent(event, receiver); }










#if 0

    class LuaEvent: public Event {
        LuaVariant  _luaVariant;
        //BitMask32 _subType?
    public:
                    LuaEvent(LuaVariant v);
        LuaVariant  getVariant(void) const;
    };



    class CollisionEvent: public Event {
        const CollisionContact* _contact;
    public:
        CollisionEvent(const CollisionContact* contact);
        auto getContact(void) const -> const CollisionContact*;
    };



    template <typename Derived>
    class CollisionEventSink {
        void processEvent(Event* event);  //delegate AND LOOK FOR SPECIAL SHIT YOU WANT TO COMPARE BEFORE TRIGGER
        void collisonEnterEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact); //downcast and call in child
        void collisionExitEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact);
        void collisionEvent(Collidable* bodyA, Collidable* bodyB, const CollisionContact* contact);
    };

    template<typename Derived>
    class CollisionEventReceiver:
            public EventReceiver,
            public CollisionEventSink<Derived>
    {
        virtual void processEvent(Event* event) override;




    };


    inline CollisionEvent::CollisionEvent(const CollisionContact* contact): Event(EVENT_COLLISION_ENTER),
        _contact(contact) {}

    inline const CollisionContact* CollisionEvent::getContact(void) const { return _contact; }
#endif



}


#endif // ELYSIAN_EVENT_SYSTEM_HPP
