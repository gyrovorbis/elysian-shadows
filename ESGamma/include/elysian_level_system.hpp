#ifndef ELYSIAN_LEVEL_SYSTEM_HPP
#define ELYSIAN_LEVEL_SYSTEM_HPP

#include <vector>
#include "elysian_system.hpp"

#include "gyro.h"

namespace elysian {

	struct Level;
	struct Area;

	class LevelSystem: public System { 
	private:

		std::vector<Level*> level;
		std::vector<Area*>  area;
		
	public:

        static const unsigned MAX_LEVELS_LOADED = 1;
#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_iOS
		static const unsigned MAX_AREAS_LOADED = 3;
#elif GYRO_PLATFORM == GYRO_PLATFORM_PSP
        static const unsigned MAX_AREAS_LOADED = 2;
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
        static const unsigned MAX_AREAS_LOADED = 1;
#endif
		

		int getLevelIndex(const char *const dir) const;
		int getAreaIndex(const char *const dir) const;

		bool SwitchLevel(const char *const dir);
		bool SwitchArea(const char *const dir);

        void Walkthrough(void); //Print currently loaded assets
	};

}


#endif