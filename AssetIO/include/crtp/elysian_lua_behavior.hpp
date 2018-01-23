#ifndef ELYSIAN_LUA_BEHAVIOR_HPP
#define ELYSIAN_LUA_BEHAVIOR_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_lua_behavior_base.hpp>
namespace elysian {
    typedef LuaBehaviorBase LuaBehavior;
}
#else
#   include <AssetIO/tk_lua_behavior.h>
namespace elysian {
    typedef TkLuaBehavior LuaBehavior;
}
typedef TkLuaBehavior LuaBehavior;

#endif

#endif // ELYSIAN_LUA_BEHAVIOR_HPP

