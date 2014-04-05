#ifndef ELYSIAN_COMBAT_SYSTEM_HPP
#define ELYSIAN_COMBAT_SYSTEM_HPP

#include <oolua.h>
#include "elysian_system.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_gui_system.hpp"

namespace elysian {

	class Entity;
	class Combat;

	class CombatReceipt {
	private:
		char		_text[ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS] = { 0 };
		GYColor4	_textColor = GYColor4 { 255, 255, 255, 255 };
		GYColor4	_partColor = GYColor4 { 255, 255, 255, 255 };
		float		_partIntensityFactor = 1.0f;
	public:

		void setText(const char* text);
		void setTextColor(const GYColor4 &textColor);
		void setPartColor(const GYColor4 &partColor);
		void setPartIntensityFactor(const float factor);
	};

	class CombatSystem: public System,
						public LuaManager,
						public CollisionContactEventMonitor {
	private:

		struct ActionResolution {
			CombatReceipt*		receipt	= nullptr;
			CollisionContact*	contact	= nullptr;
			Combat*				src		= nullptr;
			Combat*				target	= nullptr;
		} _curActionResolution;

		Pool<Entity*>	_combatants;
		unsigned		_bgmTensionLevel = 0;

		void _dealCollisionDamage(Combat*const src, Combat*const target, CollisionContact* contact);
		void _processCurrentCombatAction(void);
	public:

		//reimplemented from CollisionContactEventMonitor
		virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;

		void initialize(void);
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

	inline void CombatReceipt::setText(const char* text) { strcpy(_text, text); }
	inline void CombatReceipt::setTextColor(const GYColor4 &textColor) { _textColor = textColor; }
	inline void CombatReceipt::setPartColor(const GYColor4 &partColor) { _partColor = partColor; }
	inline void CombatReceipt::setPartIntensityFactor(const float factor) { _partIntensityFactor = factor; }

}


#endif // ELYSIAN_COMBAT_SYSTEM_HPP
