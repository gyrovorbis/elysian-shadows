/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LEVEL_BASE_HPP
#define ELYSIAN_LEVEL_BASE_HPP

#include <vector>
#include <libGyro/gyro_texture_api.h>
#include <AssetIO/level/elysian_tile.hpp>
#include <AssetIO/utility/elysian_asset_typedefs.hpp>


#define ELYSIAN_LEVEL_NAME_SIZE				50
#define ELYSIAN_LEVEL_STRING_SIZE			50
#define ELYSIAN_LEVEL_PATH_SIZE				200
#define ELYSIAN_TILESET_PATH				"/tile/tileset.txt"
#define ELYSIAN_TILESHEET_PATH				"/tile/tilesheet.png"
#define ELYSIAN_NORMALSHEET_PATH			"/tile/tilesheet_n.png"
#define ELYSIAN_SPECULARSHEET_PATH			"/tile/tilesheet_e.png"
#define ELYSIAN_BACKGROUND_TEXTURE_WIDTH                512
#define ELYSIAN_BACKGROUND_TEXTURE_HEIGHT               512
#define ELYSIAN_LEVEL_INIT_LUA_PATH			"/level_init.lua"
#define ELYSIAN_TILESHEET_WIDTH				512
#define ELYSIAN_TILESHEET_HEIGHT			1024
#define ELYSIAN_TILESHEET_TILE_WIDTH                    16
#define ELYSIAN_TILESHEET_TILE_HEIGHT                   32
#define ELYSIAN_TILESHEET_TILES				512

ELYSIAN_ASSET_TYPEDEF(Level)

struct GYTexture;

namespace elysian {

    class LevelBase {
    public:
		Tile			tileset	[ELYSIAN_TILESHEET_TILES];
        char			path	[ELYSIAN_LEVEL_PATH_SIZE];
        char			dir     [ELYSIAN_LEVEL_NAME_SIZE];
        char            string	[ELYSIAN_LEVEL_STRING_SIZE];

        GYTexture*		tileSheet			= nullptr;
        GYTexture*		normalSheet			= nullptr;
        GYTexture*		specularSheet		= nullptr;

        unsigned short	tileAmount          = ELYSIAN_TILESHEET_TILES;

        //lookup tables of attributes
        //palette table(s)
        //Friendlyset/Enemyset
        //Prefab-y data44
        //Light layer palette

        Tile*			getTile(const uint16_t index);
        unsigned short	getTileAmount(void) const;
        const char*		getDirName(void) const;
        const char*		getPath(void) const;

                        ~LevelBase(void);
        bool            load(const char *const dir);
        bool            save(const char *const dir);
        void            activate(void);
        void            deactivate(void);
        bool            loadTileset(const char *const filename);
        bool            saveTileset(const char *const filename);


        GYTexture*      getTileSheet(void) const;
        GYTexture*      getNormalSheet(void) const;
        GYTexture*      getEmissiveSheet(void) const;
    };









    // ==== INLINEZ ====

        inline void LevelBase::activate(void) { }
        inline void LevelBase::deactivate(void) { }

        inline Tile *LevelBase::getTile(const uint16_t index) {
            return &tileset[index];
        }

        inline unsigned short LevelBase::getTileAmount(void) const {
            return tileAmount;
        }

        inline const char *LevelBase::getDirName(void) const {
            return dir;
        }

        inline const char *LevelBase::getPath(void) const {
            return path;
        }

        inline GYTexture* LevelBase::getTileSheet(void) const {
            return tileSheet;
        }

        inline GYTexture* LevelBase::getNormalSheet(void) const {
            return normalSheet;
        }

        inline GYTexture* LevelBase::getEmissiveSheet(void) const {
            return specularSheet;
        }

}

#endif
