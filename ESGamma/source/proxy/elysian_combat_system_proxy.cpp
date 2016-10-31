#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_combat_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS_CONST(CombatSystem)
OOLUA_EXPORT_FUNCTIONS(CombatSystem,
                       addCombatant,
                       removeCombatant,
                       killCombatant,
                       //resolveCombatAction,
                       sendStatusModifier,
                       setBgmCombatTrack,
                       setBgmDynamic)
