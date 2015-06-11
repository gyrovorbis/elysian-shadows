#ifndef ELYSIAN_SEVERITY_MANAGER_HPP
#define ELYSIAN_SEVERITY_MANAGER_HPP

#include <elysian_asset_typedefs.hpp>

ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Area)

namespace elysian {

	class LevelManager { 
	protected:
        static Level* _curLevel;
        static Area* _curArea;
	public:
        static Area* getCurrentArea(void) { return _curArea; }
        static Level* getCurrentLevel(void) { return _curLevel; }
	}; 
	
}

#endif

