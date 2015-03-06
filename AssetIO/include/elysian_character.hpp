#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#include "elysian_component.hpp"
#include "elysian_direction.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_lua_table_proxy.hpp"

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

	class RigidBody;

	class CharacterAttrib {
		//bool _canHold = true;
	};

    class Character:
        public Component,
        public LuaTableProxy<Character>
    {
        friend class EntityBase;
        friend Entity;
		friend class CharacterSystem;
	private:
		static const Component::TYPE _type		= Component::CHARACTER;
		
	protected:
                Character(Entity& entity);
		virtual ~Character(void) override		= default;

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
        ELYSIAN_LUA_PROXY_FIELDS_END()

        virtual bool        load(FILE* file) override;
        virtual bool        save(FILE* file) override;
        virtual bool        canAddTo(Entity& entity) override { return (entity.getAnimation() && entity.getRigidBody()); }
        virtual bool        addToLevel(void) override;
        virtual bool        removeFromLevel(void) override;
        virtual auto        getType(void) const -> TYPE override { return Component::CHARACTER; }

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

	inline Character::Character(Entity& entity): Component(entity) {}
    inline void Character::setDashMultiplier(const float mult) { _dashMultiplier = mult; }
    inline float Character::getDashMultiplier(void) const { return _dashMultiplier; }
    inline bool Character::isDashing(void) const { return _dashing; }
    inline Direction::DIR4 Character::getFaceDir(void) const { return _faceDir; }
    inline void Character::setWalkAnimInterp(const bool val) { _walkAnimInterp = val; }
    inline Entity* Character::getHeldEntity(void) const { return _heldEntity; }
    inline void Character::setWalkForce(const float force) { _walkForce = force; }
    inline float Character::getWalkForce(void) const { return _walkForce; }
    inline const GYVector3& Character::getLastMoveDir(void) const { return _lastMoveDir; }

	inline bool Character::isFacing(const Entity &entity) const {
		GYVector2 distVec;
        gyVec2Sub(&distVec, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
		return (_faceDir == Direction::fromVector(distVec));
	}

	inline void Character::setAnimate(const bool val) { _animate = val; }


	inline void Character::faceEntity(Entity &entity) {
		GYVector2 dist;
        gyVec2Sub(&dist, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
		faceDir(Direction::fromVector(dist));
	}

}

#endif
