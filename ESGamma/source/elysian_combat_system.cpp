#include "elysian_combat_system.hpp"
#include "elysian_combat.hpp"
#include "elysian_area.hpp"
#include "elysian_lua_wrappers.hpp"

namespace elysian {

void CombatSystem::addCombatant(Entity &entity) {

}

void CombatSystem::removeCombatant(Entity &entity) {

}

void CombatSystem::killCombatant(Entity * const entity) {

}

void CombatSystem::resolveCombatAction(OOLUA::Lua_table_ref action, Combat*const src, Combat*const target) {
	OOLUA::Lua_table_ref statMods;

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, action.ref());	//push actionTable
	lua_getfield(*_mainState, -1, "calculateCombatStatMods");
	OOLUA::push(*_mainState, src);
	_protectedCall(1, 1, nullptr);
	OOLUA::pull(*_mainState, statMods);

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, target->getSelfTable().ref()); //push combat target's self table
	lua_getfield(*_mainState, -1, "applyCombatAction");
	_callFunction<void>(nullptr, statMods);

}

void CombatSystem::_dealCollisionDamage(Combat *const src, Combat *const target) {
	//PARTICLES AND SHIT HERE!!!
	resolveCombatAction(src->getActionTable(), src, target);
}

void CombatSystem::update(void) {
	std::vector<Combat*>& combats = getCurArea()->combats;

	for(unsigned i = 0; i < combats.size(); ++i) {

		if(combats[i]->getDmgType() == Combat::DMG_SOURCE) {
			for(unsigned j = i+1; j < combats.size(); ++j) {

				if(combats[j]->getDmgType() == Combat::DMG_TARGET &&
						(combats[i]->getAttrib()->getTypeAgainstFlags() & combats[j]->getAttrib()->getTypeFlags())) {
					_dealCollisionDamage(combats[i], combats[j]);

				}
			}

		}

	}
}


}
