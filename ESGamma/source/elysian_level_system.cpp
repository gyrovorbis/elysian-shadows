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
    for(unsigned int i = 0; i < level.size(); ++i)
        if(strcmp(level[i]->path, dir) == 0) return i;
    return -1;
}

int LevelSystem::getAreaIndex(const char *const dir) const {
    for(unsigned int i = 0; i < area.size(); ++i)
        if(strcmp(area[i]->path, dir) == 0) return i;
    return -1;
}

Level* LevelSystem::loadLevel(const char * const dir) {
    auto* lev = new Level;
    lev->load(dir);
    level.push_back(lev);
    return lev;
}

Area* LevelSystem::loadArea(const char * const dir) {
    auto* ar = new Area;//(dir, *curLevel);
    ar->setLevel(_curLevel);
    ar->load(dir);
    area.push_back(ar);
    return ar;
}

bool LevelSystem::switchLevel(const char *const dir) {
    int index;
    Level *lev;

    if(_curLevel) {
        _curLevel->deactivate();
        strcpy(_prevLevelPath, _curLevel->dir);
    }

    if((index = getLevelIndex(dir)) >= 0) {
        level[index]->activate();
        _curLevel = level[index];
        return true;
    }

    if(level.size() >= ELYSIAN_LEVELS_LOADED_MAX) {
        delete level[0];
        level.erase(level.begin());
    }
    lev = new Level;
    _curLevel = lev;
    lev->load(dir);
    lev->activate();
    level.push_back(lev);
    return true;
}

bool LevelSystem::switchArea(const char *const dir) {
    int index;
    Area *ar;

    //Clean up current area
    getPartySys()->removePartyFromArea();
    if(_curArea != nullptr) { //Can't call that if no area has been loaded yet.
        getBehavSys()->callTriggers(Behavior::ON_EXIT);
        _curArea->deactivate();
    }
    getCollisionSys()->clearContacts();
    getMapSys()->resetAnimation();

    if(_curArea) strcpy(_prevAreaPath, _curArea->dir);

    //Check if new area has already been loaded
    if((index = getAreaIndex(dir)) >= 0) {
        _curArea = area[index];
        getPartySys()->addPartyToArea();
        getBehavSys()->callTriggers(Behavior::ON_ENTER);
        _curArea->activate();
        return true;
    }

    //Check if we need to free an area before loading the next one
    if(area.size() >= ELYSIAN_AREAS_LOADED_MAX) {
        getBehavSys()->callTriggers(Behavior::ON_UNLOAD);
        delete area[0];
        area.erase(area.begin());
    }

    //Allocate new area and switch to it
    ar = new Area;//(dir, *curLevel);
    _curArea = ar;
    ar->setLevel(_curLevel);
    ar->load(dir);
    area.push_back(ar);
    ar->activate();
    getBehavSys()->callTriggers(Behavior::ON_LOAD);
    getPartySys()->addPartyToArea();
    getBehavSys()->callTriggers(Behavior::ON_ENTER);
    getAudioSys()->update();
    return true;
}


}
