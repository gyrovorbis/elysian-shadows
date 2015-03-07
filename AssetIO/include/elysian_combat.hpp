#ifndef ELYSIAN_COMBAT_HPP
#define ELYSIAN_COMBAT_HPP

#include <gyro_video_api.h>
#include "elysian_component.hpp"
#include "elysian_lua_table_proxy.hpp"
#include <oolua.h>

#define ELYSIAN_COMBAT_AGGRO_DIST_DEFAULT      200.0f
#define ELYSIAN_COMBAT_DEAGGRO_DIST_DEFAULT    400.0f

namespace elysian {

	class ParticleEmitterAttrib;
    class CombatStatusModifier;

	class CombatAttrib {
	private:
		//GYColor4				_particleColor		= { 255, 255, 255, 255 };
		uint32_t				_typeFlags			= ~0x0;
		uint32_t				_typeAgainstFlags	= ~0x0;
		ParticleEmitterAttrib*	_particleAttrib		= nullptr;

	public:

        uint32_t                getTypeFlags(void) const;
        uint32_t                getTypeAgainstFlags(void) const;
        void                    setTypeFlags(const uint32_t flags);
        void                    setTypeAgainstFlags(const uint32_t flags);
        ParticleEmitterAttrib*  getParticleAttrib(void) const;
        void                    setParticleAttrib(ParticleEmitterAttrib*const attrib);
		//GYColor4 getParticleColor(void) const;
		//void setParticleColor(const GYColor4 color);
	};

    //The only combat-related variables the engine has knowledge of
	class CombatStatus {
	public:
        enum STATUS_EFFECT {
            POISON                  = 1,
            CONFUSION               = 1<<1,
            SLEEP                   = 1<<2,
            MAX                     = 1<<23,
            STATUS_EFFECT_ALL       = ~0
        };

    protected:
        int             _maxHp                  = 100;
        int				_hp                     = 0;
        int             _maxMp                  = 100;
        int				_mp                     = 0;
        //USE STATUS BITFLAGS!!!111
        uint64_t        _statusEffects          = 0;


	public:
        void			applyModifier(const CombatStatusModifier& rvalue);
        bool            hasStatusEffect(const STATUS_EFFECT effect) const;
        int				getHp(void) const;
        int             getMaxHp(void) const;
        int				getMp(void) const;
        int             getMaxMp(void) const;
        void            setHp(const int val);
        void            setMaxHp(const int val);
        void            setMp(const int val);
        void            setMaxMp(const int val);
        void            setStatusEffects(const uint64_t effects);
        uint64_t        getStatusEffects(void) const;
	};


    class CombatStatusModifier: public CombatStatus {
        friend class CombatStatus;
    protected:
        uint64_t        _addRemoveStatusEffects        = 0;

    public:
        void            addStatusEffect(const int effect);
        void            removeStatusEffect(const int effect);
    };

    class Combat:
        public Component,
        public LuaTableProxy<Combat>
    {
	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

		enum COLLISION_FLAGS {
			ON_COLLIDE			= (1<<0),
			ON_COLLISION_ENTER	= (1<<1),
			ON_COLLISION_EXIT	= (1<<2)
		};
		enum DMG_TYPE { DMG_NONE, DMG_SOURCE, DMG_TARGET, DMG_SOURCE_TARGET };
        enum TARGET_TYPE { TARGET_NONE, TARGET_PARTY, TARGET_ENEMY, TARGET_FRIENDLY, TARGET_MAX };

	private:

		OOLUA::Lua_table_ref	_selfTable;
		OOLUA::Lua_table_ref	_actionTable;

		CombatStatus			_status;
		CombatAttrib*			_attrib			= nullptr;
		DMG_TYPE				_dmgType		= DMG_NONE;
		uint32_t				_collisionFlags	= ON_COLLISION_ENTER;
        float                   _aggroDist      = ELYSIAN_COMBAT_AGGRO_DIST_DEFAULT;
        float                   _deaggroDist    = ELYSIAN_COMBAT_DEAGGRO_DIST_DEFAULT;
        TARGET_TYPE             _targetType     = TARGET_NONE;
		bool					_dmgActive		= true;
        bool                    _inCombat       = false;
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


		void					setSelfTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getSelfTable(void);
		void					setActionTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getActionTable(void);

		DMG_TYPE				getDmgType(void) const;
		void					setDmgType(const DMG_TYPE type);
		CombatAttrib*			getAttrib(void) const;
		void					setAttrib(CombatAttrib* attrib);
		void					setCollisionFlags(const uint32_t flags);
		uint32_t				getCollisionFlags(void) const;
		bool					getDmgActive(void) const;
		void					setDmgActive(const bool active);
        TARGET_TYPE             getTargetType(void) const;
        void                    setTargetType(const TARGET_TYPE type);
        CombatStatus    		getStatus(void) const;
        void                    setStatus(const CombatStatus& status);
        void					applyModifier(const CombatStatusModifier& rvalue);

        void                    setInCombat(const bool val);
        bool                    isInCombat(void) const;
        void                    setAggroDist(const float dist);
        float                   getAggroDist(void) const;
        void                    setDeaggroDist(const float dist);
        float                   getDeaggroDist(void) const;

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

    inline void CombatStatus::applyModifier(const CombatStatusModifier& rvalue) {
		_hp += rvalue._hp;
		_mp += rvalue._mp;

        if(_hp < 0) _hp = 0;
        if(_mp < 0) _mp = 0;

        const uint64_t effectsToAdd     = (rvalue._statusEffects&rvalue._addRemoveStatusEffects);
        const uint64_t effectsToRemove  = (rvalue._statusEffects&(~rvalue._addRemoveStatusEffects));

        _statusEffects |= effectsToAdd;
        _statusEffects &= ~effectsToRemove;

	}

	inline int CombatStatus::getHp(void) const { return _hp; }
    inline int CombatStatus::getMaxHp(void) const { return _maxHp; }
	inline int CombatStatus::getMp(void) const { return _mp; }
    inline int CombatStatus::getMaxMp(void) const { return _maxMp; }
    inline void CombatStatus::setStatusEffects(const uint64_t effects) { _statusEffects = effects; }
    inline uint64_t CombatStatus::getStatusEffects(void) const { return _statusEffects; }
    inline bool CombatStatus::hasStatusEffect(const STATUS_EFFECT effect) const { return (_statusEffects&effect); }
	inline void	CombatStatus::setHp(const int hp) { _hp = hp; }
    inline void CombatStatus::setMaxHp(const int val) { _maxHp = val; }
	inline void	CombatStatus::setMp(const int mp) { _mp = mp; }
    inline void CombatStatus::setMaxMp(const int val) { _maxMp = val; }

    inline void CombatStatusModifier::addStatusEffect(const int effect) {
        _statusEffects |= effect;
        _addRemoveStatusEffects |= effect;
    }

    inline void CombatStatusModifier::removeStatusEffect(const int effect) {
        _statusEffects |= effect;
        _addRemoveStatusEffects &= ~effect;
    }

	inline void	Combat::setSelfTable(OOLUA::Lua_table_ref self) { _selfTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getSelfTable(void) { return _selfTable; }
	inline void	Combat::setActionTable(OOLUA::Lua_table_ref self) { _actionTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getActionTable(void) { return _actionTable; }

	inline auto	Combat::getDmgType(void) const -> DMG_TYPE { return _dmgType; }
    inline auto Combat::getTargetType(void) const ->TARGET_TYPE { return _targetType; }
    inline void Combat::setTargetType(const TARGET_TYPE type) { _targetType = type; }
	inline CombatAttrib* Combat::getAttrib(void) const { return _attrib; }
	inline void Combat::setAttrib(CombatAttrib* attrib) { _attrib = attrib; }
	inline void Combat::setCollisionFlags(const uint32_t flags) { _collisionFlags = flags; }
	inline uint32_t Combat::getCollisionFlags(void) const { return _collisionFlags; }
	inline bool Combat::getDmgActive(void) const { return _dmgActive; }
	inline void Combat::setDmgActive(const bool active) { _dmgActive = active; }
    inline CombatStatus Combat::getStatus(void) const { return _status; }
    inline void Combat::setStatus(const CombatStatus& status) {_status = status; }
    inline void	Combat::applyModifier(const CombatStatusModifier& rvalue) { _status.applyModifier(rvalue); }
    inline void Combat::setInCombat(const bool val) { _inCombat = val; }
    inline bool Combat::isInCombat(void) const { return _inCombat; }
    inline void Combat::setAggroDist(const float dist) { _aggroDist = dist; }
    inline float Combat::getAggroDist(void) const { return _aggroDist; }
    inline void Combat::setDeaggroDist(const float dist) { _deaggroDist = dist; }
    inline float Combat::getDeaggroDist(void) const { return _deaggroDist; }

}


#endif // ELYSIAN_COMBAT_HPP
