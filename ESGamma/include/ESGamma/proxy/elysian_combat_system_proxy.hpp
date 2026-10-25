/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_COMBAT_SYSTEM_PROXY_HPP
#define ELYSIAN_COMBAT_SYSTEM_PROXY_HPP

#include <ESGamma/system/elysian_combat_system.hpp>

namespace OOLUA {

    typedef elysian::CombatSystem           CombatSystem;
    typedef elysian::Combat                 Combat;
    typedef elysian::CombatStatusModifier   CombatStatusModifier;
}


OOLUA_PROXY(CombatSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addCombatant, Entity&)
    OOLUA_MEM_FUNC(void, removeCombatant, Entity&)
    OOLUA_MEM_FUNC(void, killCombatant, Entity&)
    OOLUA_MEM_FUNC_CONST(LuaVariant, getCombatants)
    //OOLUA_MEM_FUNC(void, resolveCombatAction, Lua_table_ref, Combat*, Combat*)
    OOLUA_MEM_FUNC(void, sendStatusModifier, Combat*, CombatStatusModifier*)
    OOLUA_MEM_FUNC(void, setBgmCombatTrack, unsigned)
    OOLUA_MEM_FUNC(void, setBgmDynamic, bool)
    OOLUA_MEM_FUNC(void, clearCombatants)
    OOLUA_MEM_FUNC_CONST(CombatSystem::COMBAT_STATE, getCombatState)
    OOLUA_MEM_FUNC(void, setCombatMgmtType, CombatSystem::COMBAT_MGMT_TYPE)
    OOLUA_MEM_FUNC(void, setCombatEnabled, bool)
OOLUA_PROXY_END

#endif // ELYSIAN_COMBAT_SYSTEM_PROXY_HPP

