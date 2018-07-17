#ifndef ELYSIAN_LEVEL_SYSTEM_BASE_HPP
#define ELYSIAN_LEVEL_SYSTEM_BASE_HPP

#include <system/elysian_system.hpp>
#include <utility/elysian_mem_pool.hpp>
#include <gyro_defines.h>

#include <vector>

#define ELYSIAN_LEVEL_SYSTEM_PERSISTENT_ENTITY_MAX  10

#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_ANDROID || GYRO_PLATFORM == GYRO_PLATFORM_iOS
#   define ELYSIAN_LEVEL_CACHE_SIZE          100
#   define ELYSIAN_AREA_CACHE_SIZE           300

#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt
#   define ELYSIAN_LEVEL_CACHE_SIZE          100
#   define ELYSIAN_AREA_CACHE_SIZE           300

#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
#   define ELYSIAN_LEVEL_CACHE_SIZE          1
#   define ELYSIAN_AREA_CACHE_SIZE           1

#endif


ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Area)
ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class LevelSystemBase: public System {
    private:
        Pool<Entity*>       _persistentPool;
        std::vector<Level*> _levels;
        std::vector<Area*>  _areas;
        char                _prevAreaPath[GYRO_PATH_MAX_SIZE]   = { 0 };
        char                _prevLevelPath[GYRO_PATH_MAX_SIZE]  = { 0 };

    protected:

        void                _onAreaEnter(void);
        void                _onAreaExit(void);

        void                _removePersistentEntities(void);
        void                _addPersistentEntities(void);

    public:

        // TOOLKIT API (Levels/Areas managed externally)
        Level*              loadLevel(const char*const path);   //creates new + loads + adds to area
        Area*               loadArea(const char*const path);

        bool                unloadLevel(Level* level);  //unloads + removes from area
        bool                unloadArea(Area* area);

        void                addArea(Area*const area);
        void                addLevel(Level*const level);

        bool                removeArea(Area*const area);
        bool                removeLevel(Level*const level);

        // ENGINE API (all loading/caching/unloading handled internally)
        Level*              findLevel(const char* path);
        Area*               findArea(const char* path);

        bool                switchLevel(const char *const dir);
        bool                switchArea(const char *const dir);

        const char*         getPrevLevelPath(void) const;
        const char*         getPrevAreaPath(void) const;
	};








    //======== INLINEZ =========
    inline void LevelSystemBase::addArea(Area * const area) {
        _areas.push_back(area);
    }

    inline void LevelSystemBase::addLevel(Level * const level) {
        _levels.push_back(level);
    }

    inline const char* LevelSystemBase::getPrevLevelPath(void) const { return _prevLevelPath; }
    inline const char* LevelSystemBase::getPrevAreaPath(void) const { return _prevAreaPath; }


}


#endif
