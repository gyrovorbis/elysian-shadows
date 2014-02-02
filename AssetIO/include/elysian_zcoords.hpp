#ifndef ELYSIAN_ZCOORDS_HPP
#define ELYSIAN_ZCOORDS_HPP

#include "elysian_area.hpp"

//This needs to be at least as tall as the rendered map area? Not really?
#define ELYSIAN_DEPTH_BUFFER_HEIGHT ELYSIAN_AREA_MAP_HEIGHT

//Background Layers
#define ELYSIAN_ZCOORD_BG_1				1.0f
#define ELYSIAN_ZCOORD_BG_2				2.0f
#define ELYSIAN_ZCOORD_BG_3				3.0f

//Map Layers
#define ELYSIAN_ZCOORD_MAP_BASE			2.0f
#define ELYSIAN_ZCOORD_MAP_SIZE			(ELYSIAN_MAP_ENTRY_ELEV_MAX*32.0f)

//Foreground Layers
#define ELYSIAN_ZCOORD_FG_1				(ELYSIAN_ZCOORD_MAP_BASE + ELYSIAN_ZCOORD_MAP_SIZE + 1.0f)
#define ELYSIAN_ZCOORD_FG_2				(ELYSIAN_ZCOORD_FG_1 + 1.0f)
#define ELYSIAN_ZCOORD_FG_3				(ELYSIAN_ZCOORD_FG_2 + 1.0f)

//Tile Decoration Layers
#define ELYSIAN_ZCOORD_TILE_DECOR		(ELYSIAN_ZCOORD_FG_3 + 1.0f)

//Overlay layers
#define ELYSIAN_ZCOORD_WARP_OVERLAY		(ELYSIAN_ZCOORD_TILE_DECOR + 1.0f)
#define ELYSIAN_ZCOORD_ADVLOG			(ELYSIAN_ZCOORD_WARP_OVERLAY + 1.0f)

namespace elysian {

	//Use these instead of the preprocessor defines, because those are
	//recalculated every use!
	struct ZCoordConstants {
		static const float bg1;
		static const float bg2;
		static const float bg3;
		static const float mapBase;
		static const float mapSize;
		static const float fg1;
		static const float fg2;
		static const float fg3;

		static const float tileDecor;
	
		static const float warpOverlay;
		static const float advLog;

	};

}

#endif
