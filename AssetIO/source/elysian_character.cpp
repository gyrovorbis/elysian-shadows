#include "elysian_character.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_animation.hpp"
#include "elysian_generic.hpp"
#include "elysian_rigid_body.hpp"
#include "elysian_collidable.hpp"

namespace elysian {

float Character::_deltaTime = 0.0f;

bool Character::save(FILE* /*file*/) {
	dbgprintf(LOG_VERBOSE, "Character::Save(FILE*)");
	return true;
}

bool Character::load(FILE* /*file*/) {
	dbgprintf(LOG_VERBOSE, "Character::Load(FILE*)");
	
	return true;
}

//O(1)
bool Character::addToLevel(void) {
	_curArea->characters.push_back(this);
        return true;
}

//O(n)
bool Character::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->characters);
}

#ifdef ELYSIAN_ENGINE

void Character::faceDir(const Direction::DIR4 dir) {
	if(_animate) {
		Animation *animComp = _entity->getAnimation();
		switch(dir) {
			case Direction::DIR4_UP:
				animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_STAND_UP);
				break;
			case Direction::DIR4_DOWN:
				animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_STAND_DOWN);
				break;
			case Direction::DIR4_LEFT:
				animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_STAND_LEFT);
				break;
			case Direction::DIR4_RIGHT:
				animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_STAND_RIGHT);
				break;
			default:
				break;
		}
	}
	_faceDir = dir;
}

void Character::move(const GYVector2 &dist) {
	if(gyVec2IsZero(&dist)) return;

	_entity->getRigidBody()->applyForce(GYVector3{ dist.x, dist.y, 0.0f });
	//_entity->transform.translate(dist);
	_movedThisFrame = true;

	Direction::DIR4 dir = Direction::fromVector(dist);

	if(_animate) {
		Animation *animComp = _entity->getAnimation();

		if((_walkAnimInterp && _faceDir == dir) || !_walkAnimInterp) {
			switch(dir) {
				case Direction::DIR4_UP:
					animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_WALK_UP);
					break;
				case Direction::DIR4_DOWN:
					animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_WALK_DOWN);
					break;
				case Direction::DIR4_LEFT:
					animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_WALK_LEFT);
					break;
				case Direction::DIR4_RIGHT:
					animComp->playAnimation(ELYSIAN_CHAR_ANIM_SEQ_WALK_RIGHT);
					break;
				default:
					break;
			}
		}
	}

	_faceDir = dir;
	gyVec2Scale(&_lastMoveDir, &dist, 1.0f/gyVec2Magnitude(&dist));
}

void Character::update(void) {
	if(!_movedThisFrame) {
		faceDir(_faceDir);
		//_entity->getRigidBody()->setVelocity(GYVector3{ 0.0f, 0.0f, 0.0f });
	}
	else _movedThisFrame = false;
}

void Character::setHeldEntity(Entity * const entity) {
	if(_heldEntity) {
		_heldEntity->getRigidBody()->setActive(true);
		_heldEntity->getCollidable()->setActive(true);
		_entity->getAnimation()->removeChildEntity(_heldEntity);
	}

	_heldEntity = entity;
	if(entity) {
		entity->getRigidBody()->setActive(false);
		entity->getRigidBody()->clearKinetic();
		entity->getCollidable()->setActive(false);
		_entity->getAnimation()->addChildEntity(entity, ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD);

	}

}


void Character::throwHeldEntity(void) {
	if(!_heldEntity) return;

	GYVector3 throwForce = {
		_lastMoveDir.x,
		_lastMoveDir.y,
		10.0f
	};
	//throwForce.z +=10000.0f;

	//Set give the object its parent's initial velocity
	_heldEntity->getRigidBody()->clearKinetic();
	_heldEntity->getRigidBody()->setVelocity(_entity->getRigidBody()->getVelocity());
	//_heldEntity->transform._pos = _entity->transform._pos;
	//_heldEntity->transform._pos.y += 64.0f;

	//Also apply a force to "throw" the bitch in the direction we're facing
	gyVec3Scale(&throwForce, &throwForce, 40000.0f/_deltaTime);
	_heldEntity->getRigidBody()->applyForce(throwForce);

	setHeldEntity(nullptr);

}




#endif


}
