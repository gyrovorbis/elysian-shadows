/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LUA_VARIANT_HPP
#define ELYSIAN_LUA_VARIANT_HPP

#ifdef ELYSIAN_ENGINE
#   include <utility/elysian_lua_variant_base.hpp>
namespace elysian {
    using LuaVariant = LuaVariantBase;
}
#else
#   include <ESToolkit/AssetIO/tk_lua_variant.h>

namespace elysian {
    using LuaVariant = LuaVariantBase;
}
using LuaVariant    = elysian::LuaVariantBase;
using LuaPair       = elysian::LuaPair;
using LuaFieldRef   = elysian::LuaFieldRef;

#endif

#endif // ELYSIAN_LUA_VARIANT_HPP

