/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LUA_MANAGER_HPP
#define ELYSIAN_LUA_MANAGER_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_lua_manager_base.hpp>
namespace elysian {
    typedef LuaManagerBase LuaManager;
}
#else
#   include <ESToolkit/ESGamma/tk_lua_manager.h>
namespace elysian {
    typedef TkLuaManager LuaManager;
}
typedef TkLuaManager LuaManager;

#endif



#endif // ELYSIAN_LUA_MANAGER_HPP
