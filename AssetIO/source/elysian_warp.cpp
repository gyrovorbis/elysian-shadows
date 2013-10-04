#include "elysian_warp.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

namespace elysian {

Warp::Warp(Entity& parent): Component(parent), _changeArea(false), _changeLevel(false) {}
Warp::Warp(void): _changeArea(false), _changeLevel(false) {}
Warp::~Warp(void) {}


bool Warp::canAddTo(Entity& entity) {
    return (entity.getCollidable());
}

bool Warp::save(std::ofstream &stream) {
	dbgprintf(LOG_VERBOSE, "Warp::Save(ofstream)");
    dbgPush();
	stream << _toPosition.x << std::endl;
	stream << _toPosition.y << std::endl;
	stream << _changeArea << std::endl;
	if(_changeArea) stream << _areaPath << std::endl;
	stream << _changeLevel << std::endl;
	if(_changeLevel) stream << _levelPath << std::endl;
	dbgPop();
	return true;
}

bool Warp::load(std::ifstream &stream) {
	dbgprintf(LOG_VERBOSE, "Warp::Load(ifstream)");
    dbgPush();
	stream >> _toPosition.x;
	stream >> _toPosition.y;
	dbgprintf(LOG_VERBOSE, "_toPosition = <%f, %f>", _toPosition.x, _toPosition.y);
	stream >> _changeArea;
	if(_changeArea) {
		stream >> _areaPath;
		dbgprintf(LOG_VERBOSE, "_areaPath = %s", _areaPath);
	}
	stream >> _changeLevel;
	if(_changeLevel) {
		stream >> _levelPath;
		dbgprintf(LOG_VERBOSE, "_levelPath = %s", _levelPath);
	}
	dbgPop();
	return true;
}

//O(1)
bool Warp::addToLevel(void) {
	_curArea->warps.push_back(this);
	return true;
}

//O(n)
bool Warp::removeFromLevel(void) {
    containerEraseOnMatch(this, _curArea->warps);
    for(unsigned int i = 0; i < _curArea->warps.size(); ++i) {
        if(_curArea->warps[i] == this) {
            _curArea->warps.erase(_curArea->warps.begin() + i);
            return true;
        }
    }
    return false;
}

}
