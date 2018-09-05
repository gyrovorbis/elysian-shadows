#include <ESGamma/crtp/elysian_level_system.hpp>
#include <ESGamma/system/elysian_combat_system.hpp>
#include <AssetIO/crtp/elysian_level.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_depth_buff_entry.hpp>
#include <ESGamma/system/elysian_warp_system.hpp>
#include <ESGamma/crtp/elysian_map_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <ESGamma/system/elysian_behavior_system.hpp>
#include <ESGamma/system/elysian_collision_system.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <ESGamma/system/elysian_day_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>
#include <ESGamma/system/elysian_event_system.hpp>

namespace elysian {


Level* LevelSystemBase::findLevel(const char* path) {
    if(path) {
        for(size_t i = 0; i < _levels.size(); ++i)
            if(strcmp(_levels[i]->path, path) == 0)
                return _levels[i];
    }
    return nullptr;
}

Area* LevelSystemBase::findArea(const char* path) {
    if(path) {
         for(size_t i = 0; i < _areas.size(); ++i)
             if(strcmp(_areas[i]->path, path) == 0)
                 return _areas[i];
    }
    return nullptr;
}



Level* LevelSystemBase::loadLevel(const char * const path) {
    auto* newLevel = new Level;
    newLevel->load(path);
    addLevel(newLevel);
    return newLevel;
}

Area* LevelSystemBase::loadArea(const char * const path) {
    auto* ar = new Area;
    ar->setLevel(_curLevel);
    ar->load(path);
    addArea(ar);
    return ar;
}

bool LevelSystemBase::unloadArea(Area* area) {
    //Area must be current temporarily for BehaviorSystem
    removeArea(area);
    delete area;
    return true;
}

bool LevelSystemBase::unloadLevel(Level* level) {
    removeLevel(level);
    delete level;
    return true;
}

bool LevelSystemBase::removeArea(Area * const area) {
    for(auto it = _areas.begin(); it != _areas.end(); ++it) {
        if(*it == area) {
            if(_curArea == area) _curArea = nullptr;
            _areas.erase(it);
            return true;
        }
    }
    return false;
}

bool LevelSystemBase::removeLevel(Level * const level) {
    for(auto it = _levels.begin(); it != _levels.end(); ++it) {
        if(*it == level) {
            if(_curLevel == level) _curLevel = nullptr;
            _levels.erase(it);
            return true;
        }
    }
    return false;
}



bool LevelSystemBase::switchLevel(const char *const path) {
    Level* level = findLevel(path);

    //Check if level is already current
    if(level && _curLevel == level) {
#ifdef ELYSIAN_ENGINE //not a big deal in ESTk
        warnprintf("Attempting to switch to Level [%s] which is already current!", _curLevel->dir);
#endif
        return false;
    }

    //Deactivate current level
    if(_curLevel) {
        _curLevel->deactivate();
        strcpy(_prevLevelPath, _curLevel->dir);
    }

    //That's it if we're swapping to no level
    if(!path) {
        _curLevel = nullptr;
        return true;
    }

    if(!level) {
#ifndef ELYSIAN_ENGINE
        GY_ASSERT(false); //YOU SHOULD ALWAYS HAVE CREATED THE LEVEL BEFORE SWITCH IN ESTK!!!!!!1111
#endif
        if(_levels.size() >= ELYSIAN_LEVEL_CACHE_SIZE) {
            auto* oldLevel = _levels[0];
            DbgEvent event("Freeing Level [%s] to make room for new level!", oldLevel->dir);
            unloadLevel(oldLevel);
        }

        level = loadLevel(path);
        addLevel(level);
    }

    level->activate();
    _curLevel = level;
    return true;
}

bool LevelSystemBase::switchArea(const char *const path) {
    Area* area = findArea(path);

    if(area && _curArea == area) {
#ifdef ELYSIAN_ENGINE   //Don't give a shit in ESTk.
        warnprintf("Attempting to switch to Area [%s] which is already current!", _curArea->dir);
#endif
        return false;
    }

    if(_curArea != nullptr) { //Can't call that if no area has been loaded yet.
        getEventSys()->broadcastEvent(Event(EVENT_AREA_EXIT));
        _onAreaExit();
    }

    if(!path) {
        _curArea = nullptr;
        return true;
    }

    if(_curArea) strcpy(_prevAreaPath, _curArea->dir);

    if(!area) {
#ifndef ELYSIAN_ENGINE
        GY_ASSERT(false); //YOU SHOULD ALWAYS HAVE CREATED THE AREA BEFORE SWITCH IN ESTK!!!!!!1111
#endif

        //Check if we need to free an area before loading the next one
        if(_areas.size() >= ELYSIAN_AREA_CACHE_SIZE) {
            Area* oldArea = _areas[0];
            DbgEvent event("Freeing Area [%s] to make room for new area!", oldArea->dir);
            unloadArea(oldArea);
        }

        area = loadArea(path);
        addArea(area);
    }

    _curArea = area;
    _curArea->activate();

    Vector3 pos;
    if(getWarpSys()->getState() == WarpSystem::WARP) {
        pos = getWarpSys()->getCurrentWarp().getToPosVec();
    } else {
        pos = area->getStartPosition();
    }

    for(unsigned i = 0; i < getPartySys()->getPartySize(); ++i) {
        getPartySys()->getPartyMember(i)->getTransform().setPos(pos.getX(), pos.getY(), pos.getZ());
    }

  //              getPartySys()->getPartyLeader()->_transform.setPos(_currentWarp._toPosition);

    getEventSys()->broadcastEvent(Event(EVENT_AREA_ENTER));
    _onAreaEnter();
    return true;
}

void LevelSystemBase::_onAreaEnter(void) {

    _addPersistentEntities();
    getBehavSys()->callTriggers(Behavior::ON_ENTER);
    getCollisionSys()->onAreaEnter();
    getAudioSys()->update();
    getCameraSys()->update();
    getCombatSys()->onAreaEnter();
}

void LevelSystemBase::_onAreaExit(void) {
    //Clean up current area
    getBehavSys()->callTriggers(Behavior::ON_EXIT);
    _removePersistentEntities();
    _curArea->deactivate();
    getCollisionSys()->onAreaExit();
    getMapSys()->deactivateArea();
}

void LevelSystemBase::_removePersistentEntities(void) {
    auto& entities = _curArea->entities;
    std::vector<Entity*> toRemove;

    for(auto it : entities) {
        if(it->isPersistent()) {
            _persistentPool.claimFreeEntry() = it;
            toRemove.push_back(it);
        }
    }

    for(auto it : toRemove) {
        _curArea->removeEntity(it);
    }
}

void LevelSystemBase::_addPersistentEntities(void) {
    for(auto entity = _persistentPool.begin(); entity < _persistentPool.end(); ++entity) {
        _curArea->addEntity(*entity);
    }
    _persistentPool.clear();
}

}
