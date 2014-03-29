#ifndef ELYSIAN_COMBAT_SYSTEM_HPP
#define ELYSIAN_COMBAT_SYSTEM_HPP

#include <oolua.h>
#include "elysian_system.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_mem_pool.hpp"

namespace elysian {

	class Entity;
	class Combat;

	class CombatSystem: public System, public LuaManager {
	private:
		Pool<Entity*>	_combatants;
		unsigned		_bgmTensionLevel = 0;

		void _dealCollisionDamage(Combat*const src, Combat*const target);
	public:

		void update(void);

		void addCombatant(Entity& entity);
		void removeCombatant(Entity& entity);
		void killCombatant(Entity*const entity);

		void setBgmTensionLevel(const unsigned level);

		void resolveCombatAction(OOLUA::Lua_table_ref action,
								 Combat*const src,
								 Combat*const target);
	};




	// ======== INLINEZ =========

	inline void CombatSystem::setBgmTensionLevel(const unsigned level) { _bgmTensionLevel = level; }

}


#endif // ELYSIAN_COMBAT_SYSTEM_HPP
