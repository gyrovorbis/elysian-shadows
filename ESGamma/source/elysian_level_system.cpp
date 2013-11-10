#include "elysian_level_system.hpp"
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_party_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_map_system.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_debug.hpp"

#include <gyro.h>

namespace elysian {

int LevelSystem::getLevelIndex(const char *const dir) const {
	for(unsigned int i = 0; i < level.size(); ++i)
		if(strcmp(level[i]->path, dir) == 0) return i;
	return -1;
}

int LevelSystem::getAreaIndex(const char *const dir) const { 
	for(unsigned int i = 0; i < area.size(); ++i)
		if(strcmp(area[i]->path, dir) == 0) return i;
	return -1;
}

bool LevelSystem::SwitchLevel(const char *const dir) {
	int index;
	Level *lev;

	if((index = getLevelIndex(dir)) >= 0) {
		_curLevel = level[index];
		return true;
	}

	if(level.size() >= MAX_LEVELS_LOADED) {
		Debug::printf(Debug::VERBOSE, "LEVEL DBG - %s", level[0]->path);
		delete level[0];
		level.erase(level.begin());
	}
	lev = new Level;
	_curLevel = lev;
	lev->load(dir);
	level.push_back(lev);
	return true;
}

bool LevelSystem::SwitchArea(const char *const dir) {
	int index;
	Area *ar;

	//Clean up current area
	getPartySys()->removePartyFromArea();
	if(_curArea != NULL) { //Can't call that if no area has been loaded yet.
		getBehavSys()->callTriggers(Behavior::ON_EXIT);
	}
	getCollisionSys()->clearContacts();
	getMapSys()->resetAnimation();
	
	//Check if new area has already been loaded
	if((index = getAreaIndex(dir)) >= 0) {
		_curArea = area[index];
		getPartySys()->addPartyToArea();
		getBehavSys()->callTriggers(Behavior::ON_ENTER);
		return true;
	}

	//Check if we need to free an area before loading the next one
	if(area.size() >= MAX_AREAS_LOADED) {
		getBehavSys()->callTriggers(Behavior::ON_UNLOAD);
		delete area[0];
		area.erase(area.begin());
	}

	//Allocate new area and switch to it
	ar = new Area;//(dir, *curLevel);
	_curArea = ar;
	ar->load(dir, _curLevel);
	area.push_back(ar);
	getBehavSys()->callTriggers(Behavior::ON_LOAD);
	getPartySys()->addPartyToArea();
	getBehavSys()->callTriggers(Behavior::ON_ENTER);
	getAudioSys()->update();
	return true;
}


}
