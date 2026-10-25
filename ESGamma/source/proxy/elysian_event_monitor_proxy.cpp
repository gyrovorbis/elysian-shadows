/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */


#include <ESGamma/proxy/elysian_event_monitor_proxy.hpp>



/*!
    \qmltype EventMonitor
    \since 1.5.8
    \inmodule Engine
    \brief Engine base-class for all event monitor types.

    EventMonitor is the base-class for all EventMonitor types, including those built into the
    \l{Engine} and those provided by Lua.
*/
/*!
 * \qmlmethod EventMonitor* EventMonitor::null()
 *
 * Static function returning a blank, default EventMonitor object that can be passed to functions when we don't
 * care about using a notification callback.
 */
/*!
 * \qmlmethod void EventMonitor::cancel()
 *
 * Cancels the action being listened to by EventMonitor. This function must be called before any objects
 * involved in the event processing are deleted, otherwise the engine will still be trying to use them. It can
 * also be called to cancel an ongoing event and reschedule or reassign the event.
 *
 * \e { For Example: If you have a \l{Character} with a WayPoint Behavior attached to it to make him walk back
 * and forth between waypoints, the waypoint event must be cancelled through this function before you can safely
 * delete the \l{Entity}, otherwise it will still be in use by the Engine!}
 */

/*!
    \qmltype LuaEventMonitor
    \inherits EventMonitor
    \since 1.5.8
    \inmodule Engine
    \brief Base class for Lua event monitors.

    Base class for all \l{EventMonitor}s to be used within Lua. Stores a user-provided Lua
    table as a context, that can be accessed later on.
*/
/*!
 * \qmlmethod void LuaEventMonitor::setTable(Lua_table table)
 *
 * Stores a reference to an internal Lua table for storing custom context data within the event.
 */
/*!
 * \qmlmethod Lua_table* LuaEventMonitor::getTable()
 *
 * Retrieves reference to user's custom Lua table.
 */

/*!
    \qmltype LuaEventCallback
    \inherits LuaEventMonitor
    \since 1.5.8
    \inmodule Engine
    \brief Holds a reference to a Lua function that will be invoked when the event is completed.

    LuaEventCallback is a type of \l{LuaEventMonitor} that invokes a Lua function when the
    requested processing is complete. The Lua callback function can be set with setCallbackFunction(),
    and an optional context table can be set through LuaEventMonitor::setTable() and can be
    retrieved later from the callback function with LuaEventMonitor::getTable().
*/
/*!
 * \qmlmethod LuaEventCallback::LuaEventCallback(Lua_function func)
 *
 * Constructor taking a reference to a Lua function to be invoked after the event is completed.
 */
/*!
 * \qmlmethod void LuaEventCallback::setCallbackFunction(Lua_function func)
 *
 * Sets the internal Lua callback function to func.
 */



OOLUA_EXPORT_FUNCTIONS(EventMonitor,
                       cancel)
OOLUA_EXPORT_FUNCTIONS_CONST(EventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventMonitor,
                       setTable,
                       getTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventCallback,
                       setCallbackFunction)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventCallback)
