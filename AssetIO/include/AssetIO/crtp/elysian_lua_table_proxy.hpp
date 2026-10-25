/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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
#   include <ESToolkit/AssetIO/tk_lua_table_proxy.h>

namespace elysian {
    template <typename P, typename T, class... Bases>
    using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;
}

template <typename P, typename T, class... Bases>
using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;

#endif


#endif // ELYSIAN_LUA_TABLE_PROXY_HPP
