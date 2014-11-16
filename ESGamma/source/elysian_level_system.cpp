#include "elysian_level_system.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")
#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
#include "elysian_party_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_day_system.hpp"
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

bool LevelSystem::switchLevel(const char *const dir) {
	int index;
	Level *lev;

    if(_curLevel) strcpy(_prevLevelPath, _curLevel->getDirName());

	if((index = getLevelIndex(dir)) >= 0) {
		_curLevel = level[index];
		return true;
	}

	if(level.size() >= MAX_LEVELS_LOADED) {
		delete level[0];
		level.erase(level.begin());
	}
	lev = new Level;
	_curLevel = lev;
	lev->load(dir);
	level.push_back(lev);
	return true;
}

bool LevelSystem::switchArea(const char *const dir) {
	int index;
	Area *ar;

	_onAreaExit();

    if(_curArea) strcpy(_prevAreaPath, _curArea->getDirName());
	
	//Check if new area has already been loaded
	if((index = getAreaIndex(dir)) >= 0) {
		_curArea = area[index];
		_onAreaEnter();
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
	_onAreaEnter();
	return true;
}

void LevelSystem::_onAreaEnter(void) {
	getPartySys()->addPartyToArea();
	getDaySys()->onAreaEnter();
	getAudioSys()->update();
    getCollisionSys()->onAreaEnter();

	getBehavSys()->callTriggers(Behavior::ON_ENTER);
}


void LevelSystem::_onAreaExit(void) {
    if(_curArea != nullptr) { //Can't call that if no area has been loaded yet.
		getBehavSys()->callTriggers(Behavior::ON_EXIT);
		getDaySys()->onAreaExit();
	}
	//Clean up current area
	getPartySys()->removePartyFromArea();
    getCollisionSys()->onAreaExit();
	getMapSys()->resetAnimation();
}


}
