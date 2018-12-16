#ifndef ELYSIAN_COMBAT_BASE_HPP
#define ELYSIAN_COMBAT_BASE_HPP

#include <libGyro/gyro_video_api.h>
#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <AssetIO/utility/elysian_bit_mask.hpp>
#include <queue>

#define ELYSIAN_COMBAT_AGGRO_DIST_DEFAULT      200.0f
#define ELYSIAN_COMBAT_DEAGGRO_DIST_DEFAULT    400.0f

ELYSIAN_ASSET_TYPEDEF(ParticleEmitterAttrib)
ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {


    class AggroManager {
    private:
        struct EmnityEntry {
            Entity* 	entity	= nullptr;
            int16_t		emnity	= 0;
        };

        std::vector<EmnityEntry>	_emnityTable;
        std::vector<Entity*>		_aggroedList;
        Entity*						_parentEntity	= nullptr;
        Entity*						_targetEntity 	= nullptr;

        void		_updateEmnityTable(void);
        void		_updateTargetEntity(void);

    public:
                    AggroManager(void) = default;
                    AggroManager(Entity* parent);
                    ~AggroManager(void);
        void        clear(void);

        //Emnity Management
        Entity*     getEntityAtEmnityPriority(unsigned index) const;
        unsigned    getEntityEmnity(Entity* entity) const;              //0 if not in table, saves hasEntity() lua wrapper
        void        addEntityEmnity(Entity* entity, int16_t emnityDelta);
        void        setEntityEmnity(Entity* entity, int16_t emnityVal); //0 removes it from the list, save helper Lua wrapper

        unsigned    getEmnityTableSize(void) const;
        void        getEmnityTableEntry(unsigned index, Entity*& entity, int16_t& emnity) const;

        void        clearEmnityTable(void);

        //Aggroed Entity Management
        unsigned    getAggroedCount(void) const;
        Entity*     getAggroedEntity(unsigned index) const;
        void        addAggroedEntity(Entity* entity);
        void        removeAggroedEntity(Entity* entity);
        void        clearAggroedList(void);

    };


    class CombatStatusModifier;

    class CombatAttrib: public LuaTableProxy<CombatAttrib> {
         ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(type,           getTypeFlags,           setTypeFlags)
            ELYSIAN_LUA_PROXY_FIELD(typeAgainst,    getTypeAgainstFlags,    setTypeAgainstFlags)
         ELYSIAN_LUA_PROXY_FIELDS_END()
    private:
        //GYColor4				_particleColor		= { 255, 255, 255, 255 };
        BitMask32                               _typeFlags;//= BitMask32(~0x0);
        BitMask32                               _typeAgainstFlags;//= BitMask32(~0x0);
        std::shared_ptr<ParticleEmitterAttrib>	_particleAttrib;

    public:

        BitMask32&                  getTypeFlags(void);
        BitMask32&                  getTypeAgainstFlags(void);
        void                        setTypeFlags(const BitMask32 flags);
        void                        setTypeAgainstFlags(const BitMask32 flags);
        auto                        getParticleAttrib(void) const -> std::shared_ptr<ParticleEmitterAttrib>;
        void                        setParticleAttrib(std::shared_ptr<ParticleEmitterAttrib> attrib);
        //GYColor4 getParticleColor(void) const;
        //void setParticleColor(const GYColor4 color);
    };

    //The only combat-related variables the engine has knowledge of
    class CombatStatus: public LuaTableProxy<CombatStatus> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(maxHp,  getMaxHp,   setMaxHp)
            ELYSIAN_LUA_PROXY_FIELD(hp,     getHp,      setHp)
            ELYSIAN_LUA_PROXY_FIELD(maxMp,  getMaxMp,   setMaxMp)
            ELYSIAN_LUA_PROXY_FIELD(mp,     getMp,      setMp)
        ELYSIAN_LUA_PROXY_FIELDS_END()

    public:
        enum STATUS_EFFECT {
            POISON                  = 1u,
            CONFUSION               = 1u<<1,
            SLEEP                   = 1u<<2,
            MAX                     = 1u<<23,
            STATUS_EFFECT_ALL       = ~0
        };

    protected:
        int             _maxHp                  = 100;
        int				_hp                     = 0;
        int             _maxMp                  = 100;
        int				_mp                     = 0;
        //USE STATUS BITFLAGS!!!111
        uint32_t        _statusEffects          = 0;


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
        void            setStatusEffects(const uint32_t effects);
        uint32_t        getStatusEffects(void) const;
    };


    class CombatStatusModifier: public CombatStatus {
        friend class CombatStatus;
    protected:
        Entity*         _srcEntity                  = nullptr;
        uint32_t        _addRemoveStatusEffects     = 0;
        int16_t         _emnity                     = 0;

    public:
        void            addStatusEffect(const uint32_t effect);
        void            removeStatusEffect(const uint32_t effect);
        Entity*         getSourceEntity(void) const;
        void            setSourceEntityEmnity(Entity* src, int16_t emnity);
        void            setSourceEntity(Entity* src);
        int16_t         getEmnity(void) const;
        void            setEmnity(int16_t value);
    };

    class CombatBase:
        public Component,
        public LuaTableProxy<CombatBase, Combat>
    {
	public:
        friend Combat;

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(dmgType,        getDmgType,         setDmgType)
            ELYSIAN_LUA_PROXY_FIELD(collisionFlags, getCollisionFlags,  setCollisionFlags)
            ELYSIAN_LUA_PROXY_FIELD(aggroDist,      getAggroDist,       setAggroDist)
            ELYSIAN_LUA_PROXY_FIELD(deaggroDist,    getDeaggroDist,     setDeaggroDist)
            ELYSIAN_LUA_PROXY_FIELD(targetType,     getTargetType,      setTargetType)
            ELYSIAN_LUA_PROXY_FIELD(inCombat,       isInCombat)
            ELYSIAN_LUA_PROXY_FIELD(status,         getStatus,          setStatus)
            ELYSIAN_LUA_PROXY_FIELD(attrib,         getAttrib,          setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        enum COLLISION_FLAGS {
            ON_COLLIDE			= (1u<<0),
            ON_COLLISION_ENTER	= (1u<<1),
            ON_COLLISION_EXIT	= (1u<<2)
        };
        enum DMG_TYPE { DMG_NONE, DMG_SOURCE, DMG_TARGET, DMG_SOURCE_TARGET };
        enum TARGET_TYPE { TARGET_NONE, TARGET_PARTY, TARGET_ENEMY, TARGET_FRIENDLY, TARGET_MAX };

    private:
        AggroManager                    _aggroMgr;
        CombatStatus                    _status;
        std::shared_ptr<CombatAttrib>	_attrib;
        DMG_TYPE                        _dmgType            = DMG_NONE;
        BitMask32                       _collisionFlags     = ON_COLLISION_ENTER;
        float                           _aggroDist          = ELYSIAN_COMBAT_AGGRO_DIST_DEFAULT;
        float                           _deaggroDist        = ELYSIAN_COMBAT_DEAGGRO_DIST_DEFAULT;
        TARGET_TYPE                     _targetType         = TARGET_NONE;
        bool                            _dmgActive          = true;
       // bool                            _dmgEnabled         = true;
        bool                            _inCombat           = false;
        bool                            _dmgRecovering      = false;
        float                           _dmgRecoveryTime    = 0.5f;
        float                           _dmgRecoveryElapsed = 0.0f;

    public:
                                CombatBase(void) = default;
                                CombatBase(Entity& entity, CombatBase*const prefab=nullptr);
                                ~CombatBase(void) = default;

        void                    update(void);

        CombatBase&             operator=(CombatBase& rhs);

		//reimplemented from component
        static constexpr const  Component::TYPE componentType = Component::COMBAT;

        AggroManager&           getAggroManager(void);
		DMG_TYPE				getDmgType(void) const;
		void					setDmgType(const DMG_TYPE type);
        auto        			getAttrib(void) const -> std::shared_ptr<CombatAttrib>;
        void					setAttrib(std::shared_ptr<CombatAttrib> attrib);
        void					setCollisionFlags(const BitMask32 flags);
        BitMask32&				getCollisionFlags(void);
        void					setDmgActive(const bool enabled);
        bool                    getDmgActive(void) const;
      //  bool                    getDmgActive(void) const;
        TARGET_TYPE             getTargetType(void) const;
        void                    setTargetType(const TARGET_TYPE type);
        CombatStatus&    		getStatus(void);
        void                    setStatus(const CombatStatus& status);
        void					applyModifier(const CombatStatusModifier& rvalue);
        void                    startDmgRecoveryTimer(void);
        void                    setInCombat(const bool val);
        bool                    isInCombat(void) const;
        void                    setAggroDist(const float dist);
        float                   getAggroDist(void) const;
        void                    setDeaggroDist(const float dist);
        float                   getDeaggroDist(void) const;
        bool                    isDead(void) const;

    };


    // ======== INLINEZ ========

    inline BitMask32& CombatAttrib::getTypeFlags(void) { return _typeFlags; }
    inline BitMask32& CombatAttrib::getTypeAgainstFlags(void) { return _typeAgainstFlags; }
    inline void CombatAttrib::setTypeFlags(const BitMask32 flags) { _typeFlags = flags; }
    inline void CombatAttrib::setTypeAgainstFlags(const BitMask32 flags) { _typeAgainstFlags = flags; }
    inline std::shared_ptr<ParticleEmitterAttrib> CombatAttrib::getParticleAttrib(void) const { return _particleAttrib; }
    inline void CombatAttrib::setParticleAttrib(std::shared_ptr<ParticleEmitterAttrib> attrib) { _particleAttrib = attrib; }
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
    inline void CombatStatus::setStatusEffects(const uint32_t effects) { _statusEffects = effects; }
    inline uint32_t CombatStatus::getStatusEffects(void) const { return _statusEffects; }
    inline bool CombatStatus::hasStatusEffect(const STATUS_EFFECT effect) const { return (_statusEffects&effect); }
    inline void	CombatStatus::setHp(const int hp) { _hp = hp; }
    inline void CombatStatus::setMaxHp(const int val) { _maxHp = val; }
    inline void	CombatStatus::setMp(const int mp) { _mp = mp; }
    inline void CombatStatus::setMaxMp(const int val) { _maxMp = val; }

    inline void CombatStatusModifier::addStatusEffect(const uint32_t effect) {
        _statusEffects |= effect;
        _addRemoveStatusEffects |= effect;
    }

    inline void CombatStatusModifier::removeStatusEffect(const uint32_t effect) {
        _statusEffects |= effect;
        _addRemoveStatusEffects &= ~effect;
    }

    inline void CombatStatusModifier::setSourceEntity(Entity *src) { _srcEntity = src; }
    inline Entity* CombatStatusModifier::getSourceEntity(void) const { return _srcEntity; }
    inline int16_t CombatStatusModifier::getEmnity(void) const { return _emnity; }
    inline void CombatStatusModifier::setEmnity(int16_t value) { _emnity = value; }
    inline void CombatStatusModifier::setSourceEntityEmnity(Entity* src, int16_t emnity) {
        setSourceEntity(src);
        setEmnity(emnity);
    }

    inline CombatBase::CombatBase(Entity &entity, CombatBase*const prefab):
        Component(entity),
        _aggroMgr(&entity)
    {
        if(prefab) *this = *prefab;
    }

    inline CombatBase& CombatBase::operator=(CombatBase& rhs) {
        _status             = rhs._status;
        _attrib             = rhs._attrib;
        _dmgType            = rhs._dmgType;
        _collisionFlags     = rhs._collisionFlags;
        _aggroDist          = rhs._aggroDist;
        _deaggroDist        = rhs._deaggroDist;
        _targetType         = rhs._targetType;
        _dmgActive          = rhs._dmgActive;
        _dmgRecovering      = rhs._dmgRecovering;
        _dmgRecoveryTime    = rhs._dmgRecoveryTime;
        _dmgRecoveryElapsed = rhs._dmgRecoveryElapsed;
        return *this;
    }
    inline auto	CombatBase::getDmgType(void) const -> DMG_TYPE { return _dmgType; }
    inline auto CombatBase::getTargetType(void) const ->TARGET_TYPE { return _targetType; }
    inline void CombatBase::setTargetType(const TARGET_TYPE type) { _targetType = type; }
    inline std::shared_ptr<CombatAttrib> CombatBase::getAttrib(void) const { return _attrib; }
    inline void CombatBase::setAttrib(std::shared_ptr<CombatAttrib> attrib) { _attrib = attrib; }
    inline void CombatBase::setCollisionFlags(const BitMask32 flags) { _collisionFlags = flags; }
    inline BitMask32& CombatBase::getCollisionFlags(void) { return _collisionFlags; }
    inline void CombatBase::setDmgActive(const bool active) { _dmgActive = active; }
    inline bool CombatBase::getDmgActive(void) const { return _dmgActive && !_dmgRecovering; }
    inline CombatStatus& CombatBase::getStatus(void) { return _status; }
    inline void CombatBase::setStatus(const CombatStatus& status) {_status = status; }
    inline void	CombatBase::applyModifier(const CombatStatusModifier& rvalue) { _status.applyModifier(rvalue); }
    inline void CombatBase::setInCombat(const bool val) { _inCombat = val; }
    inline bool CombatBase::isInCombat(void) const { return _inCombat; }
    inline void CombatBase::setAggroDist(const float dist) { _aggroDist = dist; }
    inline float CombatBase::getAggroDist(void) const { return _aggroDist; }
    inline void CombatBase::setDeaggroDist(const float dist) { _deaggroDist = dist; }
    inline float CombatBase::getDeaggroDist(void) const { return _deaggroDist; }
    inline void CombatBase::startDmgRecoveryTimer(void) {
        _dmgRecovering = true;
        _dmgRecoveryElapsed = 0.0f;
    }
    inline AggroManager& CombatBase::getAggroManager(void) { return _aggroMgr; }

    //===== AGGROMANAGER INLINEZ ======

    inline AggroManager::AggroManager(Entity* parent):
        _parentEntity(parent)
    {}

    inline AggroManager::~AggroManager(void) {
        clear();
    }

    inline void AggroManager::clear(void) {
        clearEmnityTable();
        clearAggroedList();
    }

    //Aggro
    inline unsigned AggroManager::getAggroedCount(void) const {
        return _aggroedList.size();
    }

    inline Entity* AggroManager::getAggroedEntity(unsigned index) const {
        return (index < _aggroedList.size())? _aggroedList[index] : nullptr;
    }

    inline void AggroManager::addAggroedEntity(Entity *entity) {
        _aggroedList.push_back(entity);
    }

    inline void AggroManager::removeAggroedEntity(Entity *entity) {
        for(auto it = _aggroedList.begin(); it != _aggroedList.end(); ++it) {
            if((*it) == entity) {
                _aggroedList.erase(it);
                return;
            }
        }
    }

    inline void AggroManager::clearAggroedList(void) {
        _aggroedList.clear();
    }



    inline Entity* AggroManager::getEntityAtEmnityPriority(unsigned index) const {
        return (index < _emnityTable.size())? _emnityTable[index].entity : nullptr;
    }

    inline unsigned AggroManager::getEntityEmnity(Entity *entity) const {
        for(auto&& it : _emnityTable) {
            if(it.entity == entity) return it.emnity;
        }
        return 0;
    }

    inline unsigned AggroManager::getEmnityTableSize(void) const {
        return _emnityTable.size();
    }

    inline void AggroManager::getEmnityTableEntry(unsigned index, Entity*& entity, int16_t &emnity) const {
        assert(index < _emnityTable.size());
        entity = _emnityTable[index].entity;
        emnity = _emnityTable[index].emnity;
    }

    inline void AggroManager::_updateEmnityTable(void) {
        if(_emnityTable.size()) {
            std::sort(_emnityTable.begin(), _emnityTable.end(),
                [](const EmnityEntry& lhs, const EmnityEntry& rhs) {
                    return (lhs.emnity > rhs.emnity);
                });
        }

        _updateTargetEntity();
    }

    inline void AggroManager::clearEmnityTable(void) {
        _emnityTable.clear();
        _updateTargetEntity();
    }


}


#endif // ELYSIAN_COMBAT_HPP
