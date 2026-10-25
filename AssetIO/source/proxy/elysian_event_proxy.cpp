/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <AssetIO/proxy/elysian_event_proxy.hpp>

/*!
    \qmltype EventTarget
    \since 1.6.1
    \inmodule Engine
    \brief Base class objects capable of directly receiving Events from EventSystem

    An EventTarget is an object that can have \l{Event} objects associated with it,
    either as the source of the events or the recipient of the events. EventTarget
    itself is an abstract base-class, which is subclassed by several built-in Engine
    objects such as \l{Entity} and \l{Widget} and can optionally be subclassed for
    custom Lua-based targets. A single \l{EventTarget} can receive any number of
    different events, so there's no need to instantiate a separate target to handle
    each event type.

    An EventTarget can notify any registered \l{EventObserver}s of an associated
    event occuring internally by calling \l{EventSystem::emitEvent}. An external
    object or system can notify an EventTarget that one of its own events has
    occured by calling \l{EventSystem::sendEvent}.

    For a more detailed overview of Event Management within the Engine, and using
    the relevant classes, please refer to the \l{EventSystem}{EventSystem Documentation}.

    \sa Event, EventSystem, EventObserver
*/
/*!
  \qmlproperty enumeration EventTarget::TYPE
    Every EventTarget subclass has one of the following types:
    \table
        \header
            \li Value
            \li Child Class
        \row
            \li INVALID
            \li No associated child class (you should never see this)
        \row
            \li ENTITY
            \li \l{Entity}
        \row
            \li WIDGET
            \li \l{Widget}
        \row
            \li BEHAVIOR
            \li \l{Behavior}
        \row
            \li LUA
            \li ????
   \endtable

   \sa getEventTargetType
*/
/*!
 * \qmlmethod EventTarget::TYPE EventTarget::getEventTargetType()
 *
 * Returns an enumeration value for the type of target represented by this instance.
 * \sa {EventTarget::TYPE}
 */

OOLUA_EXPORT_FUNCTIONS_CONST(EventTarget,
                             getEventTargetType)
OOLUA_EXPORT_FUNCTIONS(EventTarget)


/*!
    \qmltype EventObserver
    \since 1.6.1
    \inmodule Engine
    \brief Base class of any Engine object observing or listening to Events

    An EventObserver is an abstract base-class that is inherited by any Engine
    object capable of listening to \l{Event}s. A single EventObserver instance
    may be used to observe any number of events associated with any number of
    EventTargets, so there is no need to instantiate separate instances for each.

    \section2 Registering an Observer
    To actually receive events, an observer must be registered to the \l{EventSystem}
    through \l{EventSystem::addEventObserver}. An observer may also optionally be set to
    only observe events associated with a certain \l{EventTarget}, or will observe all of a
    certain type of event sent to any target if \l{EventSystem::addEventObserver} is passed
    nil for its target argument.

    \section2 Unregistering an Observer
    When an EventObserver no longer needs to listen for a particular event and/or a
    particular target, it should be deactivated with \l{EventSystem::removeEventObserver},
    rather than simply ignoring the event to reduce event processing overhead.

    For a more detailed overview of Event Management within the Engine, and using
    the relevant classes, please refer to the \l{EventSystem}{EventSystem Documentation}.

    \sa Event, LuaEvent, EventObserver, EventSystem
*/

OOLUA_EXPORT_NO_FUNCTIONS(EventObserver)

/*!
    \qmltype Event
    \since 1.6.1
    \inmodule Engine
    \brief Base class for all Event types with the Engine

    An Event is a simple object utilized by the EventSystem to represent some
    specific event or occurence within the Engine. Each event has a specific
    type associated with it, used for identifying the object type and a state
    used to store how the event has been handled. Events can also provide
    additional custom information by subclassing the Event class and adding
    custom data.

    For a more detailed overview of Event Management within the Engine, and using
    the relevant classes, please refer to the \l{EventSystem}{EventSystem Documentation}.

    \sa LuaEvent, EventSystem, EventTarget, EventObserver
*/
/*!
  \qmlproperty enumeration Event::TYPE
    Every Event handled by the Engine must be one of the following event types:
    \table
        \header
            \li TYPE
            \li Object Type
            \li Description
        \row
            \li Event.ENTITY_ACTIVATE
            \li Event
            \li Entity has been activated within the current area
        \row
            \li Event.ENTITY_DEACTIVATE
            \li Event
            \li Entity has been deactivated within the current area
        \row
            \li Event.KEYBOARD_ATTACHED
            \li Event
            \li New Keyboard device has been detected
        \row
            \li Event.KEYBOARD_REMOVED
            \li Event
            \li Existing Keyboard device has been removed
        \row
            \li Event.MOUSE_ATTACHED
            \li Event
            \li New Mouse device has been detected
        \row
            \li Event.MOUSE_REMOVED
            \li Event
            \li Existing Mouse device has been removed
        \row
            \li Event.RUMBLE_DEVICE_ATTACHED
            \li Event
            \li New \l{RumbleDevice} has been detected
        \row
            \li Event.RUMBLE_DEVICE_REMOVED
            \li Event
            \li Existing \l{RumbleDevice} has been removed
        \row
            \li Event.VMU_ATTACHED
            \li Event
            \li New Visual Memory device has been detected
        \row
            \li Event.VMU_REMOVED
            \li Event
            \li Existing Visual Memory device has been removed
        \row
            \li Event.CONTROLLER_ATTACHED
            \li Event
            \li New Controller device has been detected.
        \row
            \li Event.CONTROLLER_REMOVED
            \li Event
            \li Existing Controller device has been removed
        \row
            \li Event.CONTROLLER_BUTTON_PRESSED
            \li \l ControllerButtonEvent
            \li State of one of the buttons on a controller changed from being up to down.
        \row
            \li Event.CONTROLLER_BUTTON_RELEASED
            \li \l ControllerButtonEvent
            \li State of one of the buttons on a controller changed from being down to up.
        \row
            \li Event.CONTROLLER_ANALOG_MOVED
            \li \l ControllerAnalogEvent
            \li The position of one of a Controller's analog sticks has moved.
        \row
            \li Event.CONTROLLER_TRIGGER_MOVED
            \li \l ControllerTriggerEvent
            \li Amount of pressure being applied to a Controller's trigger has changed.
        \row
            \li Event.KEYBOARD_KEY_PRESSED
            \li \l KeyEvent
            \li State of one of a Keyboard's keys changed from being up to down.
        \row
            \li Event.KEYBOARD_KEY_RELEASED
            \li \l KeyEvent
            \li State of one of a Keyboard's keys changed from being down to up.
        \row
            \li Event.MOUSE_MOVED
            \li \l MouseEvent
            \li Mouse's cursor position has changed
        \row
            \li Event.MOUSE_BUTTON_PRESSED
            \li \l MouseEvent
            \li State of one of the mouse buttons changed from being up to down
        \row
            \li Event.MOUSE_BUTTON_RELEASED
            \li \l MouseEvent
            \li State of one of the mouse buttons changed from being down to up
        \row
            \li Event.MOUSE_SCROLLED
            \li \l MouseWheelEvent
            \li Mouse's scroll wheel has been moved
        \row
            \li Event.COLLISION_ENTER
            \li Event
            \li Target has begun colliding with another object
        \row
            \li Event.COLLISION_EXIT
            \li Event
            \li Target is no longer colliding with the other object
        \row
            \li Event.AREA_ENTER
            \li Event
            \li Party has entered a new area
        \row
            \li Event.AREA_EXIT
            \li Event
            \li Party is leaving the current area

        \row
            \li Event.GAME_TIME_DAWN
            \li Event
            \li In-Game time has just reached dawn (hour 6)

        \row
            \li Event.GAME_TIME_NOON
            \li Event
            \li In-game time has just reached noon (hour 12)

        \row
            \li Event.GAME_TIME_DUSK
            \li Event
            \li In-game time has just reached dusk (hour 18)

        \row
            \li Event.GAME_TIME_MIDNIGHT
            \li Event
            \li In-game time has just reached midnight (hour 24)

        \row
            \li Event.WIDGET_ACTIVE
            \li Event
            \li Target Widget has either been activated or deactivated
        \row
            \li Event.WIDGET_FOCUS
            \li Event
            \li Target Widget has either gained or lost focus
        \row
            \li Event.WIDGET_MENU_CANCEL
            \li Event
            \li Target MenuWidget has been canceled/exited by player
        \row
            \li Event.WIDGET_MENU_CURRENT
            \li Event
            \li Target MenuWidget's cursor position has changed
        \row
            \li Event.WIDGET_MENU_SELECTION
            \li Event
            \li Target MenuWidget's selection has changed

        \row
            \li Event.LUA
            \li \l{LuaEvent}
            \li Custom type used for representing custom Lua-based events
    \endtable

    \sa getEventType

*/

/*!
  \qmlproperty enumeration Event::STATE
     Every Event is in one of the following states
  \table
    \header
        \li Value
        \li Description
    \row
        \li Event.STATE_PENDING
        \li Event is still waiting to be processed by a target/observer
    \row
        \li Event.STATE_ACCEPTED
        \li Event has been successfully processed by a target/observer
    \row
        \li Event.STATE_IGNORED
        \li Event has been cancelled and won't be processed by any further target/observers
  \endtable

  \sa getEventState, setEventState
*/

/*!
 * \qmlmethod Event::TYPE Event::getEventType()
 *
 * Returns an enumeration value for the type of event reperesented by the Event instance.
 * \sa {Event::TYPE}
 */

/*!
 * \qmlmethod Event::STATE Event::getEventState()
 *
 * Returns an enumeration value for the state of the current event.
 * \sa {Event::STATE}
 */
/*!
 * \qmlmethod void Event::setEventState(Event::STATE state)
 *
 * Sets the state of the Event object to \a{state}.
 * \sa {Event::STATE}
 */

OOLUA_EXPORT_FUNCTIONS_CONST(Event,
                             getEventType,
                             getEventState)

OOLUA_EXPORT_FUNCTIONS(Event,
                       setEventState)




/*!
    \qmltype LuaEvent
    \since 1.6.1
    \inmodule Engine
    \inherits Event
    \brief Event type for custom Lua events

    LuaEvents are used to represent custom events created by Lua
    scripts that can be used to extend the basic events supported
    by the Engine. A LuaEvent can contain any arbitrary Lua variable,
    accessible through the variant property.

    \section2 LuaEvent SubType Management
    All LuaEvents have the same \l{Event::TYPE} of LUA, meaning that
    as far as the Engine is concerned, all LuaEvents are treated
    uniformly and are of the same type.

    It's up to the user to implement any sort of additional typing used to
    differentiate between different types of LuaEvent objects.
    Any additional event subtyping is to be handled by the script itself
    as part of the LuaVariant table information. For example, each
    LuaEvent table may contain an additional "subType" field that is
    accessed by the handler object to determine additional information
    about the event being processed, and a table of the diffrent supported
    subTypes can be maintained by Lua (recommended).

    For a more detailed overview of Event Management within the Engine, and using
    the relevant classes, please refer to the \l{EventSystem}{EventSystem Documentation}.

    \sa Event, EventSystem
*/


/*!
 * \qmlmethod void LuaEvent::LuaEvent()
 *
 * Default constructor constructs LuaEvent with a variant value of type "nil."
 */

/*!
 * \qmlmethod void LuaEvent::LuaEvent(LuaVariant v)
 *
 * Constructs a LuaEvent object containing the variant \a{v}.
 * \sa setVariant, getVariant
 */
/*!
 * \qmlmethod LuaVariant LuaEvent::getVariant()
 *
 * Returns the custom variant data stored within the LuaEvent.
 * \sa setVariant
 */
/*!
 * \qmlmethod void LuaEvent::setVariant(LuaVariant v)
 *
 * Sets the custom variant data stored within the LuaEvent to \a{v}.
 * \sa getVariant
 */

OOLUA_EXPORT_FUNCTIONS(LuaEvent,
                       setVariant)

OOLUA_EXPORT_FUNCTIONS_CONST(LuaEvent,
                             getVariant)

