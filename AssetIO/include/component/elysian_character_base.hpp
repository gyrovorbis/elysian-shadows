#ifndef ELYSIAN_CHARACTER_BASE_HPP
#define ELYSIAN_CHARACTER_BASE_HPP

#include <crtp/elysian_component.hpp>
#include <level/elysian_direction.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <crtp/elysian_lua_variant.hpp>

#define ELYSIAN_CHAR_ANIM_SEQ_STAND_UP          "StandUp"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_DOWN        "StandDown"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_LEFT        "StandLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_RIGHT       "StandRight"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_UP           "WalkUp"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_DOWN         "WalkDown"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_LEFT         "WalkLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_RIGHT        "WalkRight"
#define ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD        "HeldEntity"

#define ELYSIAN_CHAR_WALK_DEFAULT_FORCE         28000000.0f
#define ELYSIAN_CHAR_DASH_DEFAULT_MULTIPLIER    10.0f
#define ELYSIAN_CHAR_JUMP_DEFAULT_FORCE         GYVector3 { 0.0f, /*-330000.0f*/ 0.0f, 330000.0f }
#define ELYSIAN_CHAR_THROW_DEFAULT_FORCE		GYVector3 { 0.0f, /*2000.0f*/0.0f, 25000.0f }

#define ELYSIAN_ANIMATION_STATES_MAX            50

namespace elysian {

	class CharacterAttrib {
		//bool _canHold = true;
	};

    class CharacterBase:
        public Component,
        public LuaTableProxy<CharacterBase, Character>
    {
        friend class EntityBase;
        friend Entity;
		friend class CharacterSystem;
    public:

        enum ANIM_PLAY_STATE {
            PLAY_START,
            PLAY_DONE,
            PLAY_INTERRUPTED,
            PLAY_STOPPED
        };

        enum ANIM_STATE {
            STATE_STAND,
            STATE_WALK,
            STATE_RUN,
            STATE_DASH,
            STATE_JUMP,
            STATE_FALL,
            STATE_PUSH,
            STATE_DRAG,
            STATE_HOLD,
            STATE_COMBAT,
            STATE_STRAFE,
            STATE_KNOCKBACK,
            STATE_ATTACK,
            STATE_DEATH,
            STATE_COUNT
        };
	private:
		static const Component::TYPE _type		= Component::CHARACTER;
        bool _animStates[ELYSIAN_ANIMATION_STATES_MAX] = { false };
		
	protected:
                            CharacterBase(Entity& entity, const CharacterBase*const prefab=nullptr);
                            CharacterBase(void)                 = default;
                            ~CharacterBase(void)        		= default;

        CharacterBase&      operator=(const CharacterBase& rhs);

        OOLUA::Lua_func_ref _luaCallback;
        int                 _callbackTrack      = 0;
        bool                _tempStateActive    = false;
        Direction::DIR4		_faceDir			= Direction::DIR4_CENTER;
        GYVector3			_lastMoveDir		= { 0.0f, 0.0f, 0.0f };
        float               _walkForce          = ELYSIAN_CHAR_WALK_DEFAULT_FORCE;
        float               _dashMultiplier     = ELYSIAN_CHAR_DASH_DEFAULT_MULTIPLIER;
        bool				_walkAnimInterp		= true;
        bool				_animate			= true;
        bool				_movedThisFrame		= false;
        bool                _dashing            = false;

        //REMOVE ME EVENTUALLY
        mutable LuaVariant  _getAnimNameFromStatesRef;


        static float		_deltaTime;
        //Direction::DIR4		_prevDir			= Direction::DIR4_CENTER;

        //Pick-up state

        Entity*             _heldEntity     = nullptr;
        Entity*             _pullEntity     = nullptr;

        bool                _checkPushing(void);

    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(walkForce,          getWalkForce,           setWalkForce)
            ELYSIAN_LUA_PROXY_FIELD(dashMultiplier,     getDashMultiplier,      setDashMultiplier)
            ELYSIAN_LUA_PROXY_FIELD(holdingDampFactor,  getEntityForceDampFactor)
            ELYSIAN_LUA_PROXY_FIELD(heldEntity,         getHeldEntity)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        static constexpr const Component::TYPE componentType = Component::CHARACTER;

        Direction::DIR4     getFaceDir(void) const;
        void                setFaceDir(const Direction::DIR4 dir);
        const GYVector3&    getLastMoveDir(void) const;
        bool                isFacing(const Entity& entity) const;
        void                setWalkAnimInterp(const bool val);
        void                setAnimate(const bool val);

        bool                canPickUpEntity(Entity*const entity);
        void                setPullEntity(Entity*const entity);
        Entity*             getPullEntity(void) const;
        void                setHeldEntity(Entity*const entity);
        float               getEntityForceDampFactor(void);
        float               getOtherForceDampFactor(void);
        void                throwHeldEntity(void);
        Entity*             getHeldEntity(void) const;
        void                jump(void);
        void                setWalkForce(const float force);
        float               getWalkForce(void) const;
        void                setDashMultiplier(const float mult);
        float               getDashMultiplier(void) const;
        bool                isDashing(void) const;
        void                setDashing(const bool val);

        void                update(void);
        void                moveByDist(const GYVector2& dist);
        void                moveByForce(const GYVector2& vec);
        void                walk(const GYVector2& dir);
        void                faceDir(const Direction::DIR4 dir);
        void                faceEntity(Entity& entity);
        void                animateWalk(const GYVector2& moveVec);

        //animation component wrappers/abstraction
        void                applyKnockbackForce(const GYVector3& force);
        void                playAnimation(const Direction::DIR4 dir);
        void                replaceState(ANIM_STATE name1, ANIM_STATE name2);
        void                removeChildEntity(Entity*entity);
        void                addChildEntity(Entity*entity, const char*const name);
        void                removeState(const ANIM_STATE state);
        void                addState(const ANIM_STATE state);
        void                addTempState(const ANIM_STATE state, const int trackIndex, OOLUA::Lua_func_ref ref);
        bool                hasState(const ANIM_STATE state) const;
        void                setSpeedMultiplier(const float factor);
        const char*         getAnimNameFromStates(const Direction::DIR4 dir) const;
    };

    // ====== INLINEZ ======

    inline CharacterBase::CharacterBase(Entity& entity, const CharacterBase*const prefab): Component(entity) {
        if(prefab) *this = *prefab;
    }
    inline CharacterBase& CharacterBase::operator=(const CharacterBase& rhs) {
        _faceDir            = rhs._faceDir;
        _walkForce          = rhs._walkForce;
        _dashMultiplier     = rhs._dashMultiplier;
        _walkAnimInterp     = rhs._walkAnimInterp;
        _animate            = rhs._animate;
        return *this;
    }
    inline void CharacterBase::setDashMultiplier(const float mult) { _dashMultiplier = mult; }
    inline float CharacterBase::getDashMultiplier(void) const { return _dashMultiplier; }
    inline bool CharacterBase::isDashing(void) const { return _dashing; }
    inline Direction::DIR4 CharacterBase::getFaceDir(void) const { return _faceDir; }
    inline void CharacterBase::setWalkAnimInterp(const bool val) { _walkAnimInterp = val; }
    inline Entity* CharacterBase::getHeldEntity(void) const { return _heldEntity; }
    inline void CharacterBase::setWalkForce(const float force) { _walkForce = force; }
    inline float CharacterBase::getWalkForce(void) const { return _walkForce; }
    inline const GYVector3& CharacterBase::getLastMoveDir(void) const { return _lastMoveDir; }
    inline void CharacterBase::setAnimate(const bool val) { _animate = val; }
    inline void CharacterBase::setFaceDir(const Direction::DIR4 dir) { _faceDir = dir; }
    inline Entity* CharacterBase::getPullEntity(void) const { return _pullEntity; }
    inline void CharacterBase::setPullEntity(Entity * const entity) { _pullEntity = entity; }
    inline bool CharacterBase::hasState(const ANIM_STATE state) const { return _animStates[state]; }



}

#endif
