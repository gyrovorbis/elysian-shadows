#ifndef ELYSIAN_ZCOORDS_HPP
#define ELYSIAN_ZCOORDS_HPP

#include "elysian_area.hpp"

//This needs to be at least as tall as the rendered map area? Not really?
#define ELYSIAN_DEPTH_BUFFER_HEIGHT ELYSIAN_AREA_MAP_HEIGHT

//MAP LAYERS
#define ELYSIAN_ZCOORD_BG       1.0f

//MAP LAYERS
#define ELYSIAN_ZCOORD_TERRAIN1 2.0f
#define ELYSIAN_ZCOORD_OBJECT1  3.0f
#define ELYSIAN_ZCOORD_TERRAIN2 ELYSIAN_ZCOORD_OBJECT1 + ELYSIAN_DEPTH_BUFFER_HEIGHT + 1.0f
#define ELYSIAN_ZCOORD_OBJECT2  ELYSIAN_ZCOORD_TERRAIN2 + 1.0f
#define ELYSIAN_ZCOORD_FG       ELYSIAN_ZCOORD_OBJECT2 + ELYSIAN_DEPTH_BUFFER_HEIGHT + 1.0f

//OVERLAY LAYERS
#define ELYSIAN_ZCOORD_WARP_OVERLAY ELYSIAN_ZCOORD_FG + 1.0f
#define ELYSIAN_ZCOORD_ADVLOG   ELYSIAN_ZCOORD_WARP_OVERLAY + 1.0f

namespace elysian {

	//Use these instead of the preprocessor defines, because those are
	//recalculated every use!
	struct ZCoordConstants {
		static const float bg;
		static const float terrain1;
		static const float object1;
		static const float terrain2;
		static const float object2;
		static const float fg;
	
		static const float warpOverlay;
		static const float advLog;

	};

}

#endif