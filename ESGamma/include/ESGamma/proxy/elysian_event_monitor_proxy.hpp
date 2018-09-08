#ifndef ELYSIAN_EVENT_MONITOR_PROXY_HPP
#define ELYSIAN_EVENT_MONITOR_PROXY_HPP

#include <oolua.h>
#include <ESGamma/utility/elysian_event_monitor.hpp>


namespace OOLUA {
    typedef elysian::EventMonitor       EventMonitor;
    typedef elysian::LuaEventMonitor    LuaEventMonitor;
    typedef elysian::LuaEventCallback   LuaEventCallback;

    template<> class Proxy_class<elysian::EventMonitor>;
    template<> class Proxy_class<elysian::LuaEventMonitor>;
    template<> class Proxy_class<elysian::LuaEventCallback>;
}

OOLUA_PROXY(EventMonitor)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, cancel)
OOLUA_PROXY_END

OOLUA_PROXY(LuaEventMonitor, EventMonitor)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setTable, OOLUA::Lua_table_ref&)
    OOLUA_MEM_FUNC(OOLUA::Lua_table_ref&, getTable)
OOLUA_PROXY_END

OOLUA_PROXY(LuaEventCallback, LuaEventMonitor)
    OOLUA_TAGS(
        No_default_constructor
    )
    OOLUA_CTORS(
        OOLUA_CTOR(OOLUA::Lua_func_ref&)
    )
    OOLUA_MEM_FUNC(void, setCallbackFunction, OOLUA::Lua_func_ref&)
OOLUA_PROXY_END



#endif // ELYSIAN_EVENT_MONITOR_PROXY_HPP

