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
#include "elysian_particle_system.hpp"
#include <crtp/elysian_level.hpp>
#include "elysian_audio_system.hpp"
#include <crtp/elysian_input_system.hpp>
#include <crtp/elysian_skeleton.hpp>
#include <elysian_playable_behavior.hpp>
#include <system/elysian_gui_system_base.hpp>

namespace elysian {

//----NONSTATIC----

PartySystem::PartySystem(void) {
    for(unsigned i = 0; i < ELYSIAN_PARTY_SLOTS_MAX; ++i) _players[i].setPlayerNum(i);
}

PartySystem::~PartySystem(void) {
    //for(unsigned int i = 0; i < _playableAmount; ++i) {
        //delete _playable[i];
    //}

//	delete[] _playable;
}

void PartySystem::update(void) {//VOIDICATE
#if 0
  _tapTimer += getTimerSys()->getDeltaTime();
    _dashPartTimer += getTimerSys()->getDeltaTime();
  //if(_playableParty.size() < 1) return;
  updatePartyPaths();
#endif
}

int PartySystem::getPartySlot(const Entity *entity) const {
    const size_t size = _characterParty.size();
    for(size_t i = 0; i < size; ++i) {
        if(_characterParty[i] == entity) return i;
    }

    return -1;
}

void PartySystem::render(void) {}

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

void PartySystem::processInput(void) {
#if 0
    for(unsigned i = 0; i < _characterParty.size(); ++i) {
        _players[i].processInput(getInputSys()->getController(i), this);
    }
#endif
#if 0
    return;
    auto* cont = getInputSys()->getController(0);

  Entity*entity = getPartyLeader();
  if(!entity) return;

    GYVector2 moveDist = cont->getAnalogStick(GY_CONTROLLER_ANALOG_LEFT);
    bool moved = false;


    if(cont->getAnalogMag(GY_CONTROLLER_ANALOG_LEFT) >= 0.3f) {
        moved = true;
    }



  //if(_playableParty.size() < 1) return;

/*	if(input.getTapped(GameInput::BUT_I)) {
    for(unsigned int i = 0; i < _partyPathPoint.size(); ++i) {
      Debug::Print(Debug::NORMAL, "%d - <%f, %f>\n", i, _partyPathPoint[i].x, _partyPathPoint[i].y);
    }
    Debug::Print(Debug::NORMAL, "Dist1 - %f,%f\nDist2 - %f,%f\n", _playableParty[0]->transform.getPosition().x-_playableParty[1]->transform.getPosition().x, _playableParty[0]->transform.getPosition().y-_playableParty[1]->transform.getPosition().y, _characterParty[0]->transform.getPosition().x-_characterParty[1]->transform.getPosition().x, _characterParty[0]->transform.getPosition().y-_characterParty[1]->transform.getPosition().y);
  }*/

  /*if(input.getTapped(GameInput::BUT_O)) {
    RemoveCharacterFromParty(0u);
  }

  if(input.getTapped(GameInput::BUT_P)) {
    RemovePlayableFromParty(0u);
    return;
  }*/
    auto endDash = [&](void) {
        getPartyLeader()->getCharacter()->setSpeedMultiplier(1.0f);
        getAudioSys()->setSfxChannelPitch(0, 1.0f);
        getPartyLeader()->getCharacter()->setDashing(false);
        _tapTimer = 0.0f;
        _lastDpadTapped = GY_CONTROLLER_BUT_COUNT;
    };

    auto emitDashParticles = [&](void) {
    getPartSys()->getAttrib(0)->setAcceleration(GYVector3{ 0.0f, -70.0f, 7.0f/*7.0f*//*6.5f*/ });
        getPartSys()->getAttrib(0)->setForceMask(0);
        getPartSys()->getAttrib(0)->setEndColor(GYColor4 { 255, 255, 255, 0});
        getPartSys()->getAttrib(0)->setColor(255, 255, 255, 100);
        getPartSys()->getAttrib(0)->setMaxLife(30);
        getPartSys()->getAttrib(0)->setStick(false);
        GYVector3 pos = getPartyLeader()->_transform._pos;
        pos.z += 5.0f;
        pos.y = getPartyLeader()->getDepthBaseY();
        getPartSys()->emitFromPoint(0, pos, _dashPartAngle, 500);
        _dashPartTimer = 0.0f;
    };

    auto updateDash = [&](const GY_CONTROLLER_BUTTON but) {
    bool updateTappedButton = true;

    if(_dashing) {
      bool stop = false;
      switch(_lastDpadTapped) {
                case GY_CONTROLLER_BUT_DPAD_UP:
                  if(but == GY_CONTROLLER_BUT_DPAD_DOWN) stop = true;
          break;
                case GY_CONTROLLER_BUT_DPAD_DOWN:
                    if(but == GY_CONTROLLER_BUT_DPAD_UP) stop = true;
           break;

                case GY_CONTROLLER_BUT_DPAD_LEFT:
                    if(but == GY_CONTROLLER_BUT_DPAD_RIGHT) stop = true;
          break;

                case GY_CONTROLLER_BUT_DPAD_RIGHT:
                    if(but == GY_CONTROLLER_BUT_DPAD_LEFT) stop = true;
            break;
                default: break;
      }

            if(stop || _tapTimer > _dashTapTimeWindow) {
                endDash();
                _lastDpadTapped = GY_CONTROLLER_BUT_COUNT;
                updateTappedButton = false;
            } //else {
                //updateTappedButton = false;
        //	}
    } else {
      if(but == _lastDpadTapped && _tapTimer <= _dashTapTimeWindow) {
                //float angle;

        switch(_lastDpadTapped) {
                    case GY_CONTROLLER_BUT_DPAD_UP:
                      _dashPartAngle = M_PI*0.5f;
            break;
                    case GY_CONTROLLER_BUT_DPAD_DOWN:
                        _dashPartAngle = -M_PI*0.5f;
             break;

                    case GY_CONTROLLER_BUT_DPAD_LEFT:
                        _dashPartAngle = 0.0f;
            break;

                    case GY_CONTROLLER_BUT_DPAD_RIGHT:
                        _dashPartAngle = -M_PI;
              break;

          default: break;

        }

                getPartyLeader()->getCharacter()->setDashing(true);
        getAudioSys()->setSfxChannelPitch(0, 2.0f);
//				getPartSys()->getAttrib(0)->setSize(3);
//				getPartSys()->getAttrib(0)->setFadeOut(true);
                emitDashParticles();
        //Debug::printf(Debug::LOG_VERBOSE, "DASHING!!!");
      }
    }

    if(updateTappedButton) _lastDpadTapped = but;
    _tapTimer = 0.0f;
  };

    if(_dashDuration != 0.0f && _tapTimer > _dashDuration) {
        endDash();
    } else if(_dashing && _dashPartTimer >= _dashPartInterval) {
        emitDashParticles();
    }

    if(_lastDpadTapped != GY_CONTROLLER_BUT_COUNT) {
        //if(!input.getDown(_lastDpadTapped)) endDash();
    }

    if(cont->isPressed(GY_CONTROLLER_BUT_DPAD_UP)){
        if(cont->isTapped(GY_CONTROLLER_BUT_DPAD_UP)) updateDash(GY_CONTROLLER_BUT_DPAD_UP);
//		gyVec2AddAccum(&moveDist, &upMove);
    moved = true;
  }
    if(cont->isPressed(GY_CONTROLLER_BUT_DPAD_DOWN)){
        if(cont->isTapped(GY_CONTROLLER_BUT_DPAD_DOWN))  updateDash(GY_CONTROLLER_BUT_DPAD_DOWN);
        //gyVec2AddAccum(&moveDist, &downMove);
    moved = true;
  }

    if(cont->isPressed(GY_CONTROLLER_BUT_DPAD_LEFT)){
        if(cont->isTapped(GY_CONTROLLER_BUT_DPAD_LEFT))  updateDash(GY_CONTROLLER_BUT_DPAD_LEFT);
        //gyVec2AddAccum(&moveDist, &leftMove);
    moved = true;
  }

    if(cont->isPressed(GY_CONTROLLER_BUT_DPAD_RIGHT)){
        if(cont->isTapped(GY_CONTROLLER_BUT_DPAD_RIGHT)) updateDash(GY_CONTROLLER_BUT_DPAD_RIGHT);
        //gyVec2AddAccum(&moveDist, &rightMove);
    moved = true;
  }
#if 0
  //examine
  if(input.getTapped(GameInput::BUT_M)) {
    if(_playableParty.size() != 0) _checkExamine(_playableParty[0]);
    if(_characterParty.size() != 0) _checkExamine(_characterParty[0]);
  }



#endif

    if(cont->isTapped(GY_CONTROLLER_BUT_X)) {
        if(_characterParty.size() != 0) _characterParty[0]->getCharacter()->jump();

    }

  _footStepTimer += getTimerSys()->getDeltaTime();
  if(_wasOnGround) {
        if(!entity->getCollidable()->getRestingOnBody()) {
      getAudioSys()->setSfxChannelGain(0, 0.0f);
      _wasOnGround = false;
    }
  } else {
        if(entity->getCollidable()->getRestingOnBody()) {
      getAudioSys()->setSfxChannelGain(0, 1.0f);
      _wasOnGround = true;
    }
  }

  if(moved) {

    //if(!_footStepSfxLooping) {
      GYVector3 worldPos = entity->getCollidable()->getWorldPos();
      GYWav* walkSfx = nullptr;
      for(unsigned i = 0; i < 3; ++i) {
                TileEntry tileEntry = getCurrentArea()->getTileEntry(i, (worldPos.x+16.0f)/32.0f, (worldPos.y+16.0f)/32.0f);
        if(tileEntry.getElev() == entity->getElev()) {
                    auto* sfx = getCurrentLevel()->getTile(tileEntry.getIndex())->walkSfx;
          if(sfx) walkSfx = sfx;
        }
      }

      if(walkSfx) {
        if(walkSfx != _curWalkSfx) {
          getAudioSys()->setSfxChannelLooping(0, true);
          getAudioSys()->stopSfxChannel(0);
          getAudioSys()->setSfxChannelGain(0, 1.0f);
          getAudioSys()->playSfxChannel(0, *walkSfx);
          _footStepSfxLooping = true;
          _curWalkSfx = walkSfx;
        } else if(!_footStepSfxLooping) {
          getAudioSys()->setSfxChannelGain(0, 1.0f);
          _footStepSfxLooping = true;
        }
      }

      _footStepTimer = 0.0f;
    //}


        entity->getCharacter()->walk(moveDist);
  } else {
    if(_footStepSfxLooping && _footStepTimer >= ELYSIAN_PARTY_SYSTEM_FOOT_STEP_SFX_TIME) {
      getAudioSys()->setSfxChannelGain(0, 0.0f);
      _footStepSfxLooping = false;
    }
        if(getPartyLeader()->getCharacter()->isDashing()) {
      getAudioSys()->setSfxChannelPitch(0, 1.0f);
    }

        getPartyLeader()->getCharacter()->setDashing(false);
  }
#if 0
    if(input.getTapped(GameInput::BUT_N)) {
        if(_characterParty.size() != 0) _checkPickup(_characterParty[0]);
    }
#endif
#endif


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





#if 0

  if(leader->getCharacter()->getHeldEntity()) {
    leader->getCharacter()->throwHeldEntity();
  } else {
    if(leader->getCollidable()->getContactCount() > 0) {
      auto& rigidBodies = _curArea->rigidBodies;
      for(auto it = rigidBodies.begin();
        it != rigidBodies.end();
        ++it)
      {
        if(getCollisionSys()->checkCollision(*leader, *(*it)->getEntity())) {
                    if((*it)->getEntity()->getRigidBody()->getAttrib()->getCanHold()) {
                        leader->getCharacter()->setHeldEntity((*it)->getEntity());
                        break;
                    }
        }
      }
        }
  }

#endif

}

bool PartySystem::loadPlayables(void) {
#if 0
  std::ifstream stream(gyFilePath(ELYSIAN_PARTY_PATH));
  Debug::printf(Debug::LOG_VERBOSE, "PartySystem::LoadPlayable(%s)", ELYSIAN_PARTY_PATH);
    Debug::push();

  if(!stream.is_open()) {
    Debug::printf(Debug::LOG_WARNING, "Could not open %s", ELYSIAN_PARTY_PATH);
        Debug::pop();
    return false;
  }

  stream >> _playableAmount;
  Debug::printf(Debug::LOG_VERBOSE, "_playableAmount - %d", _playableAmount);
  _playable = new Entity*[_playableAmount];

  for(unsigned i = 0; i < _playableAmount; ++i) {
    if(!stream.is_open()) {
      Debug::printf(Debug::LOG_WARNING, "\tCould not read %s.", ELYSIAN_PARTY_PATH);
            Debug::pop();
      return false;
    }

    _playable[i] = new Entity();
    _playable[i]->load(stream);
  }

    Debug::pop();
  return true;
#endif
  return true;
}

//bool PartySystem::loadParty(std::ifstream &/*stream*/) { return true; }
//bool PartySystem::saveParty(std::ofstream &/*stream*/) { return true; }

bool PartySystem::addPlayableToParty(const char *name) {
  for(unsigned int i = 0; i < _playableAmount; ++i) {
    if(strcmp(_playable[i]->getName(), name) == 0) {
      if(getPlayablePartyMember(name)) {
        Debug::printf(Debug::LOG_WARNING, "PartySystem::AddPlayableToParty(%s): Already in the party!", name);
        return false;
      }
      Debug::printf(Debug::LOG_VERBOSE, "%s has joined the party!", name);
      _playableParty.push_back(_playable[i]);
      _curArea->addEntity(_playable[i]);
      //_playable[i]->activate();
      if(_playableParty.size() == 1) getCameraSys()->setFollowEntity(_playable[i]);
      //else _playable[i]->transform.setPosition(_playableParty[_playableParty.size()-2]->transform.getPosition());
      _resizePartyPathPoints();
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "PartySystem::AddPlayableToParty(%s): Not a loaded playable!", name);
  return false;
}

bool PartySystem::addPlayableToParty(const Entity &entity) {
  for(unsigned int i = 0; i < _playableAmount; ++i) { //check if entity is in _playable
    if(_playable[i] == &entity) {
      for(unsigned int j = 0; j < _playableParty.size(); ++j) { //check if entity is already in _playableParty
        if(_playableParty[j] == &entity) {
          Debug::printf(Debug::LOG_WARNING, "PartySystem::AddPlayableToParty(Entity&): %s is already in the party!", entity.getName());
          return false;
        }
      }
      Debug::printf(Debug::LOG_VERBOSE, "%s has joined the party!", entity.getName());
      _playableParty.push_back(_playable[i]);
      //_playable[i]->activate();
      _curArea->addEntity(_playable[i]);
      if(_playableParty.size() == 1) getCameraSys()->setFollowEntity(_playable[i]);
      //else _playable[i]->transform.setPosition(_playableParty[_playableParty.size()-2]->transform.getPosition());
      _resizePartyPathPoints();
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "! - PartySystem::AddPlayableToParty(Entity&): Entity is not a loaded playable!");
  return false;
}



bool PartySystem::removePlayableFromParty(const char *name) {
  for(unsigned int i = 0; i < _playableParty.size(); ++i) {
    if(strcmp(_playableParty[i]->getName(), name) == 0) { //determine slot from name
      removePlayableFromParty(i);//invoke slot
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "! - PartySystem::RemovePlayableFromParty(const char[]): %s is not in the party!", name);
  return false;
}

bool PartySystem::removePlayableFromParty(const Entity &entity) {
  for(unsigned int i = 0; i < _playableParty.size(); ++i) {
    if(_playableParty[i] == &entity) { //determine slot from memory address
      removePlayableFromParty(i); //invoke slot
      return true;
    }
  }
  Debug::printf(Debug::LOG_WARNING, "! - PartySystem::RemovePlayableFromParty(Entity&): Entity is not in party!");
  return false;
}

bool PartySystem::removePlayableFromParty(const unsigned int slot) {
  if(slot >= _playableParty.size()) {
    Debug::printf(Debug::LOG_WARNING, "! PartySystem::RemovePlayableFromParty(const uint): Invalid slot!");
    return false;
  }
  Debug::printf(Debug::LOG_VERBOSE, "%s has left the party!", _playableParty[slot]->getName());
  _playableParty[slot]->deactivate();


  //update character positions
  if(_characterParty.size() > 0) {
        for(unsigned int i = _characterParty.size()-1; i > 0; --i) _characterParty[i]->_transform._pos = _characterParty[i-1]->_transform._pos;
        _characterParty[0]->_transform._pos = _playableParty[_playableParty.size()-1]->_transform._pos;
  }
  //update playable positions
    for(unsigned int i = _playableParty.size()-1; i > slot; --i) _playableParty[i]->_transform._pos = _playableParty[i-1]->_transform._pos;


  _playableParty.erase(_playableParty.begin() + slot);
  if(slot == 0) {
    if(_playableParty.size() > 0) getCameraSys()->setFollowEntity(_playableParty[0]);
        else getCameraSys()->setFollowEntity(nullptr);
  }
  _resizePartyPathPoints();
  return true;
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
  return (_playableParty.size() == 0)?
        (_characterParty.size() == 0)? nullptr : _characterParty.front() :
    _playableParty.front();
}

bool PartySystem::addCharacterToParty(Entity *entity) {
    if(getPartySlot(entity) != -1) return false;

    if(!entity->getBehavior(ELYSIAN_PLAYABLE_BEHAVIOR_NAME)) {
        auto* behav = new game::PlayableBehavior(*entity);
        entity->addBehavior(behav);
        behav->setPlayerNum(_characterParty.size());
    }

    Debug::printf(Debug::LOG_VERBOSE, "%s has joined the party!", entity->getName());
    _characterParty.push_back(entity);
    if(_characterParty.size() > 1) entity->_transform.setPos(_partyPathPoint[_partyPathPoint.size()-1]);
    if(_playableParty.size() == 0 && _characterParty.size() == 1) getCameraSys()->setFollowEntity(entity);

    if(_characterParty.size() > 1) {
        getCameraSys()->addEntity(entity);
        getGuiSys()->getAdvLog().printf("~c02%s ~c00has joined the party!", entity->getName());
    }

    _resizePartyPathPoints();
    return true;
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
    return NULL;
}

Entity *PartySystem::getCharacterPartyMember(const unsigned int slot) const {
  if(slot >= _characterParty.size()) {
        //Debug::printf(Debug::LOG_WARNING, "PartySystem::getCharacterPartyMember(const uint): %d is an invalid slot!", slot);
        return nullptr;
  }
  return _characterParty[slot];
}

void PartySystem::_resizePartyPathPoints(void) {
  int pathPointAmount = 0;
  //check if even necessary
  if((pathPointAmount = (_playableParty.size()-1 + _characterParty.size())*_pointsPerCharacter+1) == (int)_partyPathPoint.size()) return;
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
  if(_playableParty.size() == 0 && _characterParty.size() == 0) return;
  if(_playableParty.size() == 1 && _characterParty.size() == 0) return;

  //Keep distance between party
  if(_playableParty.size()) {
        if(fabs(_partyPathPoint[0].x - _playableParty[0]->_transform._pos.x) < 2.50f && fabs(_partyPathPoint[0].y - _playableParty[0]->_transform._pos.y) < 2.0f) return;
  }
  else if(_characterParty.size()) {
        if(fabs(_partyPathPoint[0].x - _characterParty[0]->_transform._pos.x) < 2.0f && fabs(_partyPathPoint[0].y - _characterParty[0]->_transform._pos.y) < 2.0f) return;
  }

  //update chain of points
  for(i = _partyPathPoint.size()-1; i > 0; --i) {
    _partyPathPoint[i] = _partyPathPoint[i-1];
  }

  //add leader's current position
    _partyPathPoint[0] = (_playableParty.size() > 0)? GYVector2{_playableParty[0]->_transform._pos.x, _playableParty[0]->_transform._pos.y } : GYVector2 {_characterParty[0]->_transform._pos.x, _characterParty[0]->_transform._pos.y };

  unsigned int currentIndex = 2;
  //update playables
  unsigned int activeSize = (_playableParty.size() < _activePlayablePartySize)? _playableParty.size() : _activePlayablePartySize;
  for(i = 1; i < activeSize; ++i) {
    GYVector2 dist;
    currentIndex += _pointsPerCharacter-1;
        //Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
    //Debug::Print(Debug::NORMAL, "x - %d, y - %d\n", currentIndex, currentIndex);
    gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
        _playableParty[i]->getCharacter()->animateWalk(dist);
        _playableParty[i]->_transform.setPos(_partyPathPoint[currentIndex-1]);
  }
  //update characters
  //--currentIndex;

  i = (_playableParty.size())? 0 : 1;
  for(; i < _characterParty.size(); ++i) {
    GYVector2 dist;
    currentIndex += _pointsPerCharacter-1;
    //Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
    gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
        _characterParty[i]->getCharacter()->animateWalk(dist);
        _characterParty[i]->_transform.setPos(_partyPathPoint[currentIndex-1]);
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
        for(unsigned int i = 0; i < _playableParty.size(); ++i)
            _curArea->addEntity(_playableParty[i]);
        for(unsigned int i = 0; i < _characterParty.size(); ++i)
            _curArea->addEntity(_characterParty[i]);
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
        partySys->getPartSys()->getAttrib(0)->setEndColor(GYColor4 { 255, 255, 255, 0});
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


}



