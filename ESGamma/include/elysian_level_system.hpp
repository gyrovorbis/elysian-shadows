#ifndef ELYSIAN_LEVEL_SYSTEM_HPP
#define ELYSIAN_LEVEL_SYSTEM_HPP

#include <vector>
#include "elysian_system.hpp"

#include <gyro.h>

namespace elysian {

	struct Level;
	struct Area;

	class LevelSystem: public System { 
	private:
		char _prevAreaPath[GYRO_PATH_MAX_SIZE]	= { 0 };
		char _prevLevelPath[GYRO_PATH_MAX_SIZE] = { 0 };

		std::vector<Level*> level;
		std::vector<Area*>  area;

		void _onAreaEnter(void);
		void _onAreaExit(void);
		
	public:

        static const unsigned MAX_LEVELS_LOADED = 1;
#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_iOS
		static const unsigned MAX_AREAS_LOADED = 1;
#elif GYRO_PLATFORM == GYRO_PLATFORM_PSP
        static const unsigned MAX_AREAS_LOADED = 2;
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
        static const unsigned MAX_AREAS_LOADED = 1;
#endif
		

		int getLevelIndex(const char *const dir) const;
		int getAreaIndex(const char *const dir) const;

		bool switchLevel(const char *const dir);
		bool switchArea(const char *const dir);

		const char* getPrevLevelPath(void) const { return _prevLevelPath; }
		const char* getPrevAreaPath(void) const { return _prevAreaPath; }

		void walkthrough(void); //Print currently loaded assets
	};

}


#endif
