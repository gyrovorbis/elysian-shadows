#include <crtp/elysian_character.hpp>
#include <crtp/elysian_animation.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_rigid_body.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_skeleton.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CharacterBase)

float CharacterBase::_deltaTime = 0.0f;

CharacterBase::CharacterBase(const CharacterBase &other) {
    *this = other;
}

bool CharacterBase::isFacing(const Entity &entity) const {
    GYVector2 distVec;
    gyVec2Sub(&distVec, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
    return (_faceDir == Direction::fromVector(distVec));
}

void CharacterBase::setDashing(const bool val) {
    _dashing = val;
    setSpeedMultiplier(val? _dashMultiplier : 1.0f);
}

void CharacterBase::faceDir(const Direction::DIR4 dir) {
		switch(dir) {
			case Direction::DIR4_UP:
                playAnimation("Up");
				break;
			case Direction::DIR4_DOWN:
                playAnimation("Down");
				break;
			case Direction::DIR4_LEFT:
                playAnimation("Left");
                break;
			case Direction::DIR4_RIGHT:
                playAnimation("Right");
				break;
			default:
				break;
		}
	_faceDir = dir;
}

/* If we are currently holding another entity, their mass must be taken into account when applying
 * our movement force, so that heavy objects slow us down.
 * accel = force*inverseMass
 * accel = force*(inverseMassParent + inverseMassChild)
 */
float CharacterBase::getHeldEntityForceDampFactor(void) {
    if(!_heldEntity) return 0.0f;
	const float invParent = _entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
	const float invChild = _heldEntity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
    //Don't divid by zero, dipshit.
    if(invParent+invChild == 0.0f) return 0.0f;

    return (invChild/(invChild+invParent));
}

void CharacterBase::walk(const GYVector2 &dir) {
    if(gyVec2IsZero(&dir)) return;
    GYVector2 dist = dir;
    float mag = _walkForce * _deltaTime;
    mag *= (_dashing)? _dashMultiplier : 1.0f;
    gyVec2Normalize(&dist);
    gyVec2ScaleAccum(&dist, mag);
    moveByForce(dist);
}

void CharacterBase::moveByDist(const GYVector2 &dist) {
    _entity->_transform.translate(dist);
    animateWalk(dist);

    _lastMoveDir = GYVector3 { dist.x, dist.y, 0.0f };
    gyVec2Normalize((GYVector2*)&_lastMoveDir);
}

void CharacterBase::animateWalk(const GYVector2 &moveVec) {
    _movedThisFrame = true;
    replaceState("Stand", "Walk");

    Direction::DIR4 dir = Direction::fromVector(moveVec);

    if(_animate) {

        if((_walkAnimInterp && _faceDir == dir) || !_walkAnimInterp) {
            switch(dir) {
                case Direction::DIR4_UP:
                    playAnimation("Up");
                    break;
                case Direction::DIR4_DOWN:
                    playAnimation("Down");
                    break;
                case Direction::DIR4_LEFT:
                    playAnimation("Left");
                    break;
                case Direction::DIR4_RIGHT:
                    playAnimation("Right");
                    break;
                default:
                    break;
            }
        }
    }

    _faceDir = dir;
    //gyVec2Scale(&_lastMoveDir, &totalForce, 1.0f/gyVec2Magnitude(&totalForce));
}

void CharacterBase::faceEntity(Entity &entity) {
    GYVector2 dist;
    gyVec2Sub(&dist, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
    faceDir(Direction::fromVector(dist));
}

void CharacterBase::moveByForce(const GYVector2 &dist) {
	if(gyVec2IsZero(&dist)) return;
	GYVector2 totalForce = dist;

    if(_heldEntity) {
        gyVec2Scale(&totalForce, &dist, getHeldEntityForceDampFactor());
    }

    _entity->getRigidBody()->applyForce(GYVector3{ totalForce.x, totalForce.y, 0.0f });
    //_entity->transform.translate(dist);
    animateWalk(dist);
    _lastMoveDir = GYVector3 { dist.x, dist.y, 0.0f };
    gyVec2Normalize((GYVector2*)&_lastMoveDir);
}

void CharacterBase::update(void) {
	if(!_movedThisFrame) {
        replaceState("Walk", "Stand");
		faceDir(_faceDir);
		//_entity->getRigidBody()->setVelocity(GYVector3{ 0.0f, 0.0f, 0.0f });
	}
	else _movedThisFrame = false;

    //Set falling animation state
    if(_entity->getRigidBody()->getVelocity().z < 0.0f) replaceState("Jump", "Fall");
}

void CharacterBase::setHeldEntity(Entity * const entity) {
	if(_heldEntity) {
		_heldEntity->getRigidBody()->setActive(true);
        _heldEntity->getCollidable()->setActive(true);
        removeChildEntity(_heldEntity);
	}

	_heldEntity = entity;
	if(entity) {
        if(!entity->getRigidBody()->getAttrib()->getCanHold()) {
            _heldEntity = nullptr;
        } else {
            entity->getRigidBody()->setActive(false);
            entity->getRigidBody()->clearKinetic();
            entity->getCollidable()->setActive(false);
            addChildEntity(entity, ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD);
            removeState("Walk");
            removeState("Stand");
            removeState("Jump");
            removeState("Fall");
            addState("Hold");
        }
    } else {
        removeState("Hold");
    }

}

void CharacterBase::throwHeldEntity(void) {
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

void CharacterBase::jump(void) {
	GYVector3 force = ELYSIAN_CHAR_JUMP_DEFAULT_FORCE;
    RigidBody*const rb = _entity->getRigidBody();

    if(_heldEntity) {
        gyVec3ScaleAccum(&force, getHeldEntityForceDampFactor());
    }

    gyVec3ScaleAccum(&force, 1.0f/_deltaTime);


    getEntity()->getCollidable()->setRestingOnBody(false);

   // force.y /= rb->getStaticAttrib()->damping;
   // force.y *= rb->getStaticAttrib()->zDamping;

    rb->applyForce(force);
    replaceState("Fall", "Jump");
}


void CharacterBase::playAnimation(const char*const name) {
    if(_entity->getSkeleton()) {
        _entity->getSkeleton()->setAnimByName(0, name, true);
    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->playAnimation(name);
    }

}

void CharacterBase::replaceState(const char*const name1, const char*const name2) {
    if(_entity->getSkeleton()) {

    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->replaceState(name1, name2);
    }

}

void CharacterBase::removeChildEntity(Entity*entity) {
    if(_entity->getSkeleton()) {

    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->removeChildEntity(entity);
    }
}

void CharacterBase::addChildEntity(Entity*entity, const char*const name) {
    if(_entity->getSkeleton()) {

    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->addChildEntity(entity, name);
    }


}


void CharacterBase::removeState(const char*const state) {
    if(_entity->getSkeleton()) {

    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->removeState(state);
    }

}

void CharacterBase::addState(const char*const state) {
    if(_entity->getSkeleton()) {

    } else if(_entity->getAnimation()) {
        _entity->getAnimation()->addState(state);
    }


}

void CharacterBase::setSpeedMultiplier(const float factor) {
    if(_entity->getAnimation()) {
        _entity->getAnimation()->setSpeedMultiplier(factor);
    }

}



}
