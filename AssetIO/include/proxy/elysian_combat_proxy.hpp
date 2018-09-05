#ifndef ELYSIAN_COMBAT_PROXY_HPP
#define ELYSIAN_COMBAT_PROXY_HPP

#include <AssetIO/crtp/elysian_combat.hpp>
#include <AssetIO/proxy/elysian_component_proxy.hpp>

namespace OOLUA {
    typedef elysian::CombatAttrib           CombatAttrib;
    typedef elysian::CombatStatus           CombatStatus;
    typedef elysian::CombatStatusModifier   CombatStatusModifier;
    typedef elysian::AggroManager           AggroManager;
    typedef elysian::Combat                 Combat;
    typedef elysian::ParticleEmitterAttrib  ParticleEmitterAttrib;
}


OOLUA_PROXY(CombatAttrib)
    OOLUA_TAGS(Shared)
    OOLUA_MEM_FUNC(void, setTypeFlags, BitMask32)
    OOLUA_MEM_FUNC(void, setTypeAgainstFlags, BitMask32)
    OOLUA_MEM_FUNC(void, setParticleAttrib, std::shared_ptr<ParticleEmitterAttrib>)
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
    OOLUA_MEM_FUNC(void, addStatusEffect, const uint32_t)
    OOLUA_MEM_FUNC(void, removeStatusEffect, const uint32_t)
    OOLUA_MFUNC(setSourceEntityEmnity)
OOLUA_PROXY_END

OOLUA_PROXY(AggroManager)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getEntityAtEmnityPriority, unsigned)
    OOLUA_MFUNC_CONST(getEntityEmnity)
    OOLUA_MFUNC(addEntityEmnity)
    OOLUA_MFUNC(setEntityEmnity)
    OOLUA_MFUNC_CONST(getEmnityTableSize)
    //OOLUA_MFUNC_CONST(getEmnityTableEntry)
    OOLUA_MFUNC_CONST(getAggroedCount)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getAggroedEntity, unsigned)
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
    OOLUA_MEM_FUNC(void, setAttrib, std::shared_ptr<CombatAttrib>)
    OOLUA_MEM_FUNC(void, setDmgType, Combat::DMG_TYPE)
    OOLUA_MEM_FUNC(void, setDmgActive, bool)
    OOLUA_MEM_FUNC(void, setCollisionFlags, BitMask32)
    OOLUA_MEM_FUNC(CombatStatus&, getStatus)
    OOLUA_MEM_FUNC(AggroManager&, getAggroManager)
    OOLUA_MEM_FUNC(void, setStatus, const CombatStatus&)
    OOLUA_MEM_FUNC(void, setTargetType, const Combat::TARGET_TYPE)
    OOLUA_MEM_FUNC_CONST(Combat::TARGET_TYPE, getTargetType)
    OOLUA_MEM_FUNC(BitMask32&, getCollisionFlags)
    OOLUA_MEM_FUNC_CONST(bool, getDmgActive)
    OOLUA_MEM_FUNC_CONST(Combat::DMG_TYPE, getDmgType)
    OOLUA_MEM_FUNC_CONST(maybe_null<std::shared_ptr<CombatAttrib>>, getAttrib)
    OOLUA_MEM_FUNC_CONST(float, getAggroDist)
    OOLUA_MEM_FUNC_CONST(float, getDeaggroDist)
    OOLUA_MEM_FUNC(void, setAggroDist, const float)
    OOLUA_MEM_FUNC(void, setDeaggroDist, const float)
    OOLUA_MEM_FUNC_CONST(bool, isInCombat)
OOLUA_PROXY_END



#endif // ELYSIAN_COMBAT_PROXY_HPP

