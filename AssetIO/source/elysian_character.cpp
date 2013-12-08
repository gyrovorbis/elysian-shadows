#include "elysian_character.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_animation.hpp"
#include "elysian_generic.hpp"

namespace elysian {


Character::Character(Entity& entity): Component(entity), _movedThisFrame(false), _moveDir(DIR_NONE) {
}

bool Character::save(std::ofstream& /*stream*/) {
	dbgprintf(LOG_VERBOSE, "Character::Save(ifstream)");
	return true;
}

bool Character::load(std::ifstream& /*stream*/) {
	dbgprintf(LOG_VERBOSE, "Character::Load(ifstream)");
	
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
void Character::update(void) {
	if(!_movedThisFrame) {
		switch(_moveDir) {
		case DIR_UP:
			_entity->getAnimation()->playAnimation("StandUp");
			break;
		case DIR_DOWN:
			_entity->getAnimation()->playAnimation("StandDown");
			break;
		case DIR_LEFT:
			_entity->getAnimation()->playAnimation("StandLeft");
			break;
		case DIR_RIGHT:
			_entity->getAnimation()->playAnimation("StandRight");
			break;
		default: break;
		}
	}
	else _movedThisFrame = false;
}


void Character::face(const DIR4 dir) {
	Animation *animComp = _entity->getAnimation();
	switch(dir) {
		case DIR_UP:
			animComp->playAnimation("StandUp");
			break;
		case DIR_DOWN:
			animComp->playAnimation("StandDown");
			break;
		case DIR_LEFT:
			animComp->playAnimation("StandLeft");
			break;
		case DIR_RIGHT:
			animComp->playAnimation("StandRight");
			break;
		default:
			break;
	}
	_moveDir = dir;
}

void Character::face(Entity &entity) {
	GYVector2 dist;
	gyVec2Sub(&dist, &entity.transform._pos, &_entity->transform._pos);
	face(Direction::fromVector(dist));
}

void Character::move(const GYVector2 &dist) {
	if(gyVec2IsZero(&dist)) return;

	_entity->transform.translate(dist);

	_movedThisFrame = true;
	DIR4 dir = Direction::fromVector(dist);

	Animation *animComp = _entity->getAnimation();
	//if(dir == _moveDir && animComp->isActive()) return;

	switch(dir) {
		case DIR_UP:
			animComp->playAnimation("WalkUp");
			break;
		case DIR_DOWN:
			animComp->playAnimation("WalkDown");
			break;
		case DIR_LEFT:
			animComp->playAnimation("WalkLeft");
			break;
		case DIR_RIGHT:
			animComp->playAnimation("WalkRight");
			break;
		default:
			break;
	}

	_moveDir = dir;

}
#endif

}
