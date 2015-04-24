#ifndef ELYSIAN_LUA_TABLE_PROXY_HPP
#define ELYSIAN_LUA_TABLE_PROXY_HPP

//THANK GOD FOR C++11 ALIAS TEMPLATES, MUTHAFUCKAAAAAA!

#ifdef ELYSIAN_ENGINE
#   include "elysian_lua_table_proxy_base.hpp"
namespace elysian {
    template <typename P, typename T=P>
    using LuaTableProxy = LuaTableProxyBase<T, P>;
}
#else
#   include "tkLuaTableProxy.h"

namespace elysian {
    template <typename P, typename T>
    using LuaTableProxy = TkLuaTableProxy<P, T>;
}

template <typename P, typename T>
using LuaTableProxy = TkLuaTableProxy<P, T>;

#endif


#endif // ELYSIAN_LUA_TABLE_PROXY_HPP
