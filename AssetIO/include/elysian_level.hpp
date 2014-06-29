#ifndef ELYSIAN_LEVEL_HPP
#define ELYSIAN_LEVEL_HPP

#include <vector>
#include <gyro.h>
#include "elysian_asset.hpp"
#include "elysian_tile.hpp"

#define ELYSIAN_LEVEL_NAME_SIZE				50
#define ELYSIAN_LEVEL_STRING_SIZE			50
#define ELYSIAN_LEVEL_PATH_SIZE				200
#define ELYSIAN_TILESET_PATH				"/tile/tileset.txt"
#define ELYSIAN_TILESHEET_PATH				"/tile/tilesheet.png"
#define ELYSIAN_NORMALSHEET_PATH			"/tile/normalsheet.png"
#define ELYSIAN_SPECULARSHEET_PATH			"/tile/specularsheet.png"
#define ELYSIAN_BACKGROUND_TEXTURE_WIDTH	512
#define ELYSIAN_BACKGROUND_TEXTURE_HEIGHT	512
#define ELYSIAN_LEVEL_INIT_LUA_PATH			"/level_init.lua"
#define ELYSIAN_TILESHEET_WIDTH				512
#define ELYSIAN_TILESHEET_HEIGHT			1024
#define ELYSIAN_TILESHEET_TILE_WIDTH		16
#define ELYSIAN_TILESHEET_TILE_HEIGHT		32
#define ELYSIAN_TILESHEET_TILES				512
#define ELYSIAN_TILE_WIDTH					32
#define ELYSIAN_TILE_HEIGHT					32

namespace elysian {

    struct Level: public Asset {
		Tile			tileset	[ELYSIAN_TILESHEET_TILES];
		char			path	[ELYSIAN_LEVEL_PATH_SIZE];
		char			name	[ELYSIAN_LEVEL_NAME_SIZE];
		char            string	[ELYSIAN_LEVEL_STRING_SIZE];
#ifdef ELYSIAN_ENGINE
		GYTexture*		tileSheet			= nullptr;
		GYTexture*		normalSheet			= nullptr;
		GYTexture*		specularSheet		= nullptr;
#endif
		unsigned short	tileAmount	= ELYSIAN_TILESHEET_TILES;
		
        //lookup tables of attributes
        //palette table(s)
        //Friendlyset/Enemyset
		//Prefab-y data
		//Light layer palette

		Tile*			getTile(const uint16_t index);
		unsigned short	getTileAmount(void) const;
		const char*		getName(void) const;
		const char*		getPath(void) const;

				~Level(void);
		bool	load(const char *const dir);
		bool	save(const char *const dir);
		bool	loadTileset(const char *const filename);
		bool	saveTileset(const char *const filename);
    };





	// ==== INLINEZ ====

		inline Tile *Level::getTile(const uint16_t index) { 
			return &tileset[index];
		}

		inline unsigned short Level::getTileAmount(void) const {
			return tileAmount;
		}
		
		inline const char *Level::getName(void) const {
			return name;
		}

		inline const char *Level::getPath(void) const {
			return path;
		}

}

#endif
