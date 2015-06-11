#ifndef ELYSIAN_SEVERITY_SYSTEM_HPP
#define ELYSIAN_SEVERITY_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_mem_pool.hpp"
#include "tk_area.h"
#include "elysian_level.hpp"


#include <vector>

#define ELYSIAN_SEVERITY_SYSTEM_PERSISTENT_ENTITY_MAX  10

#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_ANDROID || GYRO_PLATFORM == GYRO_PLATFORM_iOS
#   define ELYSIAN_SEVERITYS_LOADED_MAX    1
#   define ELYSIAN_AREAS_LOADED_MAX     3

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt
#   define ELYSIAN_SEVERITYS_LOADED_MAX    100
#   define ELYSIAN_AREAS_LOADED_MAX     300

#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
#   define ELYSIAN_SEVERITYS_LOADED_MAX    1
#   define ELYSIAN_AREAS_LOADED_MAX     1

#endif


ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Area)

namespace elysian {




    template <typename T>
    class ZoneManager: public System {
    private:

        struct TimeStampEntry {
            T       zone;
            float   timestamp;
            //friend bool operator< (const TimeStampEntry& e1, const TimeStampEntry& e2);
        };

        typedef Pool<TimeStampEntry, true> ZonePool;

        ZonePool _entries = ZonePool(ELYSIAN_SEVERITYS_LOADED_MAX, 1);
        char     _prevPath[GYRO_PATH_MAX_SIZE] = { 0 };

    protected:

        typename ZonePool::iterator  _findZone(const char*const dir);

        //Compile-time polymorphic via template specializations!
        void                onLoadZone(void);
        void                onUnloadZone(void);
        void                onEnterZone(void);
        void                onExitZone(void);

        T*                  getCurZone(void);
        void                setCurZone(T*const zone);

    public:
        T*          getZone(const char*const dir);
        T*          loadZone(const char*const dir);
        bool        unloadZone(const char*const dir);
        bool        switchZone(const char*const dir);
        const char* getPrevZonePath(void) const;
    };

    class LevelSystem: public System {
        friend class ZoneManager<Area>;
        friend class ZoneManager<Level>;

	private:

        //ZoneManager<Area>    _areaManager;
        //ZoneManager<Level>    _levelManager;

        char            _prevAreaPath[GYRO_PATH_MAX_SIZE]   = { 0 };
        char            _prevLevelPath[GYRO_PATH_MAX_SIZE]  = { 0 };

        void            _onAreaEnter(void);
        void            _onAreaExit(void);

        int             _getLevelIndex(const char *const dir) const;
        int             _getAreaIndex(const char *const dir) const;

        void            _removePersistentEntities(void);
        void            _addPersistentEntities(void);
		
	public:

    private:

            std::vector<Level*> _level;
            std::vector<Area*>  _area;
            Pool<Entity*>       _persistentPool;;
    public:


        int             getLevelIndex(const char *const dir) const;
        int             getAreaIndex(const char *const dir) const;

        Level*          loadLevel(const char*const dir);
        Area*           loadArea(const char*const dir);

        bool            unloadLevel(const char*const dir);
        bool            unloadArea(const char*const dir);

        void            addArea(Area*const area);
        void            addLevel(Level*const level);

        void            removeArea(Area*const area);
        void            removeLevel(Level*const level);

        bool            switchLevel(const char *const dir);
        bool            switchArea(const char *const dir);

        const char*     getPrevLevelPath(void) const { return _prevLevelPath; }
        const char*     getPrevAreaPath(void) const { return _prevAreaPath; }

        void            walkthrough(void); //Print currently loaded assets
	};



    //======== INLINEZ =========
    inline void LevelSystem::addArea(Area * const area) {
        _area.push_back(area);
    }

    inline void LevelSystem::addLevel(Level * const level) {
        _level.push_back(level);
    }

    inline void LevelSystem::removeArea(Area * const area) {
        for(auto it = _area.begin(); it != _area.end(); ++it) {
            if(*it == area) {
                _area.erase(it);
                return;
            }
        }
    }

    inline void LevelSystem::removeLevel(Level * const level) {
        for(auto it = _level.begin(); it != _level.end(); ++it) {
            if(*it == level) {
                _level.erase(it);
                return;
            }
        }
    }









    // ==== INLINEZ =====
/*
    inline Area* LevelSystem::loadArea(const char * const dir) {
        return _loadZone<Area>(dir);
    }

    inline Level* LevelSystem::loadLevel(const char * const dir) {
        return _loadZone<Level>(dir);
    }

    template<typename T>
    inline bool operator< (const LevelSystem::TimeStampEntry<T>& e1, const LevelSystem::TimeStampEntry<T>& e2) {
        return (e1.timestamp < e2.timestamp);
    }

    template<typename T>
    inline TimeStampEntry<T>::iterator LevelSystem::ZoneManager<T>::_findZone(const char * const dir) {
        auto lambda = [&](const TimeStampEntry<T>& entry) {
            return (strcmp(dir, entry.name) == 0);
        };
        return _entries.findByComparator(lambda);
    }

    template<typename T>
    inline T* LevelSystem::ZoneManager<T>::loadZone(const char*const dir) {
        TimeStampEntry<T>* entry;

        entry = &_entries.claimFreeEntry();

        entry->zone.load(dir);
        entry->timestamp = getTimerSys()->getTotalTicks();

        return entry->zone;
    }

    template<typename T>
    inline T* LevelSystem::ZoneManager<T>::getZone(const char * const dir) {
        auto it = _findZone(dir);
        return it < _entries.end()? (*it) : nullptr;
    }

    template<typename T>
    inline bool LevelSystem::ZoneManager<T>::unloadZone(const char*const dir) {
        if(_findZone(dir) < _entries.end()) {
            _entries.remove(it);
            return true;
        }
        return false;
    }

    template<typename T>
    inline const char* LevelSystem::ZoneManager<T>::getPrevZonePath(void) {
        return _prevPath;
    }

    template<typename T>
    inline bool LevelSystem::ZoneManager<T>::switchZone(const char*const dir) {
        auto* zone = getZone(dir);

        if(!zone) {
            zone = loadZone(dir);
        }

        strcpy(_prevPath, getCurZone()->path);

    }

    template<>
    void ZoneManager<Area>::onLoadZone(void) {

    }

    template<>
    void ZoneManager<Area>::onUnloadZone(void) {

    }

    template<>
    void ZoneManager<Area>::onEnterZone(void) {

    }

    template<>
    void ZoneManager<Area>::onExitZone(void) {

    }

    template<>
    void ZoneManager<Area>::getCurZone(void) {

    }

    template<>
    void ZoneManager<Area>::setCurZone(void) {

    }
*/


}


#endif
