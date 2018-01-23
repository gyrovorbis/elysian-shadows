#ifndef ELYSIAN_LUA_TABLE_PROXY_HPP
#define ELYSIAN_LUA_TABLE_PROXY_HPP

//THANK GOD FOR C++11 ALIAS TEMPLATES, MUTHAFUCKAAAAAA!

#ifdef ELYSIAN_ENGINE
#   include <utility/elysian_lua_table_proxy_base.hpp>
namespace elysian {
    template <typename P, typename T, class... Bases>
    using LuaTableProxy = LuaTableProxyBase<T, P, Bases...>;
}
#else
#   include <AssetIO/tk_lua_table_proxy.h>

namespace elysian {
    template <typename P, typename T, class... Bases>
    using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;
}

template <typename P, typename T, class... Bases>
using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;

#endif


#endif // ELYSIAN_LUA_TABLE_PROXY_HPP
