#ifndef ELYSIAN_COMBAT_PROXY_HPP
#define ELYSIAN_COMBAT_PROXY_HPP

#include <crtp/elysian_combat.hpp>
#include <proxy/elysian_component_proxy.hpp>

namespace OOLUA {
    typedef elysian::CombatAttrib           CombatAttrib;
    typedef elysian::CombatStatus           CombatStatus;
    typedef elysian::CombatStatusModifier   CombatStatusModifier;
    typedef elysian::Combat                 Combat;
    typedef elysian::ParticleEmitterAttrib  ParticleEmitterAttrib;
}


OOLUA_PROXY(CombatAttrib)
    OOLUA_MEM_FUNC(void, setTypeFlags, BitMask32)
    OOLUA_MEM_FUNC(void, setTypeAgainstFlags, BitMask32)
    OOLUA_MEM_FUNC(void, setParticleAttrib, cpp_in_p<ParticleEmitterAttrib*>)
    OOLUA_MEM_FUNC(BitMask32&, getTypeFlags)
    OOLUA_MEM_FUNC(BitMask32&, getTypeAgainstFlags)
OOLUA_PROXY_END

OOLUA_PROXY(CombatStatus)
    OOLUA_MEM_FUNC(void, setHp, int)
    OOLUA_MEM_FUNC(void, setMp, int)
    OOLUA_MEM_FUNC(void, setMaxHp, int)
    OOLUA_MEM_FUNC(void, setMaxMp, int)
    OOLUA_MEM_FUNC_CONST(int, getHp)
    OOLUA_MEM_FUNC_CONST(int, getMp)
    OOLUA_MEM_FUNC_CONST(int, getMaxHp)
    OOLUA_MEM_FUNC_CONST(int, getMaxMp)
    OOLUA_MEM_FUNC_CONST(bool, hasStatusEffect, const CombatStatus::STATUS_EFFECT)
OOLUA_PROXY_END

OOLUA_PROXY(CombatStatusModifier, CombatStatus)
    OOLUA_MEM_FUNC(void, addStatusEffect, const int)
    OOLUA_MEM_FUNC(void, removeStatusEffect, const int)
OOLUA_PROXY_END

OOLUA_PROXY(Combat, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setSelfTable, Lua_table_ref)
    OOLUA_MEM_FUNC(void, setActionTable, Lua_table_ref)
    OOLUA_MEM_FUNC(Lua_table_ref&, getSelfTable)
    OOLUA_MEM_FUNC(Lua_table_ref&, getActionTable)
    OOLUA_MEM_FUNC(void, setAttrib, cpp_in_p<CombatAttrib*>)
    OOLUA_MEM_FUNC(void, setDmgType, Combat::DMG_TYPE)
    OOLUA_MEM_FUNC(void, setDmgActive, bool)
    OOLUA_MEM_FUNC(void, setCollisionFlags, BitMask32)
    OOLUA_MEM_FUNC(CombatStatus&, getStatus)
    OOLUA_MEM_FUNC(void, setStatus, const CombatStatus&)
    OOLUA_MEM_FUNC(void, setTargetType, const Combat::TARGET_TYPE)
    OOLUA_MEM_FUNC_CONST(Combat::TARGET_TYPE, getTargetType)
    OOLUA_MEM_FUNC(BitMask32&, getCollisionFlags)
    OOLUA_MEM_FUNC_CONST(bool, getDmgActive)
    OOLUA_MEM_FUNC_CONST(Combat::DMG_TYPE, getDmgType)
    OOLUA_MEM_FUNC_CONST(maybe_null<CombatAttrib*>, getAttrib)
    OOLUA_MEM_FUNC_CONST(float, getAggroDist)
    OOLUA_MEM_FUNC_CONST(float, getDeaggroDist)
    OOLUA_MEM_FUNC(void, setAggroDist, const float)
    OOLUA_MEM_FUNC(void, setDeaggroDist, const float)
    OOLUA_MEM_FUNC_CONST(bool, isInCombat)
OOLUA_PROXY_END



#endif // ELYSIAN_COMBAT_PROXY_HPP

