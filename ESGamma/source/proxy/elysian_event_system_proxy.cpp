#include <ESGamma/proxy/elysian_event_system_proxy.hpp>

/*!
    \qmltype EventSystem
    \since 1.6.1
    \inmodule Engine
    \brief Engine subsystem for managing all Event handling

    EventSystem is a subsystem within the Engine responsible for handling
    all internally generated Engine \l{Event}s and can also be used for handling
    external, custom Lua events as well. At its core, the EventSystem is used to
    allow one object to signal to another object or group of objects than some
    \l{Event} has occured.

    \section2 Retrieving the EventSystem
    The EventSystem object can be retrieved through the static \l{Engine} class:
    \code
    eventSys = Engine.getEventSys()
    \endcode

    \section1 Overview of Event Handling

    \section2 Event Representation
    An \l{Event} can either represent a simple message, containing only an \l{Event::TYPE},
    or it can be a complex object, subclassing \l{Event} then adding information of its own.
    For more information, view the \l{Event}{Event Type Documentation}.

    \section2 Event Targets
    \l{EventTarget}s may be seen as the source or desination objects of an \l{Event}. There are
    several built-in \l{EventTarget}s such as \l{Entity} and \l{Behavior}, which both are both
    subclasses. You can view these classes as "containing" the \l{Event}s.

    \section2 Sending or Emitting an Event
    When an \l{Event} happens externally to its associated \l{EventTarget}, the \l{Event} is
    sent to the \l{EventTarget} through the EventSystem. An example of this is the \l{CollisionSystem}
    informing an \l{Entity} \l{EventTarget} that another object is colliding
    with it by sending it an \l{Event} with the \l{Event::TYPE}{COLLISION_ENTER} type.

    Another example of this is using a \l{LuaEvent} to represent a simple message and passing it
    from one \l{Behavior} to another.

    \code
    function onAreaEnter()
        local luaMessage = LuaEvent.new("What's up, other LuaBehavior!?")
        local otherBehavior = self.selfEntity.behaviors[2]
        Engine.getEventSys():sendEvent(luaMessage, behavior2)
    end
    \endcode

    \section2 Receiving an Event
    An \l{EventTarget} object receives an Event through its onReceiveEvent method/trigger. So the
    second \l{LuaBehavior} could receive the \l{Event} sent from the previous example with:

    \code
    function onReceiveEvent(self, event)
        if(event:getEventType() == Event::LUA) then
            print(event:getVariant())
        end
    end
    \endcode
    \e{Note: If you are receiving multiple different types of events within the same \l{EventTarget},
        you should check the \l{Event::TYPE} first, before accessing it, to know what type of object it is.}

    \section2 Observing an Event
    In addition to simply sending the message from one object to another, an event can be "observed"
    which allows it to be routed to any number of other objects as well. To observe an Event, an \l{EventObserver}
    object is created and registered with the EventSystem for the associated Event. Then it too will receive the
    Event, through the onObserveEvent method/trigger.

    So a third \l{LuaBehavior} wanting to listen in on messages received by the second \l{LuaBehavior} may look like:
    \code
    function onCreate(self, params)
        --register the current behavior as an event observer for LUA events routed to the second behavior on the same Entity
        Engine.getEventSys():addEventObserver(self.selfBehav, Event.LUA, self.selfEntity.behaviors[2])
    end

    function onObserveEvent(self, event, eventTarget)
        if(eventTarget == self.selfEntity.behaviors[2]) then
            if(event:getEventType() == Event::LUA) then
                printf("I snooped the  message too! Message: "..event:getVariant())
            end
        end
    end
    \endcode
    You can also observe all \l{Event} objects of a certain type regardless of which \l{EventTarget} they are sent too by simply
    passing nil when adding the observer for the target. This would allow the third \l{LuaBehavior} in the above example to
    receive all \l{LuaEvent}s regardless of where they are routed.

    \e{NOTE: If you are observing multiple different types of events routed to multiple different targets, you should check both
    the target of the event and the event type to know which event it is you're currently observing!}

    \section2 Event Routing
    There are 3 basic methods for routing \l{Event}s:
    \table
        \header
            \li Method
            \li Routing
            \li Usage
        \row
            \li \l{EventSystem::sendEvent}{sendEvent}
            \li Send an \l{Event} to an \l{EventTarget} receiver and also notify all listening \l{EventObserver}s.
            \li Used to notify an \l{EventTarget} of one of its \l{Event}s occurring externally to it or simply as
                a means to pass a message from one object to an \l{EventTarget} object.
        \row
            \li \l{EventSystem::emitEvent}{emitEvent}
            \li Works similarly to sendEvent, EXCEPT the \l{EventTarget} is not also notified of the event.
            \li This is used  when the \l{Event} is produced by the \l{EventTarget} itself, to simply notify any
                \l{EventObserver} that it has occurred.
        \row
            \li \l{EventSystem::broadcastEvent}{broadcastEvent}
            \li The \l{Event} is routed to ALL \l{EventObserver}s listening for the specified \l{Event}, regardless of
                their target.
            \li This is used for global \l{Event}s that don't have an associated \l{EventTarget} object, such as a
                keyboard being removed, or the current area changing, which isn't associated with an \l{Entity} or \l{Behavior}.
    \endtable

    \section2 Removing Observers
    When a script is destroyed, or the Event no longer needs to be observed, the \l{EventObserver} \b{must} be removed from the
    EventSystem. It is also a very good idea to remove observers for Events that do not currently need to be processed, rather than
    ignoring the event in the onObserveEvent method/trigger, as it will reduce the amount of processing required for each Event and
    will result in optimal performance, since the Engine will not have to waste time calling into Lua triggers or C++ virtual methods
    that will just be ignored.

    \code
    function onDestroy(self)
        Engine.getEventSys():removeEventObserver(self.selfBehav, Event.LUA, self.selfEntity.behaviors[2])
    end
    \endcode
    \e {NOTE: You can remove the observer registered to all \l{EventTarget}s of the associated event type by simply passing nil
    as the \l{EventTarget}}

    \section1 Event Demo Area
    You can find a live demo area demonstrating both observing events and sending/receiving events between two different Lua
    scripts here: \l {file:/assets/level/Examples/area/EventDemo}

    The main Lua Behavior script containing the Event logic is located at \l {file:/assets/level/Examples/area/EventDemo/event_demo_behav.lua}.

    \sa Event, EventTarget, EventObserver
*/
/*!
 * \qmlmethod void EventSystem::sendEvent(Event* event, EventTarget* receiver)
 *
 * Sends the given \a{event} to the \a{receiver} object, by calling its onReceiveEvent() method/trigger and also notifies all
 * \l{EventObserver} objects associated with the given \l{Event} and \{EventTarget} by calling their onObserveEvent() method/trigger.
 *
 * \sa emitEvent, broadcastEvent
 */
/*!
 * \qmlmethod void EventSystem::emitEvent(Event* event, EventTarget* receiver)
 *
 * Notifies all \l{EventObserver} objects associated with the given \l{Event} and \{EventTarget} by calling their onObserveEvent() method/trigger.
 *
 * \sa sendEvent, broadcastEvent
 */
/*!
 * \qmlmethod void EventSystem::broadcastEvent(Event* event)
 *
 * Notifies all \l{EventObserver} objects registered for the \l{Event::TYPE} by calling their onObserveEvent() method/trigger.
 *
 * \sa sendEvent, emitEvent
 */
/*!
 * \qmlmethod void EventSystem::addEventObserver(EventObserver* observer, Event::TYPE eventType, EventTarget* target)
 *
 * Registers the given \a{observer} to be notified when any event with the given \a{eventType} occurs with the associated \a{target}
 * or for ANY \l{EventTarget} if \a{target} is nil.
 *
 * \sa removeEventObserver
 */
/*!
 * \qmlmethod void EventSystem::removeEventObserver(EventObserver* observer, Event::TYPE eventType, EventTarget* target)
 *
 * Removes the given \a{observer} from being notified when an event with the given \a{eventType} occurs with the associated \a{target}
 * or for all \l{EventTarget}s if \a{target} is nil.
 *
 * \sa addEventObserver
 */

OOLUA_EXPORT_FUNCTIONS(EventSystem,
                       sendEvent,
                       emitEvent,
                       broadcastEvent,
                       addEventObserver,
                       removeEventObserver)

OOLUA_EXPORT_FUNCTIONS_CONST(EventSystem)

