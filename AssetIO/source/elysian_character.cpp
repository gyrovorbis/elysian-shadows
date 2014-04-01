#include "elysian_character.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_animation.hpp"
#include "elysian_generic.hpp"

namespace elysian {

bool Character::save(FILE* file) {
	dbgprintf(LOG_VERBOSE, "Character::Save(FILE*)");
	return true;
}

bool Character::load(FILE* file) {
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
	_faceDir = dir;
}

void Character::move(const GYVector2 &dist) {
	if(gyVec2IsZero(&dist)) return;

	_entity->transform.translate(dist);
	_movedThisFrame = true;

	Direction::DIR4 dir = Direction::fromVector(dist);
	Animation *animComp = _entity->getAnimation();

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
	_faceDir = dir;
}

#endif


}
