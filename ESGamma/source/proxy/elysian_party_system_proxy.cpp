#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_party_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS(PartySystem,
                        addCharacterToParty,
                        removeCharacterFromParty,
                        setPartyLeader,
                        addPlayable)
OOLUA_EXPORT_FUNCTIONS_CONST(PartySystem,
                                getPartyLeader,
                                getCharacterPartyMember,
                                getPartySize,
                                getPartySlot,
                                getPlayable)
