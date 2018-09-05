#ifndef ELYSIAN_PARTY_SYSTEM_PROXY_LUA
#define ELYSIAN_PARTY_SYSTEM_PROXY_LUA

#include <oolua.h>
#include <ESGamma/system/elysian_party_system.hpp>

namespace OOLUA {
    typedef elysian::PartySystem    PartySystem;
}

OOLUA_PROXY(PartySystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, addCharacterToParty, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC(bool, removeCharacterFromParty, in_out_p<const Entity*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getPartyLeader)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getCharacterPartyMember, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getPartySize)
    OOLUA_MEM_FUNC(void, setPartyLeader, Entity*const)
    OOLUA_MEM_FUNC_CONST(int, getPartySlot, const Entity*)
    OOLUA_MEM_FUNC(void, addPlayable, cpp_in_p<Entity*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getPlayable, const char*)
    OOLUA_MEM_FUNC(void, setFollowLogicEnabled, bool)
    OOLUA_MEM_FUNC_CONST(int, getPlayerIndexFromSlot, int)
    OOLUA_MEM_FUNC_CONST(int, getPlayerIndexFromPartyMember, const Entity*)
OOLUA_PROXY_END

#endif // ELYSIAN_PARTY_SYSTEM_PROXY_LUA

