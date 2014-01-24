#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#include <fstream>
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

namespace elysian {
	
	class Character: public Component {
        friend class Entity;
	private:
		static const Component::TYPE _type		= Component::CHARACTER;
		
	protected:
		Character(Entity& entity);
		virtual ~Character(void) override		= default;

		bool				_movedThisFrame		= false;
		Direction::DIR4		_faceDir			= Direction::DIR4_CENTER;
		
	public:
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool canAddTo(Entity& entity) override { return (entity.getAnimation() && entity.getAnimation()); }
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual Component::TYPE getType(void) const override { return Component::CHARACTER; }

		Direction::DIR4 getFaceDir(void) const;
		bool isFacing(const Entity& entity) const;

#ifdef ELYSIAN_ENGINE
		void update(void);
		void move(const GYVector2& vec);
		void faceDir(const Direction::DIR4 dir);
		void faceEntity(Entity& entity);
#endif
	};

	inline Character::Character(Entity& entity): Component(entity) {}

	inline Direction::DIR4 Character::getFaceDir(void) const {
		return _faceDir;
	}

	inline bool Character::isFacing(const Entity &entity) const {
		GYVector2 distVec;
		gyVec2Sub(&distVec, &entity.transform._pos, &_entity->transform._pos);
		return (_faceDir == Direction::fromVector(distVec));
	}


#ifdef ELYSIAN_ENGINE

	inline void Character::update(void) {
		if(!_movedThisFrame) {
			faceDir(_faceDir);
		}
		else _movedThisFrame = false;
	}

	inline void Character::faceEntity(Entity &entity) {
		GYVector2 dist;
		gyVec2Sub(&dist, &entity.transform._pos, &_entity->transform._pos);
		faceDir(Direction::fromVector(dist));
	}


#endif


}

#endif
