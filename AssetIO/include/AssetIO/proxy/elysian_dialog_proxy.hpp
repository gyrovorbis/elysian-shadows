/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DIALOG_PROXY_HPP
#define ELYSIAN_DIALOG_PROXY_HPP

#include <AssetIO/crtp/elysian_dialog.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>

namespace OOLUA {
    typedef elysian::Entity             Entity;
    typedef elysian::DialogScreen       DialogScreen;
    typedef elysian::DialogText         DialogText;
    typedef elysian::DialogOption       DialogOption;
    typedef elysian::DialogPrompt       DialogPrompt;
    typedef elysian::Dialog             Dialog;
}

OOLUA_PROXY(DialogScreen)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(const char*, getText)
    OOLUA_MEM_FUNC(void, setText, const char*)
    OOLUA_MEM_FUNC(void, addSourceEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getEntity)
    OOLUA_MEM_FUNC_CONST(unsigned, getIndex)
    OOLUA_MEM_FUNC(void, setTrigger, bool)
    OOLUA_MEM_FUNC_CONST(bool, isTrigger)
OOLUA_PROXY_END

OOLUA_PROXY(DialogText, DialogScreen)
    OOLUA_MEM_FUNC_CONST_RENAME(getNextIndex, int, getNextIndexLua)
    OOLUA_MEM_FUNC_RENAME(setNextIndex, void, setNextIndexLua, int)
OOLUA_PROXY_END

OOLUA_PROXY(DialogOption)
    OOLUA_MEM_FUNC_CONST(const char*, getText)
    OOLUA_MEM_FUNC(void, setText, const char*)
    OOLUA_MEM_FUNC_CONST_RENAME(getNextIndex, int, getNextIndexLua)
    OOLUA_MEM_FUNC_RENAME(setNextIndex, void, setNextIndexLua, int)
OOLUA_PROXY_END

OOLUA_PROXY(DialogPrompt, DialogScreen)
    OOLUA_MEM_FUNC(void, addOption, const char*, int)
    OOLUA_MEM_FUNC_CONST(unsigned, getOptionCount)
    OOLUA_MEM_FUNC_CONST(maybe_null<DialogOption*>, getOption, unsigned)
    OOLUA_MEM_FUNC(void, setOption, unsigned, cpp_in_p<DialogOption*>)
OOLUA_PROXY_END

OOLUA_PROXY(Dialog)
    OOLUA_TAGS(Shared)
    OOLUA_MEM_FUNC(DialogText*, addTextScreen, const char*)
    OOLUA_MEM_FUNC(DialogPrompt*, addPromptScreen, const char*)
    OOLUA_MEM_FUNC(void, setScreen, const unsigned, cpp_in_p<DialogScreen*>)
    OOLUA_MEM_FUNC_CONST(LuaVariant, getScreenLua, const unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getScreenCount)
    OOLUA_MEM_FUNC(void, setTriggerEntity, Entity*)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getTriggerEntity)
    OOLUA_MEM_FUNC_RENAME(setStartScreen, void, setStartScreenLua, const int)
    OOLUA_MEM_FUNC_CONST_RENAME(getStartScreen, int, getStartScreenLua)
    OOLUA_MEM_FUNC_CONST_RENAME(getScreen, LuaVariant, getScreenLua, const unsigned)
OOLUA_PROXY_END

#endif // ELYSIAN_DIALOG_PROXY_HPP

