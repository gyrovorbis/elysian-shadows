#ifndef ELYSIAN_COMBAT_HPP
#define ELYSIAN_COMBAT_HPP

#include <gyro_video_api.h>
#include "elysian_component.hpp"
#ifdef ELYSIAN_ENGINE
#	include <oolua.h>
#endif

namespace elysian {

	class ParticleEmitterAttrib;

	class CombatAttrib {
	private:
		//GYColor4				_particleColor		= { 255, 255, 255, 255 };
		uint32_t				_typeFlags			= ~0x0;
		uint32_t				_typeAgainstFlags	= ~0x0;
		ParticleEmitterAttrib*	_particleAttrib		= nullptr;

	public:

		uint32_t getTypeFlags(void) const;
		uint32_t getTypeAgainstFlags(void) const;
		void setTypeFlags(const uint32_t flags);
		void setTypeAgainstFlags(const uint32_t flags);
		ParticleEmitterAttrib* getParticleAttrib(void) const;
		void setParticleAttrib(ParticleEmitterAttrib*const attrib);
		//GYColor4 getParticleColor(void) const;
		//void setParticleColor(const GYColor4 color);
	};

	class CombatStatus {
	public:
		enum STATUS_EFFECT_STATE {
			STATUS_EFFECT_NONE,
			STATUS_EFFECT_ENABLE,
			STATUS_EFFECT_DISABLE
		};

	private:

		int				_hp				= 0;
		int				_mp				= 0;
		STATUS_EFFECT_STATE	_poison		= STATUS_EFFECT_NONE;
		STATUS_EFFECT_STATE	_sleep		= STATUS_EFFECT_NONE;
		STATUS_EFFECT_STATE	_confusion	= STATUS_EFFECT_NONE;

	public:

		void				applyCombatStatus(const CombatStatus& rvalue);

		int					getHp(void) const;
		int					getMp(void) const;
		STATUS_EFFECT_STATE getPoison(void) const;
		STATUS_EFFECT_STATE getSleep(void) const;
		STATUS_EFFECT_STATE getConfusion(void) const;

		void				setHp(const int hp);
		void				setMp(const int mp);
		void				setPoison(const STATUS_EFFECT_STATE state);
		void				setSleep(const STATUS_EFFECT_STATE state);
		void				setConfusion(const STATUS_EFFECT_STATE state);
	};

	class Combat: public Component {
	public:
		enum COLLISION_FLAGS {
			ON_COLLIDE			= (1<<0),
			ON_COLLISION_ENTER	= (1<<1),
			ON_COLLISION_EXIT	= (1<<2)
		};
		enum DMG_TYPE { DMG_NONE, DMG_SOURCE, DMG_TARGET, DMG_SOURCE_TARGET };

	private:

#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref	_selfTable;
		OOLUA::Lua_table_ref	_actionTable;
#endif
		CombatStatus			_status;
		CombatAttrib*			_attrib			= nullptr;
		DMG_TYPE				_dmgType		= DMG_NONE;
		uint32_t				_collisionFlags	= ON_COLLISION_ENTER;
		bool					_dmgActive		= true;
	public:

				Combat(Entity& entity);
		virtual ~Combat(void) override = default;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

#ifdef ELYSIAN_ENGINE
		void					setSelfTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getSelfTable(void);
		void					setActionTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getActionTable(void);
#endif

		DMG_TYPE				getDmgType(void) const;
		void					setDmgType(const DMG_TYPE type);
		CombatAttrib*			getAttrib(void) const;
		void					setAttrib(CombatAttrib* attrib);
		void					setCollisionFlags(const uint32_t flags);
		uint32_t				getCollisionFlags(void) const;
		bool					getDmgActive(void) const;
		void					setDmgActive(const bool active);
		const CombatStatus&		getStatus(void) const;
		void					applyStatus(const CombatStatus& rvalue);

	};


	// ======== INLINEZ ========

	inline uint32_t CombatAttrib::getTypeFlags(void) const { return _typeFlags; }
	inline uint32_t CombatAttrib::getTypeAgainstFlags(void) const { return _typeAgainstFlags; }
	inline void CombatAttrib::setTypeFlags(const uint32_t flags) { _typeFlags = flags; }
	inline void CombatAttrib::setTypeAgainstFlags(const uint32_t flags) { _typeAgainstFlags = flags; }
	inline ParticleEmitterAttrib* CombatAttrib::getParticleAttrib(void) const { return _particleAttrib; }
	inline void CombatAttrib::setParticleAttrib(ParticleEmitterAttrib* attrib) { _particleAttrib = attrib; }
	//inline GYColor4 CombatAttrib::getParticleColor(void) const { return _particleColor; }
	//inline void CombatAttrib::setParticleColor(const GYColor4 color) { _particleColor = color; }

	inline void CombatStatus::applyCombatStatus(const CombatStatus& rvalue) {
		auto checkApplyStatusEffect = [](STATUS_EFFECT_STATE& lvalue,
									const STATUS_EFFECT_STATE rvalue)
		{
			if(rvalue != STATUS_EFFECT_NONE) {
				lvalue = rvalue;
			}
		};

		_hp += rvalue._hp;
		_mp += rvalue._mp;
		checkApplyStatusEffect(_poison, rvalue._poison);
		checkApplyStatusEffect(_sleep, rvalue._sleep);
		checkApplyStatusEffect(_confusion, rvalue._confusion);
	}

	inline int CombatStatus::getHp(void) const { return _hp; }
	inline int CombatStatus::getMp(void) const { return _mp; }
	inline auto CombatStatus::getPoison(void) const -> STATUS_EFFECT_STATE { return _poison; }
	inline auto CombatStatus::getSleep(void) const -> STATUS_EFFECT_STATE { return _sleep; }
	inline auto CombatStatus::getConfusion(void) const -> STATUS_EFFECT_STATE { return _confusion; }

	inline void	CombatStatus::setHp(const int hp) { _hp = hp; }
	inline void	CombatStatus::setMp(const int mp) { _mp = mp; }
	inline void	CombatStatus::setPoison(const STATUS_EFFECT_STATE state) { _poison = state; }
	inline void	CombatStatus::setSleep(const STATUS_EFFECT_STATE state) { _sleep = state; }
	inline void	CombatStatus::setConfusion(const STATUS_EFFECT_STATE state) { _confusion = state; }


#ifdef ELYSIAN_ENGINE

	inline void	Combat::setSelfTable(OOLUA::Lua_table_ref self) { _selfTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getSelfTable(void) { return _selfTable; }
	inline void	Combat::setActionTable(OOLUA::Lua_table_ref self) { _actionTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getActionTable(void) { return _actionTable; }

#endif

	inline auto	Combat::getDmgType(void) const -> DMG_TYPE { return _dmgType; }
	inline CombatAttrib* Combat::getAttrib(void) const { return _attrib; }
	inline void Combat::setAttrib(CombatAttrib* attrib) { _attrib = attrib; }
	inline void Combat::setCollisionFlags(const uint32_t flags) { _collisionFlags = flags; }
	inline uint32_t Combat::getCollisionFlags(void) const { return _collisionFlags; }
	inline bool Combat::getDmgActive(void) const { return _dmgActive; }
	inline void Combat::setDmgActive(const bool active) { _dmgActive = active; }
	inline const CombatStatus& Combat::getStatus(void) const { return _status; }
	inline void	Combat::applyStatus(const CombatStatus& rvalue) { _status.applyCombatStatus(rvalue); }
}


#endif // ELYSIAN_COMBAT_HPP
