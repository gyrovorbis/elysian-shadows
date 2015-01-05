
#include "elysian_collision_system.hpp"
#include "elysian_debug.hpp"
#include <gyro.h>

#include "elysian_map_system.hpp"
//#include ELYSIAN_ASSET_HEADER("elysian_map_system.hpp", "tkMapSystem.h")
#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_font.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_entity.hpp"

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
