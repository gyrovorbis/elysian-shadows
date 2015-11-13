#include <crtp/elysian_character.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_rigid_body.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_skeleton.hpp>
#include <crtp/elysian_lua_variant.hpp>
#include <lua/elysian_lua_singleton.hpp>
#include <system/elysian_collision_system.hpp>
#if 1
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>
#endif

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(CharacterBase)

float CharacterBase::_deltaTime = 0.0f;

bool CharacterBase::isFacing(const Entity &entity) const {
    GYVector2 distVec;
    gyVec2Sub(&distVec, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
    return (_faceDir == Direction::fromVector2(distVec));
}

void CharacterBase::setDashing(const bool val) {
    if(val) {
        _entity->getRigidBody()->getAttrib()->getForceMask().setBit(1, false);
        _entity->getRigidBody()->clearKinetic();
        addState(STATE_DASH);
    }
    else {
        _entity->getRigidBody()->getAttrib()->getForceMask().setBit(1, true);
        removeState(STATE_DASH);
    }
    _dashing = val;
    setSpeedMultiplier(val? _dashMultiplier : 1.0f);
}

void CharacterBase::faceDir(const Direction::DIR4 dir) {
    playAnimation(dir);
#if 0
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
#endif
  _faceDir = dir;
}

/* If we are currently holding another entity, their mass must be taken into account when applying
 * our movement force, so that heavy objects slow us down.
 * accel = force*inverseMass
 * accel = force*(inverseMassParent + inverseMassChild)
 */
float CharacterBase::getEntityForceDampFactor(void) {
    Entity* entity;

    if(_heldEntity)         entity = _heldEntity;
    else if(_pullEntity) entity = _pullEntity;
    else return 0.0f;

  const float invParent = _entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
    const float invChild = entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
    //Don't divid by zero, dipshit.
    if(invParent+invChild == 0.0f) return 0.0f;

    return (invChild/(invChild+invParent));
}

float CharacterBase::getOtherForceDampFactor(void) {
    Entity* entity;

    if(_heldEntity)         entity = _heldEntity;
    else if(_pullEntity) entity = _pullEntity;
    else return 0.0f;

    const float invParent = _entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
    const float invChild = entity->getCollidable()->getAttrib()->getStaticAttrib()->inverseMass;
    //Don't divid by zero, dipshit.
    if(invParent+invChild == 0.0f) return 0.0f;

    return (invParent/(invChild+invParent));
}


void CharacterBase::walk(const GYVector2 &dir) {
    if(gyVec2IsZero(&dir) || hasState(STATE_KNOCKBACK)) return;
    GYVector2 dist = dir;
    float mag = _walkForce;
    mag *= (_dashing)? _dashMultiplier : 1.0f;
    float scale = gyVec2Magnitude(&dir);
    if(scale > 0.7f)
        replaceState(STATE_WALK, STATE_RUN);
    else
        replaceState(STATE_RUN, STATE_WALK);
    gyVec2ScaleAccum(&dist, mag);
    moveByForce(dist);
}

void CharacterBase::moveByDist(const GYVector2 &dist) {
    _entity->_transform.translate(dist);
    animateWalk(dist);

    _lastMoveDir = GYVector3 { dist.x, dist.y, 0.0f };
    gyVec2Normalize((GYVector2*)&_lastMoveDir);
}

void CharacterBase::animateWalk(const GYVector2 &/*moveVec*/) {
    _movedThisFrame = true;
    //replaceState(STATE_STAND, STATE_WALK);

    //Direction::DIR4 dir = Direction::fromVector(_faceDir);

  //  if(_animate) {
    playAnimation(_faceDir);
#if 0
            switch(_faceDir) {
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
       // }
#endif

    //_faceDir = dir;
    //gyVec2Scale(&_lastMoveDir, &totalForce, 1.0f/gyVec2Magnitude(&totalForce));
}

void CharacterBase::faceEntity(Entity &entity) {
    GYVector2 dist;
    gyVec2Sub(&dist, (GYVector2*)&entity._transform._pos, (GYVector2*)&_entity->_transform._pos);
    faceDir(Direction::fromVector2(dist));
}

void CharacterBase::moveByForce(const GYVector2 &dist) {
  if(gyVec2IsZero(&dist)) return;
  GYVector2 totalForce = dist;

    if(_heldEntity || _pullEntity) {
        GYVector2 objForce;
        gyVec2Scale(&totalForce, &dist, getEntityForceDampFactor());

        if(_pullEntity) {

            gyVec2Scale(&objForce, &dist, (getEntityForceDampFactor()*_entity->getCollidable()->getAttrib()->getStaticAttrib()->getInverseMass())/_pullEntity->getCollidable()->getAttrib()->getStaticAttrib()->getInverseMass());
            _pullEntity->getRigidBody()->applyForce(GYVector3{ objForce.x, objForce.y, 0.0f });
        }
    }


    _entity->getRigidBody()->applyForce(GYVector3{ totalForce.x, totalForce.y, 0.0f });
    //_entity->transform.translate(dist);
    animateWalk(dist);
    _lastMoveDir = GYVector3 { dist.x, dist.y, 0.0f };
    gyVec2Normalize((GYVector2*)&_lastMoveDir);
}

void CharacterBase::update(void) {

    const auto pushing = _checkPushing();

    if(!pushing) removeState(STATE_PUSH);

  if(!_movedThisFrame) {
        replaceState(STATE_WALK, STATE_STAND);
        replaceState(STATE_RUN, STATE_STAND);
        faceDir(_faceDir);
    //_entity->getRigidBody()->setVelocity(GYVector3{ 0.0f, 0.0f, 0.0f });
  }
  else _movedThisFrame = false;

    //Set falling animation state
    if(_entity->getRigidBody()->getVelocity().z < 0.0f) {
        replaceState(STATE_JUMP, STATE_FALL);
        removeState(STATE_PUSH);
    }

    if(_entity->getRigidBody()->getRestingOnBody()) {
        removeState(STATE_KNOCKBACK);
        removeState(STATE_FALL);
        if(pushing) addState(STATE_PUSH);

    }
}

bool CharacterBase::_checkPushing(void) {
    bool found = false;

    Singletons::getCollisionSys()->foreachContact(*getEntity(), [&](const CollisionContact*const contact, const int bodyIndex) {
        GYVector2 normal;
        contact->getNormal(bodyIndex? 0 : 1, normal);

        if(_faceDir == Direction::fromVector2(normal)) {
            found = true;
            return true;
        }

        return false;
    });

    return found;

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
            //addChildEntity(entity, ELYSIAN_CHAR_ANIM_CHILD_SEQ_HELD);
            getEntity()->getSkeleton()->attachEntityToSlot("hold_slot", entity);

            removeState(STATE_WALK);
            removeState(STATE_STAND);
            removeState(STATE_JUMP);
            removeState(STATE_FALL);
            removeState(STATE_COMBAT);
            addState(STATE_HOLD);
        }
    } else {
        removeState(STATE_HOLD);
        getEntity()->getSkeleton()->removeEntityFromSlot("hold_slot");
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
        startPosOffset = {0.f,0.f,0.f};
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

    _heldEntity->activate();
    setHeldEntity(nullptr);
}

void CharacterBase::jump(void) {
    if(!_entity->getRigidBody()->getRestingOnBody()) return;

  GYVector3 force = ELYSIAN_CHAR_JUMP_DEFAULT_FORCE;
    RigidBody*const rb = _entity->getRigidBody();

    if(_heldEntity) {
        gyVec3ScaleAccum(&force, getEntityForceDampFactor());
    }

    gyVec3ScaleAccum(&force, 1.0f/_deltaTime);


    getEntity()->getCollidable()->setRestingOnBody(false);

   // force.y /= rb->getStaticAttrib()->damping;
   // force.y *= rb->getStaticAttrib()->zDamping;

    rb->applyForce(force);
    replaceState(STATE_FALL, STATE_JUMP);
}


void CharacterBase::playAnimation(Direction::DIR4 dir) {
    if(_tempStateActive) return;
    const char* name = getAnimNameFromStates(dir);

    if(!name) return;

    if(_entity->getSkeleton()) {
        _entity->getSkeleton()->setAnim(name);
    }
}

void CharacterBase::replaceState(ANIM_STATE state1, ANIM_STATE state2) {
    if(_entity->getSkeleton()) {
        _animStates[state1] = false;
        _animStates[state2] = true;
    }

}

void CharacterBase::removeChildEntity(Entity*/*entity*/) {
    if(_entity->getSkeleton()) {

    }
}

void CharacterBase::addChildEntity(Entity*/*entity*/, const char*const /*name*/) {
    if(_entity->getSkeleton()) {

    }


}


void CharacterBase::removeState(const ANIM_STATE state) {
  //  if(_entity->getSkeleton()) {
        _animStates[state] = false;
   // }

}

void CharacterBase::addState(const ANIM_STATE state) {
    if(_entity->getSkeleton()) {
        _animStates[state] = true;
    }


}

void CharacterBase::setSpeedMultiplier(const float /*factor*/) {

}


bool CharacterBase::canPickUpEntity(Entity * const entity) {
    if(entity->getRigidBody()) {
        return entity->getRigidBody()->getAttrib()->getCanHold();
    }
    return true;

}



const char* CharacterBase::getAnimNameFromStates(const Direction::DIR4 dir) const {

    LuaManager::stackBalanceBegin();
    static char buffer[200] = { 0 };


    if(_getAnimNameFromStatesRef.isNil()) {
#if 0
        LuaManager::getGlobal("Character");
        lua_getfield(*_mainState, -1, "getAnimNameFromStates");
        assert(_getAnimNameFromStatesRef.pull());
        lua_pop(*_mainState, 1);
#endif
        LuaVariant v(static_cast<const Character*>(this));
        _getAnimNameFromStatesRef = v["getAnimNameFromStates"];
        GY_ASSERT(_getAnimNameFromStatesRef.getType() == LUA_TFUNCTION);
    }

    const char* string = nullptr;
    LuaVariant r = _getAnimNameFromStatesRef(static_cast<const Character*>(this), dir);

    if(r.getType() == LUA_TSTRING) {
        if(r != "") {
            assert(r.getValue<const char*>());
            strcpy(buffer, r.getValue<const char*>());
            string = buffer;
        }
    }

    LuaManager::stackBalanceEnd();

    return string;

}


void CharacterBase::applyKnockbackForce(const GYVector3 &force) {
    addState(STATE_KNOCKBACK);
    _entity->getRigidBody()->applyForce(force);

}

void CharacterBase::addTempState(const ANIM_STATE state, const int track, OOLUA::Lua_func_ref ref) {
    _tempStateActive    = true;
    _luaCallback        = ref;
    _callbackTrack      = track;

    addState(state);
    auto*name = getAnimNameFromStates(_faceDir);

    if(name) {
        getEntity()->getSkeleton()->setAnim(name, [=](int trackIndex, spEventType eventType, spEvent* /*event*/, int /*loopCount*/) {

            if(_callbackTrack == trackIndex || _callbackTrack == -1) {
                LuaVariant v;

                switch(eventType) {
                case SP_ANIMATION_END:
                case SP_ANIMATION_COMPLETE:

                    OOLUA::push(*_mainState, _luaCallback);
                    v.pull();
                    (void)v();
                    _tempStateActive = false;
                    removeState(state);
                    return true;
                    break;
                default: break;
                }

            }
            return false;

        });
    }


}


}
