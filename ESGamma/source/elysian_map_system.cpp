
#include "elysian_collision_system.hpp"
#include "elysian_debug.hpp"
#include <gyro.h>

#include "tkMapSystem.h"
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
#include ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")
#include "elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_font.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_zcoords.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")

namespace elysian {

//Took animation into account!
#if 0
	Tile *TerrainSystem::getCurrentTile(const unsigned char index) const {
		return(!isValidTile(index))? NULL : &_curLevel->tileset[_currentTile[index]];
	}

	Tile *TerrainSystem::getCurrentObject(const unsigned char index) const {
		return(!isValidObject(index))? NULL : &_curLevel->tileset[_currentObject[index]];
	}
#endif

}
