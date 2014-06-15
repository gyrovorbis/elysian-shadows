#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#include "elysian_component.hpp"
#include "elysian_direction.hpp"
#include "elysian_entity.hpp"

#define ELYSIAN_CHAR_ANIM_SEQ_STAND_UP		"StandUp"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_DOWN	"StandDown"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_LEFT	"StandLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_STAND_RIGHT	"StandRight"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_UP		"WalkUp"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_DOWN		"WalkDown"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_LEFT		"WalkLeft"
#define ELYSIAN_CHAR_ANIM_SEQ_WALK_RIGHT	"WalkRight"
#define ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD	"HeldEntity"

#define ELYSIAN_CHAR_JUMP_DEFAULT_FORCE		GYVector3{ 0.0f, /*-330000.0f*/ 0.0f, 330000.0f };

namespace elysian {

	class RigidBody;

	class CharacterAttrib {
		//bool _canHold = true;
	};

	class Character: public Component {
        friend class Entity;
		friend class CharacterSystem;
	private:
		static const Component::TYPE _type		= Component::CHARACTER;
		
	protected:
		Character(Entity& entity);
		virtual ~Character(void) override		= default;

		bool				_movedThisFrame		= false;
		Direction::DIR4		_faceDir			= Direction::DIR4_CENTER;
		GYVector2			_lastMoveDir		= { 0.0f, 0.0f };
		bool				_walkAnimInterp		= true;
		bool				_animate			= true;
		static float		_deltaTime;
		//Direction::DIR4		_prevDir			= Direction::DIR4_CENTER;

		//Pick-up state

		Entity* _heldEntity = nullptr;
		
	public:
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool canAddTo(Entity& entity) override { return (entity.getAnimation() && entity.getRigidBody()); }
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual Component::TYPE getType(void) const override { return Component::CHARACTER; }

		Direction::DIR4 getFaceDir(void) const;
		bool			isFacing(const Entity& entity) const;
		void			setWalkAnimInterp(const bool val);
		void			setAnimate(const bool val);

		void			setHeldEntity(Entity*const entity);
		float			getHeldEntityForceDampFactor(void);
		void			throwHeldEntity(void);
		Entity*			getHeldEntity(void) const;
		void			jump(void);

#ifdef ELYSIAN_ENGINE
		void update(void);
		void move(const GYVector2& vec);
		void faceDir(const Direction::DIR4 dir);
		void faceEntity(Entity& entity);
#endif
	};


	// ====== INLINEZ ======

	inline Character::Character(Entity& entity): Component(entity) {}

	inline Direction::DIR4 Character::getFaceDir(void) const {
		return _faceDir;
	}

	inline bool Character::isFacing(const Entity &entity) const {
		GYVector2 distVec;
		gyVec2Sub(&distVec, (GYVector2*)&entity.transform._pos, (GYVector2*)&_entity->transform._pos);
		return (_faceDir == Direction::fromVector(distVec));
	}

	inline void Character::setWalkAnimInterp(const bool val) { _walkAnimInterp = val; }

	inline Entity* Character::getHeldEntity(void) const { return _heldEntity; }


#ifdef ELYSIAN_ENGINE

	inline void Character::setAnimate(const bool val) { _animate = val; }


	inline void Character::faceEntity(Entity &entity) {
		GYVector2 dist;
		gyVec2Sub(&dist, (GYVector2*)&entity.transform._pos, (GYVector2*)&_entity->transform._pos);
		faceDir(Direction::fromVector(dist));
	}


#endif


}

#endif
