#ifndef ELYSIAN_LUA_MANAGER_HPP
#define ELYSIAN_LUA_MANAGER_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_lua_manager_base.hpp>
namespace elysian {
    typedef LuaManagerBase LuaManager;
}
#else
#   include <tkLuaManager.h>
namespace elysian {
    typedef TkLuaManager LuaManager;
}
typedef TkLuaManager LuaManager;

#endif



#endif // ELYSIAN_LUA_MANAGER_HPP
