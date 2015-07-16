#ifndef ELYSIAN_CHARACTER_BASE_HPP
#define ELYSIAN_CHARACTER_BASE_HPP

#include <crtp/elysian_component.hpp>
#include <level/elysian_direction.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>

#define ELYSIAN_CHAR_ANIM_SEQ_STAND_UP		"StandUp"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_DOWN	"StandDown"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_LEFT	"StandLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_RIGHT	"StandRight"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_UP		"WalkUp"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_DOWN		"WalkDown"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_LEFT		"WalkLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_RIGHT	"WalkRight"
#define ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD	"HeldEntity"

#define ELYSIAN_CHAR_WALK_DEFAULT_FORCE         68000000.0f
#define ELYSIAN_CHAR_DASH_DEFAULT_MULTIPLIER    5.0f
#define ELYSIAN_CHAR_JUMP_DEFAULT_FORCE         GYVector3 { 0.0f, /*-330000.0f*/ 0.0f, 330000.0f }
#define ELYSIAN_CHAR_THROW_DEFAULT_FORCE		GYVector3 { 0.0f, /*2000.0f*/0.0f, 25000.0f }

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
	private:
		static const Component::TYPE _type		= Component::CHARACTER;
		
	protected:
                CharacterBase(Entity& entity);
                CharacterBase(const CharacterBase& other);
                CharacterBase(void)                 = default;
                ~CharacterBase(void)        		= default;

        Direction::DIR4		_faceDir			= Direction::DIR4_CENTER;
        GYVector3			_lastMoveDir		= { 0.0f, 0.0f, 0.0f };
        float               _walkForce          = ELYSIAN_CHAR_WALK_DEFAULT_FORCE;
        float               _dashMultiplier     = ELYSIAN_CHAR_DASH_DEFAULT_MULTIPLIER;
        bool				_walkAnimInterp		= true;
        bool				_animate			= true;
        bool				_movedThisFrame		= false;
        bool                _dashing            = false;

        static float		_deltaTime;
        //Direction::DIR4		_prevDir			= Direction::DIR4_CENTER;

        //Pick-up state

        Entity* _heldEntity = nullptr;

    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(walkForce,          getWalkForce,           setWalkForce)
            ELYSIAN_LUA_PROXY_FIELD(dashMultiplier,     getDashMultiplier,      setDashMultiplier)
            ELYSIAN_LUA_PROXY_FIELD(holdingDampFactor,  getHeldEntityForceDampFactor)
            ELYSIAN_LUA_PROXY_FIELD(heldEntity,         getHeldEntity)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        static constexpr const Component::TYPE componentType = Component::CHARACTER;

        Direction::DIR4     getFaceDir(void) const;
        const GYVector3&    getLastMoveDir(void) const;
        bool                isFacing(const Entity& entity) const;
        void                setWalkAnimInterp(const bool val);
        void                setAnimate(const bool val);

        void                setHeldEntity(Entity*const entity);
        float               getHeldEntityForceDampFactor(void);
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
    };


    // ====== INLINEZ ======

    inline CharacterBase::CharacterBase(Entity& entity): Component(entity) {}
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

}

#endif
