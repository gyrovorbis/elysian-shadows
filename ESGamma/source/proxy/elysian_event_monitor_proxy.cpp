
#include <proxy/elysian_event_monitor_proxy.hpp>



/*!
    \qmltype EventMonitor
    \since 1.5.8
    \inmodule ESToolkit
    \brief Engine base-class for all event monitor types.
*/


/*!
    \qmltype LuaEventMonitor
    \inherits EventMonitor
    \since 1.5.8
    \inmodule ESToolkit
    \brief Base class for Lua event monitors.
*/
/*!
 * \qmlmethod void LuaEventMonitor::setTable(OOLUA::Lua_table_ref& table)
 *
 * Stores a reference to an internal Lua table for storing custom context data within the event.
 */
/*!
 * \qmlmethod OOLUA::Lua_table_ref& LuaEventMonitor::getTable()
 *
 * Retrieves reference to user's custom Lua table.
 */

/*!
    \qmltype LuaEventCallback
    \inherits LuaEventMonitor
    \since 1.5.8
    \inmodule ESToolkit
    \brief Holds a reference to a Lua function that will be invoked when the event is completed.
*/
/*!
 * \qmlmethod LuaEventCallback::LuaEventCallback(OOLUA::Lua_func_ref& func)
 *
 * Constructor taking a reference to a Lua function to be invoked after the event is completed.
 */
/*!
 * \qmlmethod void LuaEventCallback::setCallbackFunction(OOLUA::Lua_func_ref& func)
 *
 * Sets the internal Lua callback function to func.
 */



OOLUA_EXPORT_FUNCTIONS(EventMonitor)
OOLUA_EXPORT_FUNCTIONS_CONST(EventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventMonitor,
                       setTable,
                       getTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventCallback,
                       setCallbackFunction)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventCallback)
