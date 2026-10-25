/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EVENT_SYSTEM_HPP
#define ELYSIAN_EVENT_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <vector>
#include <AssetIO/utility/elysian_bit_mask.hpp>
#include <AssetIO/level/elysian_event.hpp>

namespace elysian {

    class EventSystem: public System {

    public:
                EventSystem(void) = default;
                ~EventSystem(void);

        /* Used to signal to signal to an EventTarget and all listening Observers that an Event
           has occurred, with the target as intended recipient object of the event.
        */
        void    sendEvent(Event* event, EventTarget* receiver);
        void    sendEvent(Event&& event, EventTarget* receiver);  //R-value overload for supporting stack-allocated Events

        /* Like sendEvent, notifies all listening Observers of an Event associated associated
         * with the given EventTarget, unlike sendEvent, the target itself serves as the
         * origin of the Event, so the Event is not sent back to the target itself.
         */
        void    emitEvent(Event* event, EventTarget* source);
        void    emitEvent(Event&& event, EventTarget* source); //R-value overload for supporting stack-allocated Events

        /* Works the exact same way as sendEvent, except the event is sent to ALL observers
         * of an associated Event, rather than those listening for a certain EventTarget.
         * This is essentially the same thing as calling sendEvent with target == nullptr,
         * but prefer this method as it makes the intent clearer.
         */
        void    broadcastEvent(Event* event);
        void    broadcastEvent(Event&& event);         //R-value overload for supporting stack-allocated Events

        void    addEventObserver(EventObserver* monitor, EventType type, EventTarget* target=nullptr);
        void    addEventObserverLua(EventObserver* monitor, EventType type, LuaVariant target);     //Overload so Lua can pass nil as target

        void    removeEventObserver(EventObserver* monitor, EventType type, EventTarget* target=nullptr);
        void    removeEventObserverLua(EventObserver* monitor, EventType type, LuaVariant target);  //Overload so Lua can pass nil as target

        void    clearEventObservers(void);

    private:
        struct EventObserverEntry {
            EventObserver*  monitor;
            EventTarget*    target = nullptr;
            bool            operator==(const EventObserverEntry& rhs) const;
        };

        std::vector<EventObserverEntry>
                _monitors[EVENT_TYPE_COUNT];

        void    _processEvent(Event* event, EventTarget* target, bool triggerEvent);

        //Compile-time override me in ESTk code for logging!
        void    _notifyTarget(EventTarget* target, Event* event);

        //Compile-time override me in ESTk code for logging!
        void    _notifyMonitor(EventObserver* monitor, Event* event, EventTarget* target);


    };


    //======= INLINE ========
    inline bool EventSystem::EventObserverEntry::operator==(const EventObserverEntry& rhs) const {
        return monitor == rhs.monitor && target == rhs.target;
    }

    inline EventSystem::~EventSystem(void) {
        clearEventObservers();
    }

    inline void EventSystem::addEventObserver(EventObserver* monitor, EventType type, EventTarget* target) {
        if(type < EVENT_TYPE_COUNT) {
            _monitors[type].push_back(EventObserverEntry{ monitor, target });
        } else errprintf("EventSystem::addEventObserver() - Tried to register monitor for invalid event type: %d", type);
    }

    inline void EventSystem::clearEventObservers(void) {
        for(size_t i = 0; i < EVENT_TYPE_COUNT; ++i) {
            _monitors[i].clear();
        }
    }

    inline void EventSystem::sendEvent(Event* event, EventTarget* target) {
        _processEvent(event, target, true);
    }
    //R-value overload for supporting stack-allocated Events
    inline void EventSystem::sendEvent(Event&& event, EventTarget* target) {
        sendEvent(&event, target);
    }

    inline void EventSystem::emitEvent(Event* event, EventTarget* source) {
        _processEvent(event, source, false);
    }
    //R-value overload for supporting stack-allocated Events
    inline void EventSystem::emitEvent(Event&& event, EventTarget* source) {
        emitEvent(&event, source);
    }

    inline void EventSystem::broadcastEvent(Event* event) {
        _processEvent(event, nullptr, false);
    }

    inline void EventSystem::broadcastEvent(Event&& event) {
        broadcastEvent(&event);
    }

    inline void EventSystem::_notifyTarget(EventTarget* target, Event* event) {
        target->_onReceiveEvent(event);
    }

    inline void EventSystem::_notifyMonitor(EventObserver* monitor, Event* event, EventTarget* target) {
        monitor->_onObserveEvent(event, target);
    }

    /* Right now we're assuming "No target" (nullptr) means you give a fuck about ALL messages
     * to all targets, which may not actually be the case if a particular message can actually be
     * both global and recipient-based? Or is that just fucking stupid anyway? If that ever matters,
     * we have to make an EventTarget::any static target to differentiate "all" targets from
     * "no" targets.
     */
    inline void EventSystem::_processEvent(Event* event, EventTarget* target, bool triggerEvent) {
        GY_ASSERT(event);
        const auto eventType = event->getEventType();
        GY_ASSERT(eventType < EVENT_TYPE_COUNT);
        auto& monitors = _monitors[eventType];
        if(target && triggerEvent) {
            _notifyTarget(target, event);
            if(event->getEventState() == EVENT_STATE_IGNORED) return;
        }
        for(auto&& it: monitors) {
            if(!it.target || (it.target == target)) {
                _notifyMonitor(it.monitor, event, target);
            }
        }
    }







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
    class CollisionEventTarget:
            public EventTarget,
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
