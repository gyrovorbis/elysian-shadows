#include "elysian_enemy.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

namespace elysian {

//====NONSTATIC====


bool Enemy::canAddTo(Entity& parent) {
	//eventually probably should add RIGIDBODY
    return (parent.getCollidable() && parent.getAnimation() && parent.getCharacter());
	
}
bool Enemy::load(std::ifstream& /*stream*/) {
    dbgprintf(LOG_VERBOSE, "Enemy::Load(ifstream&)\n");
	return true;
}

bool Enemy::save(std::ofstream& /*stream*/) {
    dbgprintf(LOG_VERBOSE, "Enemy::Save(ofstream&)\n");
	return true;
}

bool Enemy::addToLevel(void) {
	_curArea->enemies.push_back(this);
	return true;
}

bool Enemy::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->enemies);
}

}
