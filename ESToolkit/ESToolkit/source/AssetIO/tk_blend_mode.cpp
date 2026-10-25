/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/AssetIO/tk_blend_mode.h>
#include <AssetIO/proxy/elysian_primitive_proxy.hpp>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>

void TkBlendMode::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* typeDelegate = new EnumDropdownPropertyItemDelegate({
       "BLEND_ZERO", "BLEND_ONE", "BLEND_SRC_COLOR", "BLEND_ONE_MINUS_SRC_COLOR", "BLEND_DST_COLOR", "BLEND_ONE_MINUS_DST_COLOR",
       "BLEND_SRC_ALPHA", "BLEND_ONE_MINUS_SRC_ALPHA", "BLEND_DST_ALPHA", "BLEND_ONE_MINUS_DST_ALPHA", "BLEND_CONSTANT_COLOR",
       "BLEND_ONE_MINUS_CONSTANT_COLOR", "BLEND_CONSTANT_ALPHA", "BLEND_ONE_MINUS_CONSTANT_ALPHA", "BLEND_SRC_ALPHA_SATURATE",
       "BLEND_SRC1_COLOR", "BLEND_ONE_MINUS_SRC1_COLOR", "BLEND_SRC1_ALPHA", "BLEND_ONE_MINUS_SRC1_ALPHA"
    });

    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, "srcFactor");

    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, "dstFactor");


    lua_pop(*_mainState, 1);

}
