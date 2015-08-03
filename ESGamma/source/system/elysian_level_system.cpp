#include "elysian_level_system.hpp"
#include <crtp/elysian_level.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_entity.hpp>
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tk_map_system.h")
#include <crtp/elysian_depth_buff_entry.hpp>
#include "elysian_map_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_audio_system.hpp"
#include "elysian_day_system.hpp"
#include "elysian_debug.hpp"

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

void LevelSystem::removeArea(Area * const area) {
    for(auto it = _area.begin(); it != _area.end(); ++it) {
        if(*it == area) {
            if(_curArea == area) _curArea = nullptr;
            _area.erase(it);
            return;
        }
    }
}

void LevelSystem::removeLevel(Level * const level) {
    for(auto it = _level.begin(); it != _level.end(); ++it) {
        if(*it == level) {
            if(_curLevel == level) _curLevel = nullptr;
            _level.erase(it);
            return;
        }
    }
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

    if(_level.size() >= ELYSIAN_SEVERITYS_LOADED_MAX) {
        delete _level[0];
        _level.erase(_level.begin());
    }
#ifndef ELYSIAN_ENGINE
    Q_ASSERT(false); //YOU SHOULD ALWAYS HAVE CREATED THE AREA BEFORE SWITCH IN ESTK!!!!!!1111
#endif
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
        getBehavSys()->callTriggers(Behavior::ON_EXIT);
        getPartySys()->removePartyFromArea();
        _removePersistentEntities();
        _curArea->deactivate();
        getCollisionSys()->onAreaExit();
        getMapSys()->deactivateArea();
    }


    if(!dir) {
        _curArea = nullptr;
        return false;
    }

    if(_curArea) strcpy(_prevAreaPath, _curArea->dir);

    //Check if new area has already been loaded
    if((index = getAreaIndex(dir)) >= 0) {
        _curArea = _area[index];
        _curArea->activate();
        getPartySys()->addPartyToArea();
        _addPersistentEntities();
        getBehavSys()->callTriggers(Behavior::ON_ENTER);
        getCollisionSys()->onAreaEnter();
        return true;
    }

    //Check if we need to free an area before loading the next one
    if(_area.size() >= ELYSIAN_AREAS_LOADED_MAX) {
        getBehavSys()->callTriggers(Behavior::ON_UNLOAD);
        delete _area[0];
        _area.erase(_area.begin());
    }

#ifndef ELYSIAN_ENGINE
    Q_ASSERT(false); //YOU SHOULD ALWAYS HAVE CREATED THE AREA BEFORE SWITCH IN ESTK!!!!!!1111
#endif

    //Allocate new area and switch to it
    ar = new Area;//(dir, *curLevel);
    _curArea = ar;
    ar->setLevel(_curLevel);
    ar->load(dir);
    _area.push_back(ar);
    ar->activate();
    getBehavSys()->callTriggers(Behavior::ON_LOAD);
    getPartySys()->addPartyToArea();
    _addPersistentEntities();
    getBehavSys()->callTriggers(Behavior::ON_ENTER);
    getCollisionSys()->onAreaEnter();
    getAudioSys()->update();
    return true;
}

void LevelSystem::_removePersistentEntities(void) {
    auto& entities = _curArea->entities;

    for(auto& it : entities) {
        if(it->isPersistent()) {
            _persistentPool.claimFreeEntry() = it;
            _curArea->removeEntity(it);
        }
    }
}

void LevelSystem::_addPersistentEntities(void) {
    for(auto entity = _persistentPool.begin(); entity < _persistentPool.end(); ++entity) {
        _curArea->addEntity(*entity);
    }
}


}
