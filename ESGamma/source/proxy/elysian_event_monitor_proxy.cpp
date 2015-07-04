
#include <proxy/elysian_event_monitor_proxy.hpp>



OOLUA_EXPORT_FUNCTIONS(EventMonitor)
OOLUA_EXPORT_FUNCTIONS_CONST(EventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventMonitor,
                       setTable,
                       getTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventMonitor)

OOLUA_EXPORT_FUNCTIONS(LuaEventCallback,
                       setCallbackFunction)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaEventCallback)
