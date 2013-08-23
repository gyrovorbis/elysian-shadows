#include "elysian_examinable.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

	//overridden from Component
	bool Examinable::canAddTo(Entity& entity) {
		return entity.getCollidable();
	}

	bool Examinable::load(std::ifstream& stream) {
		return true;
	}

	bool Examinable::save(std::ofstream& stream) {
		return true;
	}

	bool Examinable::addToLevel(void) {
		_curArea->examinables.push_back(this);
		return true;
	}
	
	bool Examinable::removeFromLevel(void) {
		for(auto it = _curArea->examinables.begin();
			it != _curArea->examinables.end();
			++it) 
		{
			if(*it == this) {
				_curArea->examinables.erase(it);
				return true;
			}
		}
		return false;
	}

	Examinable::TYPE Examinable::getType(void) const {
		return EXAMINABLE;
	}

}