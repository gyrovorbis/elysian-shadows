#include "elysian_party_system.hpp"
#include "elysian_debug.hpp"
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_character.hpp>
#include "crtp/elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include <crtp/elysian_collidable.hpp>
#include "elysian_collision_system.hpp"
#include <crtp/elysian_examinable.hpp>
#include "elysian_behavior_system.hpp"
#include <crtp/elysian_particle_system.hpp>
#include <crtp/elysian_level.hpp>
#include "elysian_audio_system.hpp"
#include <crtp/elysian_input_system.hpp>
#include <crtp/elysian_skeleton.hpp>
#include <behavior/elysian_playable_behavior.hpp>
#include <system/elysian_gui_system_base.hpp>

namespace elysian {

//----NONSTATIC----

PartySystem::PartySystem(void) {
    for(unsigned i = 0; i < ELYSIAN_PARTY_SLOTS_MAX; ++i) _players[i].setPlayerNum(i);

    getEventSys()->addEventObserver(this, EVENT_AREA_ENTER);
    getEventSys()->addEventObserver(this, EVENT_AREA_EXIT);
}

PartySystem::~PartySystem(void) {
#if 1
    for(auto* entity : _playables) {
        delete entity;
    }
#endif
}

void PartySystem::update(void) {//VOIDICATE
//  _tapTimer += getTimerSys()->getDeltaTime();
  //  _dashPartTimer += getTimerSys()->getDeltaTime();
 // if(_playableParty.size() < 1) return;
    _updateCamera();
    updatePartyPaths();
}

void PartySystem::onObserveEvent(Event *event, EventReceiver *receiver) {
    switch(event->getEventType()) {
    case EVENT_AREA_ENTER:
        getPartySys()->addPartyToArea();
        break;
    case EVENT_AREA_EXIT:
        getPartySys()->removePartyFromArea();
        break;

    }

}

int PartySystem::getPartySlot(const Entity *entity) const {
    const size_t size = _characterParty.size();
    for(size_t i = 0; i < size; ++i) {
        if(_characterParty[i] == entity) return i;
    }

    return -1;
}

void PartySystem::addPlayable(Entity* entity) {
    dbgprintf("PartySystem - Adding playable character [%s].", entity->getName());
    _playables.push_back(entity);
}

void PartySystem::addPlayableToParty(const char* name) {
    Entity* playable = getPlayable(name);

    if(!playable) {
        errprintf("Could not add [%s] to the party! Entity not found!", name);
        return;
    }

    addCharacterToParty(playable);
}

Entity* PartySystem::getPlayable(const char* name) const {
    for(auto&& entity : _playables)
        if(strcmp(name, entity->getName()) == 0)
            return entity;
    return nullptr;
}


void PartySystem::setPartyLeader(Entity *entity) {
    //make sure fucker is in teh party
    addCharacterToParty(entity);

    int slot = getPartySlot(entity);
    //he's already the leader
    if(slot == 0) return;

    Entity* prevLeader = getPartyLeader();
    _characterParty[0] = entity;
    _characterParty[slot] = prevLeader;
}


void PartySystem::checkPickup(Entity * const entity) const {

    auto* cont = getInputSys()->getController(0);

    if(entity->getCharacter()->getHeldEntity()) {
        if(!cont->isPressed(GY_CONTROLLER_BUT_RSHOULDER)) {
            entity->getCharacter()->throwHeldEntity();
        }

    } else if(entity->getCharacter()->getPullEntity()) {

        if(!cont->isPressed(GY_CONTROLLER_BUT_RSHOULDER)) {
            entity->getCharacter()->setPullEntity(nullptr);
        }


    } else {

        if(cont->isPressed(GY_CONTROLLER_BUT_RSHOULDER)) {
            if(entity->getCollidable()->getContactCount() > 0) {
                auto& rigidBodies = _curArea->rigidBodies;
                for(auto it = rigidBodies.begin();
                    it != rigidBodies.end();
                    ++it)
                {
                    if(getCollisionSys()->checkCollision(*entity, *(*it)->getEntity())) {
                       // if((*it)->getEntity()->getRigidBody()->getAttrib()->getCanHold()) {

                            if(entity->getCharacter()->canPickUpEntity((*it)->getEntity())) {
                                entity->getCharacter()->setHeldEntity((*it)->getEntity());
                            } else {
                                entity->getCharacter()->setPullEntity((*it)->getEntity());

                            }


                            break;

                        }

                   // }
                }
            }

        }
    }


}

Entity *PartySystem::getPlayablePartyMember(const char *name) const {
  for(unsigned int i = 0; i < _playableParty.size(); ++i) {
    if(strcmp(_playableParty[i]->getName(), name) == 0) {
      return _playableParty[i];
    }
  }
  Debug::printf(Debug::LOG_WARNING, "PartySystem::getPlayableFromParty(const char[]): %s is not in the party!", name);
    return nullptr;
}

Entity *PartySystem::getPartyMember(const unsigned int slot) const {
    if(slot >= _characterParty.size()) {
    Debug::printf(Debug::LOG_WARNING, "PartySystem::getPlayableFromParty(const uint): Invalid slot %d!", slot);
        return nullptr;
  }
    return _characterParty[slot];
}


Entity *PartySystem::getPartyLeader(void) const {
    for(auto&& it : _playableParty) {
        if(it.active) return it;
    }

    for(auto&& it : _characterParty) {
        if(it.active) return it;
    }

    return nullptr;
}

void PartySystem::addCharacterToParty(Entity* entity) {
    if(getPartySlot(entity) != -1) {
        warnprintf("Failed to add entity [%s] to party: Already in the party!", entity->getName());
    }

    if(_partyActive && !entity->isActive()) _curArea->addEntity(entity);

    if(!entity->getBehavior(ELYSIAN_PLAYABLE_BEHAVIOR_NAME)) {
        auto* behav = new game::PlayableBehavior(*entity);
        entity->addBehavior(behav);
        behav->setPlayerNum(0);
    }

    Debug::printf(Debug::LOG_VERBOSE, "%s has joined the party!", entity->getName());
    _characterParty.push_back(entity);
    if(_characterParty.size() > 1) entity->_transform.setPos(_partyPathPoint[_partyPathPoint.size()-1]);
    _updateCamera();

    if(_characterParty.size() > 1) {
        getCameraSys()->addEntity(entity);
        getGuiSys()->getAdvLog().printf("~c02%s ~c00has joined the party!", entity->getName());
    }

    _resizePartyPathPoints();
    return;
}

bool PartySystem::removeCharacterFromParty(const char *name) {
  for(unsigned int i = 0; i < _characterParty.size(); ++i) {
    if(strcmp(_characterParty[i]->getName(), name) == 0) { //determine slot from name
      removeCharacterFromParty(i); //invoke slot
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "! - PartySystem::RemoveCharacterFromParty(const char[]): %s is not in the party!", name);
  return false;
}

bool PartySystem::removeCharacterFromParty(const Entity *entity) {
  for(unsigned int i = 0; i < _characterParty.size(); ++i) {
    if(_characterParty[i] == entity) { //determine slot from memory address
      removeCharacterFromParty(i); //invoke slot
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "! - PartySystem::RemoveCharacterFromParty(Entity&): %s is not in the party!", entity->getName());
  return false;
}

bool PartySystem::removeCharacterFromParty(const unsigned int slot) {
  if(slot >= _characterParty.size()) {
    Debug::printf(Debug::LOG_WARNING, "! - PartySystem::RemoveCharacterFromParty(const uint): %d is an invalid slot!", slot);
    return false;
  }
  Debug::printf(Debug::LOG_VERBOSE, "%s has left the party!", _characterParty[slot]->getName());
  //update positions
    for(unsigned int i = _characterParty.size()-1; i > slot; --i) _characterParty[i]->_transform._pos = _characterParty[i-1]->_transform._pos;
  _characterParty.erase(_characterParty.begin() + slot);
  _resizePartyPathPoints();
    _updateCamera();
  //for(unsigned int i = slot; i < _characterParty.size(); ++i) _characterParty[i]->transform.setPosition(_partyPathPoint[_partyPathPoint.size()-2]);
  return true;
}

Entity *PartySystem::getCharacterPartyMember(const char *name) const {
  for(unsigned int i = 0; i < _characterParty.size(); ++i) {
    if(strcmp(_characterParty[i]->getName(), name) == 0) {
      return _characterParty[i];
    }
  }
    //Debug::printf(Debug::LOG_WARNING, "PartySystem::getCharacterPartyMember(const char[]): %s is not in the party!", name);
    return nullptr;
}

Entity *PartySystem::getCharacterPartyMember(const unsigned int slot) const {
  if(slot >= _characterParty.size()) {
        //Debug::printf(Debug::LOG_WARNING, "PartySystem::getCharacterPartyMember(const uint): %d is an invalid slot!", slot);
        return nullptr;
  }
  return _characterParty[slot];
}

void PartySystem::_resetPartyPathPoints(void) {
    auto* leader = getPartyLeader();
    if(leader) {
        for(int i = 0; i < _partyPathPoint.size(); ++i) {
            _partyPathPoint[i] = GYVector2 { leader->getTransform().getPosX(), leader->getTransform().getPosY() };
        }
    }
}

void PartySystem::_resizePartyPathPoints(void) {
    const unsigned activePartyMemberCount = getActivePartyMemberCount();
  int pathPointAmount = activePartyMemberCount*_pointsPerCharacter+1;
  //check if even necessary
  if(pathPointAmount  == (int)_partyPathPoint.size()) return;
  if(pathPointAmount < 0) return;

  //allocate more
  if(pathPointAmount > (int)_partyPathPoint.size()) {
    GYVector2 lastPoint;
    if(_partyPathPoint.size() == 0) {
            lastPoint = (_playableParty.size() != 0)? GYVector2 {_playableParty[0]->_transform._pos.x, _playableParty[0]->_transform._pos.y } : GYVector2 {_characterParty[0]->_transform._pos.x, _characterParty[0]->_transform._pos.y };
    } else {
      lastPoint = _partyPathPoint[_partyPathPoint.size()-1];
    }
    while((int)_partyPathPoint.size() != pathPointAmount) {
      _partyPathPoint.push_back(lastPoint);
    }
  }
  //delete points
  else {
    while((int)_partyPathPoint.size() > pathPointAmount) {
      _partyPathPoint.erase(_partyPathPoint.end()-1);
    }
  }

  Debug::printf(Debug::LOG_VERBOSE, "_partyPathPoint.size() - %d", _partyPathPoint.size());
}

void PartySystem::updatePartyPaths(void) {
  unsigned int i;
  if(!_followLogic) return;
  if(_playableParty.size() == 0 && _characterParty.size() == 0) return;
  if(_playableParty.size() == 1 && _characterParty.size() == 0) return;

  //Keep distance between party
#if 1
  auto* leader = getPartyLeader();
  if(!leader) return;
  if(_playableParty.size()) {
        if(fabs(_partyPathPoint[0].x - leader->_transform._pos.x) < 1.0f && fabs(_partyPathPoint[0].y - leader->_transform._pos.y) < 1.0f) return;
  }
  else if(_characterParty.size()) {
        if(fabs(_partyPathPoint[0].x - leader->_transform._pos.x) < 1.0f && fabs(_partyPathPoint[0].y - leader->_transform._pos.y) < 1.0f) return;
  }
#endif

  //update chain of points
  for(i = _partyPathPoint.size()-1; i > 0; --i) {
    _partyPathPoint[i] = _partyPathPoint[i-1];
  }

  //add leader's current position
    _partyPathPoint[0] = (_playableParty.size() > 0)?
                GYVector2{leader->_transform._pos.x, leader->_transform._pos.y } :
                GYVector2 {leader->_transform._pos.x, leader->_transform._pos.y };

  unsigned int currentIndex = 2;
  //update playables
  unsigned int activeSize = _playableParty.size();//(_playableParty.size() < _activePlayablePartySize)? _playableParty.size() : _activePlayablePartySize;
  for(i = 1; i < activeSize; ++i) {
    Entity* partyMember = _playableParty[i];
    if(!isPartyMemberActive(i)) continue;
    GYVector2 dist;
    currentIndex += _pointsPerCharacter-1;
        //Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
    //Debug::Print(Debug::NORMAL, "x - %d, y - %d\n", currentIndex, currentIndex);
    gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
        partyMember->getCharacter()->moveByDist(dist);
      //  _playableParty[i]->_transform.setPos(_partyPathPoint[currentIndex-1]);
  }
  //update characters
  //--currentIndex;

  i = 0;//(_playableParty.size())? 0 : 1;
  for(; i < _characterParty.size(); ++i) {
      Entity* partyMember = _characterParty[i];
      if(!isPartyMemberActive(i) || partyMember == leader) continue;
    GYVector2 dist;
    currentIndex += _pointsPerCharacter-1;
    //Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
    gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
       // _characterParty[i]->getCharacter()->animateWalk(dist);
    partyMember->getCharacter()->moveByDist(dist);
       partyMember->_transform.setPos(_partyPathPoint[currentIndex-1]);
  }
}

void PartySystem::removePartyFromArea(void) {
    if(_partyActive) {
        for(unsigned int i = 0; i < _playableParty.size(); ++i)
            _curArea->removeEntity(_playableParty[i]);
        for(unsigned int i = 0; i < _characterParty.size(); ++i)
            _curArea->removeEntity(_characterParty[i]);
        _partyActive = false;
    }
}

void PartySystem::addPartyToArea(void) {
    if(!_partyActive) {
        auto* leader = getPartyLeader();

        auto addToArea = [&](Entity* partyMember, unsigned slot) {
            if(leader && partyMember != leader) {
                partyMember->getTransform().setPos(leader->getTransform().getPos());
            }
            if(isPartyMemberActive(slot)) _curArea->addEntity(partyMember);
            else _curArea->addInactiveEntity(partyMember);
        };
#if 0
        for(unsigned int i = 0; i < _playableParty.size(); ++i)
            addToArea(_playableParty[i], i);
#endif
        for(unsigned int i = 0; i < _characterParty.size(); ++i)
            addToArea(_characterParty[i], i);

        _resetPartyPathPoints();
        _updateCamera();
        _partyActive = true;
    }
}


void PartySystem::checkExamine(Entity* const entity) const {
  if(!entity->getCollidable()->getContactCount()) return;


  //TODO: THIS IS REESTED
  auto& entities = _curArea->entities;
  for(auto it = entities.begin();
    it != entities.end();
    ++it)
  {
    Examinable* examinable = (*it)->getExaminable();
    if(examinable && examinable->isEnabled()) {
      if(getCollisionSys()->checkCollision(*entity, **it)) {
        getBehavSys()->onExamine(*it, entity);

        //if(examinable->getDialog()) //getDialogSys()->startDialog(examinable->getDialog());

      }
    }
  }
}

void PartySystem::_updateCamera(void) const {
    getCameraSys()->setFollowEntity(getPartyLeader());
}

// =========== PLAYER ==========
void Player::_processMovement(const float /*deltaTime*/, const Controller*const cont, Entity*const entity, PartySystem*const partySys) {
    auto angle = cont->getAnalogAngle(GY_CONTROLLER_ANALOG_LEFT);
    auto mag = cont->getAnalogMag(GY_CONTROLLER_ANALOG_LEFT);

    _tapTimer += partySys->getTimerSys()->getDeltaTime();
    _dashPartTimer += partySys->getTimerSys()->getDeltaTime();

//    auto endDash = [&](void) {
//        entity->getCharacter()->setSpeedMultiplier(1.0f);
//        partySys->getAudioSys()->setSfxChannelPitch(0, 1.0f);
//        entity->getCharacter()->setDashing(false);
//        _tapTimer = 0.0f;
//        _lastAngle = 0.0f;
//    };

    auto emitDashParticles = [&](void) {
        partySys->getPartSys()->getAttrib(0)->setAcceleration(GYVector3{ 0.0f, -70.0f, 7.0f/*7.0f*//*6.5f*/ });
        partySys->getPartSys()->getAttrib(0)->setForceMask(0);
        partySys->getPartSys()->getAttrib(0)->setEndColor(Color4(255, 255, 255, 0));
        partySys->getPartSys()->getAttrib(0)->setColor(255, 255, 255, 100);
        partySys->getPartSys()->getAttrib(0)->setMaxLife(30);
        partySys->getPartSys()->getAttrib(0)->setStick(false);
        GYVector3 pos = entity->_transform._pos;
        pos.z += 5.0f;
        pos.y = entity->getDepthBaseY();
        //partySys->getPartSys()->emitFromPoint(0, pos, angle, 500);
        _dashPartTimer = 0.0f;
    };

    _tapTimer += partySys->getTimerSys()->getDeltaTime();
    switch(_dashState) {
        case DASH_NONE:
        entity->getCharacter()->setDashing(false);
        if(mag < 0.3f) {
            ++_dashState;
                 //   dbgprintf("DASH_FIRST_TAP");
        }
        break;
    case DASH_FIRST_TAP:
        if(mag >= 0.5f) {
            _tapTimer = 0.0f;
            _lastAngle = angle;
            ++_dashState;
                 //   dbgprintf("DASH_RELEASE");
        }
        break;
    case DASH_RELEASE:
        //if(fabs(_lastAngle-angle) > 0.5f*M_PI) {
            //_dashState = DASH_NONE;
        //} else {
            if(_tapTimer >= _dashTapTimeWindow) {
                _dashState = DASH_NONE;
                    //    dbgprintf("DASH_NONE");
            } else if(mag < 0.5f) {
          //      _lastAngle = angle;
                ++_dashState;
                 //       dbgprintf("DASH_SECOND_TAP");
            }
        //}
        break;
    case DASH_SECOND_TAP:
        //if(fabs(_lastAngle-angle) > 0.5f*M_PI) {
          //  _dashState = DASH_NONE;
        //} else {
            if(_tapTimer >= _dashTapTimeWindow) {
                _dashState = DASH_NONE;
                    //    dbgprintf("DASH_NONE: TIME");
            } else if(mag >= 0.5f) {
                //if(fabs(_lastAngle-angle) > M_PI) {
                //    _dashState = DASH_NONE;
                //            dbgprintf("DASH_NONE: ANGLE[%f]", fabs(_lastAngle-angle));
                //} else {
                      //      dbgprintf("DASH_ACTIVE");
                    _lastAngle = angle;
                    ++_dashState;
                    entity->getCharacter()->setDashing(true);
                    emitDashParticles();
                    _tapTimer = 0.0f;
                //}
            }
        //}
        break;
    case DASH_ACTIVE:
        default:
        if(!entity->getCharacter()->isDashing() || _tapTimer >= _dashDuration) {
            _dashState = DASH_NONE;
                  //  dbgprintf("DASH_NONE");
        }
        break;
    }

        //Compensate for inaccuracy and over-sensitivity with dead-zones on either side
        if(mag < 0.3f)      return;
        mag -= 0.3f;
        mag /= 0.7f;

        GYVector2 moveVec = cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT);

        if(!cont->isPressed(GY_CONTROLLER_BUT_LSHOULDER)/* && !entity->getCharacter()->getPullEntity()*/) {
            auto dir = Direction::fromVector2(moveVec);
            entity->getCharacter()->setFaceDir(dir);
        } else {

            if(mag >= 0.7f && !entity->getCharacter()->isDashing()) mag = 0.699999f;

        }

        gyVec2ScaleAccum(&moveVec, mag);

        entity->getCharacter()->walk(moveVec);




}

void Player::_processJump(const float /*deltaTime*/, const Controller*const cont, Entity*const entity) {
    if(cont->isTapped(GY_CONTROLLER_BUT_X)) {
        entity->getCharacter()->jump();
    }
}



void Player::_processExamine(const float /*deltaTime*/, const Controller*const cont, Entity*const entity, PartySystem* partySys) {
    if(cont->isTapped(GY_CONTROLLER_BUT_CIRCLE)) {
        partySys->checkExamine(entity);
    }
}


void Player::_processIdle(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) { }
void Player::_processPickup(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const entity, PartySystem* partySys) {



   // if(cont->isTapped(GY_CONTROLLER_BUT_RSHOULDER)) {
        partySys->checkPickup(entity);
   // }

}

void Player::_processCarry(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) {
}

void Player::_processPushPull(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) {
}


void Player::processInput(const Controller* const cont, PartySystem*const partySys) {


    auto* partyMember = partySys->getPartyMember(_playerNum);
    const float deltaTime = partySys->getTimerSys()->getDeltaTime();


    _processMovement(deltaTime, cont, partyMember, partySys);
    _processJump(deltaTime, cont, partyMember);
    _processExamine(deltaTime, cont, partyMember, partySys);
    _processPickup(deltaTime, cont, partyMember, partySys);
}

void PartySystem::swapPartyMemberSlots(unsigned int slot1, unsigned int slot2) {
    auto* slot1Entity = getPartyMember(slot1);
    auto* slot2Entity = getPartyMember(slot2);
    bool slot1WasActive = isPartyMemberActive(slot1);
    bool slot2WasActive = isPartyMemberActive(slot2);

    if(!slot1Entity || !slot2Entity) {
        errprintf("[PartySystem] Failed to swap party members at slot %d (%s) and %d (%s)!",
                  slot1, slot1Entity? slot1Entity->getName() : "Invalid",
                  slot2, slot2Entity? slot2Entity->getName() : "Invalid");
        return;
    }

    _characterParty[slot1] = slot2Entity;
    _characterParty[slot2] = slot1Entity;
    setPartyMemberActive(slot1, slot2WasActive);
    setPartyMemberActive(slot2, slot1WasActive);

    auto pos = slot1Entity->getTransform().getPos();
    slot1Entity->getTransform().setPos(slot2Entity->getTransform().getPos());
    slot2Entity->getTransform().setPos(pos);

    _updateCamera();

}

unsigned PartySystem::getActivePartyMemberCount(void) const {
    unsigned activeCount = 0;
    for(unsigned i = 0; i < getPartySize(); ++i) {
        if(isPartyMemberActive(i)) ++activeCount;
    }
    return activeCount;

}


void PartySystem::setPartyMemberActive(unsigned slot, bool active) {
    _characterParty[slot].active = active;
    if(!active) _characterParty[slot]->deactivate();
    else _characterParty[slot]->activate();

    _resetPartyPathPoints();
    Entity* leader = getPartyLeader();
    if(leader && slot != getPartySize()-1) {
        for(unsigned i = 0; i < getPartySize(); ++i) {
            Entity* follower = getPartyMember(i);
            if(follower == leader) continue;
            follower->getTransform().setPos(leader->getTransform().getPos());
        }
    }
}

void PartySystem::setSlotFollowLogicEnabled(const unsigned int slot, bool enabled) {
    if(slot < getPartySize()) {
        _characterParty[slot].followLogic = enabled;

    } else errprintf("Tried to set invalid slot [%d] follow logic enabled to [%d]", slot, enabled);
}


}



