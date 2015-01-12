#include "elysian_level_system.hpp"
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
#include "elysian_map_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_day_system.hpp"
#include "elysian_debug.hpp"

#include <gyro.h>

namespace elysian {

int LevelSystem::getLevelIndex(const char *const dir) const {
    for(unsigned int i = 0; i < _level.size(); ++i)
        if(strcmp(_level[i]->path, dir) == 0) return i;
    return -1;
}

int LevelSystem::getAreaIndex(const char *const dir) const {
    for(unsigned int i = 0; i < _area.size(); ++i)
        if(strcmp(_area[i]->path, dir) == 0) return i;
    return -1;
}

Level* LevelSystem::loadLevel(const char * const dir) {
    auto* lev = new Level;
    lev->load(dir);
    _level.push_back(lev);
    return lev;
}

Area* LevelSystem::loadArea(const char * const dir) {
    auto* ar = new Area;//(dir, *curLevel);
    ar->setLevel(_curLevel);
    ar->load(dir);
    _area.push_back(ar);
    return ar;
}

bool LevelSystem::switchLevel(const char *const dir) {
    int index;
    Level *lev;

    if(_curLevel) {
        _curLevel->deactivate();
        strcpy(_prevLevelPath, _curLevel->dir);
    }

    if(!dir) {
        _curLevel = nullptr;
        return false;
    }

    if((index = getLevelIndex(dir)) >= 0) {
        _level[index]->activate();
        _curLevel = _level[index];
        return true;
    }

    if(_level.size() >= ELYSIAN_LEVELS_LOADED_MAX) {
        delete _level[0];
        _level.erase(_level.begin());
    }
    lev = new Level;
    _curLevel = lev;
    lev->load(dir);
    lev->activate();
    _level.push_back(lev);
    return true;
}

bool LevelSystem::switchArea(const char *const dir) {
    int index;
    Area *ar;


    if(_curArea != nullptr) { //Can't call that if no area has been loaded yet.
        //Clean up current area
        getPartySys()->removePartyFromArea();
        getBehavSys()->callTriggers(Behavior::ON_EXIT);
        _curArea->deactivate();
        getCollisionSys()->clearContacts();
        getMapSys()->resetAnimation();
    }


    if(!dir) {
        _curArea = nullptr;
        return false;
    }

    if(_curArea) strcpy(_prevAreaPath, _curArea->dir);

    //Check if new area has already been loaded
    if((index = getAreaIndex(dir)) >= 0) {
        _curArea = _area[index];
        getPartySys()->addPartyToArea();
        getBehavSys()->callTriggers(Behavior::ON_ENTER);
        _curArea->activate();
        return true;
    }

    //Check if we need to free an area before loading the next one
    if(_area.size() >= ELYSIAN_AREAS_LOADED_MAX) {
        getBehavSys()->callTriggers(Behavior::ON_UNLOAD);
        delete _area[0];
        _area.erase(_area.begin());
    }

    //Allocate new area and switch to it
    ar = new Area;//(dir, *curLevel);
    _curArea = ar;
    ar->setLevel(_curLevel);
    ar->load(dir);
    _area.push_back(ar);
    ar->activate();
    getBehavSys()->callTriggers(Behavior::ON_LOAD);
    getPartySys()->addPartyToArea();
    getBehavSys()->callTriggers(Behavior::ON_ENTER);
    getAudioSys()->update();
    return true;
}


}
