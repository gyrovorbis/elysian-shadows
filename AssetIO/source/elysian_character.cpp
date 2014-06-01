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

/* If we are currently holding another entity, their mass must be taken into account when applying
 * our movement force, so that heavy objects slow us down.
 * accel = force*inverseMass
 * accel = force*(inverseMassParent + inverseMassChild)
 */
float Character::getHeldEntityForceDampFactor(void) {
	const float invParent = _entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
	const float invChild = _heldEntity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;

	//Don't divid by zero, dipshit.
	if(invParent+invChild == 0.0f) return 0.0f;

	return (invChild/(invChild+invParent));
}

void Character::move(const GYVector2 &dist) {
	if(gyVec2IsZero(&dist)) return;
	GYVector2 totalForce = dist;

	if(_heldEntity) {
		gyVec2Scale(&totalForce, &dist, getHeldEntityForceDampFactor());
	}

	_entity->getRigidBody()->applyForce(GYVector3{ totalForce.x, totalForce.y, 0.0f });
	//_entity->transform.translate(dist);
	_movedThisFrame = true;

	Direction::DIR4 dir = Direction::fromVector(totalForce);

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
	gyVec2Scale(&_lastMoveDir, &totalForce, 1.0f/gyVec2Magnitude(&totalForce));
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
        //0.8f
        0.1f
	};
	gyVec3Scale(&throwForce, &throwForce, 300000.0f/_deltaTime);

	//Set give the object its parent's initial velocity
	_heldEntity->getRigidBody()->clearKinetic();
	_heldEntity->getRigidBody()->setVelocity(_entity->getRigidBody()->getVelocity());
	_entity->getRigidBody()->clearKinetic();

	//Also apply a force to "throw" the bitch in the direction we're facing
	_heldEntity->getRigidBody()->applyForce(throwForce);

	setHeldEntity(nullptr);
}

void Character::jump(void) {
	GYVector3 force = ELYSIAN_CHAR_JUMP_DEFAULT_FORCE;

	if(_heldEntity) {
		gyVec3ScaleAccum(&force, getHeldEntityForceDampFactor());
	}

	gyVec3ScaleAccum(&force, 1.0f/_deltaTime);
	_entity->getRigidBody()->applyForce(force);

}


#endif


}
