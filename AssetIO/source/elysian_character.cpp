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
				animComp->playAnimation("Up");
				break;
			case Direction::DIR4_DOWN:
				animComp->playAnimation("Down");
				break;
			case Direction::DIR4_LEFT:
				animComp->playAnimation("Left");
				break;
			case Direction::DIR4_RIGHT:
				animComp->playAnimation("Right");
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

#if 0
	if(_heldEntity) {
		gyVec2Scale(&totalForce, &dist, getHeldEntityForceDampFactor());
	}
#endif

	_entity->getRigidBody()->applyForce(GYVector3{ totalForce.x, totalForce.y, 0.0f });
	//_entity->transform.translate(dist);
	_movedThisFrame = true;
	_entity->getAnimation()->replaceState("Stand", "Walk");

	Direction::DIR4 dir = Direction::fromVector(totalForce);

	if(_animate) {
		Animation *animComp = _entity->getAnimation();

		if((_walkAnimInterp && _faceDir == dir) || !_walkAnimInterp) {
			switch(dir) {
				case Direction::DIR4_UP:
					animComp->playAnimation("Up");
					break;
				case Direction::DIR4_DOWN:
					animComp->playAnimation("Down");
					break;
				case Direction::DIR4_LEFT:
					animComp->playAnimation("Left");
					break;
				case Direction::DIR4_RIGHT:
					animComp->playAnimation("Right");
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
		_entity->getAnimation()->replaceState("Walk", "Stand");
		faceDir(_faceDir);
		//_entity->getRigidBody()->setVelocity(GYVector3{ 0.0f, 0.0f, 0.0f });
	}
	else _movedThisFrame = false;

    //Set falling animation state
    if(_entity->getRigidBody()->getVelocity().z < 0.0f) _entity->getAnimation()->replaceState("Jump", "Fall");
}

void Character::setHeldEntity(Entity * const entity) {
	if(_heldEntity) {
		_heldEntity->getRigidBody()->setActive(true);
		_heldEntity->getCollidable()->setActive(true);
		_entity->getAnimation()->removeChildEntity(_heldEntity);
	}

	_heldEntity = entity;
	if(entity) {
        if(!entity->getRigidBody()->getAttrib()->canHold()) {
            _heldEntity = nullptr;
        } else {
            entity->getRigidBody()->setActive(false);
            entity->getRigidBody()->clearKinetic();
            entity->getCollidable()->setActive(false);
            _entity->getAnimation()->addChildEntity(entity, ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD);
            _entity->getAnimation()->removeState("Walk");
            _entity->getAnimation()->removeState("Stand");
            _entity->getAnimation()->addState("Hold");
        }
    } else {
        _entity->getAnimation()->removeState("Hold");
    }

}

void Character::throwHeldEntity(void) {
	if(!_heldEntity) return;

    GYVector3 startPosOffset;
    GYVector3 velocity;
    GYVector3 throwForce = ELYSIAN_CHAR_THROW_DEFAULT_FORCE;/*{
		_lastMoveDir.x,
        _lastMoveDir.y,
        //0.8f
        0.1f
    };*/
   // gyVec3Scale(&throwForce, &throwForce, 1.0f/_deltaTime);
    gyVec3ScaleAccum(&throwForce, 1.0f/_heldEntity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass);

    switch(_faceDir) {
    case Direction::DIR4_UP:
        startPosOffset = GYVector3 { 0.0f, -0.5f*_entity->getCollidable()->getWorldSize().y-0.5f*_heldEntity->getCollidable()->getWorldSize().y, 0.0f };
        break;
    case Direction::DIR4_DOWN:
        startPosOffset = GYVector3 { 0.0f, 0.5f*_entity->getCollidable()->getWorldSize().y+0.5f*_heldEntity->getCollidable()->getWorldSize().y, 0.0f };
        break;
    case Direction::DIR4_LEFT:
        startPosOffset = GYVector3 { -0.5f*_entity->getCollidable()->getWorldSize().x-0.5f*_heldEntity->getCollidable()->getWorldSize().x, 0.0f, 0.0f };
        break;
    case Direction::DIR4_RIGHT:
        startPosOffset = GYVector3 { 0.5f*_entity->getCollidable()->getWorldSize().x+0.5f*_heldEntity->getCollidable()->getWorldSize().x, 0.0f, 0.0f };
        break;
    default:
        break;

    }

    //_heldEntity->setDepthBaseY(_entity->getDepthBaseY());


    float yOffset = _entity->getDepthBaseY() - _heldEntity->getDepthBaseY();
    _heldEntity->getTransform()._pos.z = _entity->getTransform()._pos.z + yOffset;
    //_heldEntity->getTransform()._pos.y -

    gyVec3AddAccum(&_heldEntity->getTransform()._pos, &startPosOffset);


            //_entity->getTransform()._pos.y; /*+ 0.5f* _entity->getTransform()._size.y * _entity->getTransform()._scale.y*/
          //  + 0.5f* _heldEntity->getTransform()._size.y * _heldEntity->getTransform()._scale.y;
     //   _heldEntity->getTransform()._pos.z = _entity->getTransform()._pos.z;
    //  _heldEntity->getTransform()._pos.y -= (_heldEntity->getTransform()._pos.z - _entity->getTransform()._pos.z);

	//Set give the object its parent's initial velocity
	_heldEntity->getRigidBody()->clearKinetic();
    velocity = _entity->getRigidBody()->getVelocity();
    gyVec3ScaleAccum(&velocity, 2.0f);

    _heldEntity->getRigidBody()->setVelocity(velocity);
	_entity->getRigidBody()->clearKinetic();

	//Also apply a force to "throw" the bitch in the direction we're facing
    if(_movedThisFrame) _heldEntity->getRigidBody()->applyForce(throwForce);

	setHeldEntity(nullptr);
}

void Character::jump(void) {
	GYVector3 force = ELYSIAN_CHAR_JUMP_DEFAULT_FORCE;
    RigidBody*const rb = _entity->getRigidBody();

#if 0
	if(_heldEntity) {
		gyVec3ScaleAccum(&force, getHeldEntityForceDampFactor());
	}
#endif

	gyVec3ScaleAccum(&force, 1.0f/_deltaTime);


    getEntity()->getCollidable()->setRestingOnBody(false);

   // force.y /= rb->getStaticAttrib()->damping;
   // force.y *= rb->getStaticAttrib()->zDamping;

    rb->applyForce(force);

    _entity->getAnimation()->replaceState("Fall", "Jump");
}


#endif


}
