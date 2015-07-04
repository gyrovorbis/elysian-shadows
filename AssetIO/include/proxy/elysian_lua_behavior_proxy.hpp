#ifndef ELYSIAN_LUA_BEHAVIOR_PROXY_HPP
#define ELYSIAN_LUA_BEHAVIOR_PROXY_HPP

#include <component/elysian_lua_behavior.hpp>
#include <proxy/elysian_component_proxy.hpp>

namespace OOLUA {
    typedef elysian::Behavior       Behavior;
    typedef elysian::LuaBehavior    LuaBehavior;

}

OOLUA_PROXY(Behavior, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addTrigger, Behavior::TRIGGER)
    OOLUA_MEM_FUNC(void, removeTrigger, Behavior::TRIGGER)
    OOLUA_MEM_FUNC_CONST(const char*, getBehavName)
OOLUA_PROXY_END

OOLUA_PROXY(LuaBehavior, Behavior)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
//    OOLUA_MEM_FUNC_CONST(const char*, getBehavName)
    OOLUA_MEM_FUNC_CONST(uint64_t, getTriggers)
    OOLUA_MEM_FUNC(void, setTriggers, uint64_t)
    OOLUA_MEM_FUNC_CONST(maybe_null<LuaBehaviorScript*>, getScript)
    OOLUA_MEM_FUNC(void, setBehavName, const char*const)
    OOLUA_MEM_FUNC(Lua_table_ref, getParamTable)
    OOLUA_MEM_FUNC(void, setParamTable, Lua_table_ref)

OOLUA_PROXY_END

#endif // ELYSIAN_LUA_BEHAVIOR_PROXY_HPP

