#ifndef ELYSIAN_COMBAT_SYSTEM_HPP
#define ELYSIAN_COMBAT_SYSTEM_HPP

#include <oolua.h>
#include "elysian_system.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_asset_typedefs.hpp"

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Combat;
	class CombatStatus;
    class CombatStatusModifier;

	class CombatReceipt {
	private:
		char		_text[ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS] = { 0 };
		GYColor4	_textColor = GYColor4 { 255, 255, 255, 255 };
		GYColor4	_partColor = GYColor4 { 255, 255, 255, 255 };
		float		_partIntensityFactor = 1.0f;
	public:

		const char* getText(void);
		GYColor4	getPartColor(void);
		GYColor4	getTextColor(void);
		float		getPartIntensityFactor(void);
		void		setText(const char* text);
		void		setTextColor(const GYColor4 &textColor);
		void		setPartColor(const GYColor4 &partColor);
		void		setPartIntensityFactor(const float factor);
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
			GYVector2			initialVel = { 0.0f, 0.0f };
		} _curActionRes;

		Pool<Entity*>	_combatants;
		unsigned		_bgmCombatTrack = 1;
		bool			_bgmDynamic		= true;

		void _dealCollisionDamage(Combat*const src, Combat*const target, Collidable* bodyB, CollisionContact* contact);
        void _processCurrentCombatAction(CollisionContact*const contact, Combat*const target);
        void _emitCombatModifierText(const Combat*const target, const CombatStatus& modifier);
	public:
		virtual ~CombatSystem(void) = default;

		//reimplemented from CollisionContactEventMonitor
		virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;

		void initialize(void);
		void update(void);

		void addCombatant(Entity& entity);
		void removeCombatant(Entity& entity);
		void killCombatant(Entity& entity);

		void setBgmDynamic(const bool enabled=true);
		void setBgmCombatTrack(const unsigned track);

        /*void resolveCombatAction(OOLUA::Lua_table_ref action,
								 Combat*const src,
                                 Combat*const target);*/

        void sendStatusModifier(Combat*const target, CombatStatusModifier*const modifier);
	};








	// ======== INLINEZ =========

	inline void CombatSystem::setBgmCombatTrack(const unsigned track) { _bgmCombatTrack = track; }
	inline void CombatSystem::setBgmDynamic(const bool enabled) { _bgmDynamic = enabled; }

	inline void CombatReceipt::setText(const char* text) { strcpy(_text, text); }
	inline void CombatReceipt::setTextColor(const GYColor4 &textColor) { _textColor = textColor; }
	inline void CombatReceipt::setPartColor(const GYColor4 &partColor) { _partColor = partColor; }
	inline void CombatReceipt::setPartIntensityFactor(const float factor) { _partIntensityFactor = factor; }
	inline const char* CombatReceipt::getText(void) { return _text; }
	inline GYColor4	CombatReceipt::getPartColor(void) { return _partColor; }
	inline GYColor4	CombatReceipt::getTextColor(void) { return _textColor; }
	inline float CombatReceipt::getPartIntensityFactor(void) { return _partIntensityFactor; }


}


#endif // ELYSIAN_COMBAT_SYSTEM_HPP
