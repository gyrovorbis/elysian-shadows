#include "elysian_party_system.hpp"
#include "elysian_debug.hpp"
#include "elysian_entity.hpp"
#include "elysian_character.hpp"
#include "elysian_game_input.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_animation.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_collidable.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_examinable.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_particle_system.hpp"
#include "elysian_level.hpp"
#include "elysian_audio_system.hpp"

namespace elysian {

//----NONSTATIC----

PartySystem::PartySystem(void)
{}

PartySystem::~PartySystem() {
	for(unsigned int i = 0; i < _playableAmount; ++i) {
		delete _playable[i];
	}
	
	delete[] _playable;
}

void PartySystem::update(void) {//VOIDICATE
	_tapTimer += getTimerSys()->getDeltaTime();
    _dashPartTimer += getTimerSys()->getDeltaTime();
	//if(_playableParty.size() < 1) return;
	updatePartyPaths();
}

void PartySystem::render() {}

void PartySystem::processInput(const GameInput &input) {
	Entity*entity = getPartyLeader();
	if(!entity) return;

	const static GYVector2 rightMove = { 1.0f, 0.0f };
	const static GYVector2 leftMove = { -1.0f, 0.0f };
	const static GYVector2 upMove = { 0.0f, -1.0f };
	const static GYVector2 downMove = { 0.0f, 1.0f };
	bool moved = false;
	GYVector2 moveDist = { 0.0f, 0.0f };

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
        getPartyLeader()->getAnimation()->setSpeedMultiplier(1.0f);
        getAudioSys()->setSfxChannelPitch(0, 1.0f);
        _dashing = false;
        _moveSpeed = ELYSIAN_PARTY_SYSTEM_WALK_SPEED;
        _tapTimer = 0.0f;
    };

    auto emitDashParticles = [&](void) {
        getPartSys()->getAttrib(0)->setAcceleration(GYVector3{ 0.0f, -70.0f, /*7.0f*//*6.5f*/ });
        getPartSys()->getAttrib(0)->setForceMask(0);
        getPartSys()->getAttrib(0)->setEndColor(GYColor4 { 255, 255, 255, 0});
        getPartSys()->getAttrib(0)->setColor(255, 255, 255, 100);
        getPartSys()->getAttrib(0)->setMaxLife(30);
        getPartSys()->getAttrib(0)->setStick(false);
        GYVector3 pos = getPartyLeader()->transform._pos;
        pos.z += 5.0f;
        pos.y = getPartyLeader()->getDepthBaseY();
        getPartSys()->emitFromPoint(0, pos, _dashPartAngle, 500);
        _dashPartTimer = 0.0f;
    };

	auto updateDash = [&](const GameInput::BUTTON but) {
		bool updateTappedButton = true;

		if(_dashing) {
			bool stop = false;
			switch(_lastDpadTapped) {
				case GameInput::BUT_DPAD_U:
				  if(but == GameInput::BUT_DPAD_D) stop = true;
					break;
				case GameInput::BUT_DPAD_D:
					if(but == GameInput::BUT_DPAD_U) stop = true;
					 break;

				case GameInput::BUT_DPAD_L:
					if(but == GameInput::BUT_DPAD_R) stop = true;
					break;

				case GameInput::BUT_DPAD_R:
					if(but == GameInput::BUT_DPAD_L) stop = true;
					  break;
				default: break;
			}

			if(stop) {
                endDash();
			} else {
				updateTappedButton = false;
			}
		} else {
			if(but == _lastDpadTapped && _tapTimer <= _dashTapTimeWindow) {
                //float angle;

				switch(_lastDpadTapped) {
					case GameInput::BUT_DPAD_U:
                      _dashPartAngle = M_PI*0.5f;
						break;
					case GameInput::BUT_DPAD_D:
                        _dashPartAngle = -M_PI*0.5f;
						 break;

					case GameInput::BUT_DPAD_L:
                        _dashPartAngle = 0.0f;
						break;

					case GameInput::BUT_DPAD_R:
                        _dashPartAngle = -M_PI;
						  break;

					default: break;

				}

				_dashing = true;
				getAudioSys()->setSfxChannelPitch(0, 2.0f);
                _moveSpeed = _dashSpeed;
                getPartyLeader()->getAnimation()->setSpeedMultiplier(_dashAnimSpeedMultiplier);
//				getPartSys()->getAttrib(0)->setSize(3);
//				getPartSys()->getAttrib(0)->setFadeOut(true);
                emitDashParticles();
				//Debug::printf(Debug::VERBOSE, "DASHING!!!");
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

	
	if(input.getDown(GameInput::BUT_DPAD_U)){
		if(input.getTapped(GameInput::BUT_DPAD_U)) updateDash(GameInput::BUT_DPAD_U);
		gyVec2AddAccum(&moveDist, &upMove);
		moved = true;
	}
	if(input.getDown(GameInput::BUT_DPAD_D)){
		if(input.getTapped(GameInput::BUT_DPAD_D))  updateDash(GameInput::BUT_DPAD_D);
		gyVec2AddAccum(&moveDist, &downMove);
		moved = true;
	}
	
	if(input.getDown(GameInput::BUT_DPAD_L)){
		if(input.getTapped(GameInput::BUT_DPAD_L))  updateDash(GameInput::BUT_DPAD_L);
		gyVec2AddAccum(&moveDist, &leftMove);
		moved = true;
	}
	
	if(input.getDown(GameInput::BUT_DPAD_R)){
		if(input.getTapped(GameInput::BUT_DPAD_R)) updateDash(GameInput::BUT_DPAD_R);
		gyVec2AddAccum(&moveDist, &rightMove);
		moved = true;
	}

	//examine
	if(input.getTapped(GameInput::BUT_M)) {
		if(_playableParty.size() != 0) _checkExamine(_playableParty[0]);
		if(_characterParty.size() != 0) _checkExamine(_characterParty[0]);
	}


	if(input.getTapped(GameInput::BUT_I)) {
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
				TileEntry tileEntry = getCurArea()->getTileEntry(i, (worldPos.x+16.0f)/32.0f, (worldPos.y+16.0f)/32.0f);
				if(tileEntry.getElev() == entity->getElev()) {
					auto* sfx = getCurLevel()->getTile(tileEntry.getIndex())->walkSfx;
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



		gyVec2ScaleAccum(&moveDist, getTimerSys()->getDeltaTime()*_moveSpeed);
		entity->getCharacter()->move(moveDist);
	} else {
		if(_footStepSfxLooping && _footStepTimer >= ELYSIAN_PARTY_SYSTEM_FOOT_STEP_SFX_TIME) {
			getAudioSys()->setSfxChannelGain(0, 0.0f);
			_footStepSfxLooping = false;
		}
		if(_moveSpeed == ELYSIAN_PARTY_SYSTEM_DASH_SPEED) {
			getAudioSys()->setSfxChannelPitch(0, 1.0f);
		}

		_dashing = false;
		_moveSpeed = ELYSIAN_PARTY_SYSTEM_WALK_SPEED;
		//_curWalkSfx = nullptr;
		//getAudioSys()->setSfxChannelLooping(0, false);
		entity->getAnimation()->setSpeedMultiplier(1.0f);
	}

    if(input.getTapped(GameInput::BUT_N)) {
        if(_characterParty.size() != 0) _checkPickup(_characterParty[0]);
    }
}

void PartySystem::_checkPickup(Entity * const leader) const {
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
                    if((*it)->getEntity()->getRigidBody()->getAttrib()->canHold()) {
                        leader->getCharacter()->setHeldEntity((*it)->getEntity());
                        break;
                    }
				}
			}
		}

	}

}

bool PartySystem::loadPlayables(void) {
#if 0
	std::ifstream stream(gyFilePath(ELYSIAN_PARTY_PATH));
	Debug::printf(Debug::VERBOSE, "PartySystem::LoadPlayable(%s)", ELYSIAN_PARTY_PATH);
    Debug::push();
	
	if(!stream.is_open()) {
		Debug::printf(Debug::CRITICAL, "Could not open %s", ELYSIAN_PARTY_PATH);
        Debug::pop();
		return false;
	}
	
	stream >> _playableAmount;
	Debug::printf(Debug::VERBOSE, "_playableAmount - %d", _playableAmount);
	_playable = new Entity*[_playableAmount];
	
	for(unsigned i = 0; i < _playableAmount; ++i) {
		if(!stream.is_open()) {
			Debug::printf(Debug::CRITICAL, "\tCould not read %s.", ELYSIAN_PARTY_PATH);
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
				Debug::printf(Debug::CRITICAL, "PartySystem::AddPlayableToParty(%s): Already in the party!", name);
				return false;
			}
			Debug::printf(Debug::VERBOSE, "%s has joined the party!", name);
			_playableParty.push_back(_playable[i]);
			_curArea->addEntity(_playable[i]);
			//_playable[i]->activate();
			if(_playableParty.size() == 1) getCameraSys()->setFollowEntity(_playable[i]);
			//else _playable[i]->transform.setPosition(_playableParty[_playableParty.size()-2]->transform.getPosition());
			_resizePartyPathPoints();
			return true;
		}
	}
	Debug::printf(Debug::CRITICAL, "PartySystem::AddPlayableToParty(%s): Not a loaded playable!", name);
	return false;
}

bool PartySystem::addPlayableToParty(const Entity &entity) {
	for(unsigned int i = 0; i < _playableAmount; ++i) { //check if entity is in _playable
		if(_playable[i] == &entity) {
			for(unsigned int j = 0; j < _playableParty.size(); ++j) { //check if entity is already in _playableParty
				if(_playableParty[j] == &entity) {
					Debug::printf(Debug::CRITICAL, "PartySystem::AddPlayableToParty(Entity&): %s is already in the party!", entity.getName());
					return false;
				}
			}
			Debug::printf(Debug::VERBOSE, "%s has joined the party!", entity.getName());
			_playableParty.push_back(_playable[i]);
			//_playable[i]->activate();
			_curArea->addEntity(_playable[i]);
			if(_playableParty.size() == 1) getCameraSys()->setFollowEntity(_playable[i]);
			//else _playable[i]->transform.setPosition(_playableParty[_playableParty.size()-2]->transform.getPosition());
			_resizePartyPathPoints();
			return true;
		}
	}
	Debug::printf(Debug::CRITICAL, "! - PartySystem::AddPlayableToParty(Entity&): Entity is not a loaded playable!");
	return false;
}



bool PartySystem::removePlayableFromParty(const char *name) {
	for(unsigned int i = 0; i < _playableParty.size(); ++i) {
		if(strcmp(_playableParty[i]->getName(), name) == 0) { //determine slot from name
			removePlayableFromParty(i);//invoke slot
			return true;
		}
	}
	Debug::printf(Debug::CRITICAL, "! - PartySystem::RemovePlayableFromParty(const char[]): %s is not in the party!", name);
	return false;
}

bool PartySystem::removePlayableFromParty(const Entity &entity) {
	for(unsigned int i = 0; i < _playableParty.size(); ++i) {
		if(_playableParty[i] == &entity) { //determine slot from memory address
			removePlayableFromParty(i); //invoke slot
			return true;
		}
	}
	Debug::printf(Debug::CRITICAL, "! - PartySystem::RemovePlayableFromParty(Entity&): Entity is not in party!");
	return false;
}

bool PartySystem::removePlayableFromParty(const unsigned int slot) {
	if(slot >= _playableParty.size()) {
		Debug::printf(Debug::CRITICAL, "! PartySystem::RemovePlayableFromParty(const uint): Invalid slot!");
		return false;
	}
	Debug::printf(Debug::VERBOSE, "%s has left the party!", _playableParty[slot]->getName());
	_playableParty[slot]->deactivate();
	
	
	//update character positions
	if(_characterParty.size() > 0) {
		for(unsigned int i = _characterParty.size()-1; i > 0; --i) _characterParty[i]->transform._pos = _characterParty[i-1]->transform._pos;
		_characterParty[0]->transform._pos = _playableParty[_playableParty.size()-1]->transform._pos;
	}
	//update playable positions
	for(unsigned int i = _playableParty.size()-1; i > slot; --i) _playableParty[i]->transform._pos = _playableParty[i-1]->transform._pos;

	
	_playableParty.erase(_playableParty.begin() + slot);
	if(slot == 0) {
		if(_playableParty.size() > 0) getCameraSys()->setFollowEntity(_playableParty[0]);
		else getCameraSys()->setFollowEntity(NULL);
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
	Debug::printf(Debug::CRITICAL, "PartySystem::getPlayableFromParty(const char[]): %s is not in the party!", name);
	return NULL;
}

Entity *PartySystem::getPlayablePartyMember(const unsigned int slot) const {
	if(slot >= _playableParty.size()) {
		Debug::printf(Debug::CRITICAL, "PartySystem::getPlayableFromParty(const uint): Invalid slot %d!", slot);
		return NULL;
	}
	return _playableParty[slot];
}


Entity *PartySystem::getPartyLeader(void) const {
	return (_playableParty.size() == 0)? 
        (_characterParty.size() == 0)? NULL : _characterParty.front() : 
    _playableParty.front();
}

bool PartySystem::addCharacterToParty(Entity *entity) {
	for(unsigned int i = 0; i < _characterParty.size(); ++i) { //check if character is already in _characterParty
		if(_characterParty[i] == entity) {
			Debug::printf(Debug::CRITICAL, "PartySystem::AddCharacterToParty(Entity&): %s is already in the party!", entity->getName());
			return false;
		}
	}
	//Debug::printf(Debug::VERBOSE, "%s has joined the party!", entity->getName());
	_characterParty.push_back(entity);
	if( _characterParty.size() > 1) entity->transform.setPos(_partyPathPoint[_partyPathPoint.size()-1]);
	if(_playableParty.size() == 0 && _characterParty.size() == 1) getCameraSys()->setFollowEntity(entity);
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
	Debug::printf(Debug::CRITICAL, "! - PartySystem::RemoveCharacterFromParty(const char[]): %s is not in the party!", name);
	return false;
}

bool PartySystem::removeCharacterFromParty(const Entity *entity) {
	for(unsigned int i = 0; i < _characterParty.size(); ++i) {
		if(_characterParty[i] == entity) { //determine slot from memory address
			removeCharacterFromParty(i); //invoke slot
			return true;
		}
	}
	Debug::printf(Debug::CRITICAL, "! - PartySystem::RemoveCharacterFromParty(Entity&): %s is not in the party!", entity->getName());
	return false;
}

bool PartySystem::removeCharacterFromParty(const unsigned int slot) {
	if(slot >= _characterParty.size()) {
		Debug::printf(Debug::CRITICAL, "! - PartySystem::RemoveCharacterFromParty(const uint): %d is an invalid slot!", slot);
		return false;
	}
	Debug::printf(Debug::VERBOSE, "%s has left the party!", _characterParty[slot]->getName());
	//update positions
	for(unsigned int i = _characterParty.size()-1; i > slot; --i) _characterParty[i]->transform._pos = _characterParty[i-1]->transform._pos;
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
	Debug::printf(Debug::CRITICAL, "PartySystem::getCharacterPartyMember(const char[]): %s is not in the party!", name);
	return NULL;
}

Entity *PartySystem::getCharacterPartyMember(const unsigned int slot) const {
	if(slot >= _characterParty.size()) {
		Debug::printf(Debug::CRITICAL, "PartySystem::getCharacterPartyMember(const uint): %d is an invalid slot!", slot);
		return NULL;
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
			lastPoint = (_playableParty.size() != 0)? GYVector2 {_playableParty[0]->transform._pos.x, _playableParty[0]->transform._pos.y } : GYVector2 {_characterParty[0]->transform._pos.x, _characterParty[0]->transform._pos.y };
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
	
	Debug::printf(Debug::VERBOSE, "_partyPathPoint.size() - %d", _partyPathPoint.size());
}

void PartySystem::updatePartyPaths(void) {
	unsigned int i;
	if(_playableParty.size() == 0 && _characterParty.size() == 0) return;
	if(_playableParty.size() == 1 && _characterParty.size() == 0) return;

	//Keep distance between party
	if(_playableParty.size()) {
		if(fabs(_partyPathPoint[0].x - _playableParty[0]->transform._pos.x) < 2.50f && fabs(_partyPathPoint[0].y - _playableParty[0]->transform._pos.y) < 2.0f) return;
	}
	else if(_characterParty.size()) {
		if(fabs(_partyPathPoint[0].x - _characterParty[0]->transform._pos.x) < 2.0f && fabs(_partyPathPoint[0].y - _characterParty[0]->transform._pos.y) < 2.0f) return;
	}

	//update chain of points
	for(i = _partyPathPoint.size()-1; i > 0; --i) {
		_partyPathPoint[i] = _partyPathPoint[i-1];
	}
	
	//add leader's current position
	_partyPathPoint[0] = (_playableParty.size() > 0)? GYVector2{_playableParty[0]->transform._pos.x, _playableParty[0]->transform._pos.y } : GYVector2 {_characterParty[0]->transform._pos.x, _characterParty[0]->transform._pos.y };

	unsigned int currentIndex = 2;
	//update playables
	unsigned int activeSize = (_playableParty.size() < _activePlayablePartySize)? _playableParty.size() : _activePlayablePartySize;
	for(i = 1; i < activeSize; ++i) {
		GYVector2 dist;
		currentIndex += _pointsPerCharacter-1;
				//Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
		//Debug::Print(Debug::NORMAL, "x - %d, y - %d\n", currentIndex, currentIndex);
		gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
		_playableParty[i]->getCharacter()->move(dist);
	}
	//update characters
	//--currentIndex;

	i = (_playableParty.size())? 0 : 1;
	for(; i < _characterParty.size(); ++i) {
		GYVector2 dist;
		currentIndex += _pointsPerCharacter-1;
		//Debug::Print(Debug::NORMAL, "%d\n", currentIndex);
		gyVec2Sub(&dist, &_partyPathPoint[currentIndex-2], &_partyPathPoint[currentIndex-1]);
		_characterParty[i]->getCharacter()->move(dist);
	}
}

void PartySystem::removePartyFromArea(void) {
	for(unsigned int i = 0; i < _playableParty.size(); ++i) 
		_playableParty[i]->deactivate();
	for(unsigned int i = 0; i < _characterParty.size(); ++i)
		_characterParty[i]->deactivate();
}

void PartySystem::addPartyToArea(void) {
	for(unsigned int i = 0; i < _playableParty.size(); ++i) 
		_playableParty[i]->activate();
	for(unsigned int i = 0; i < _characterParty.size(); ++i)
		_characterParty[i]->activate();
}


void PartySystem::_checkExamine(Entity* const entity) const {
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

}



