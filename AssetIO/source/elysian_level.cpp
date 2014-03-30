#include <string.h>
#include "elysian_level.hpp"
#ifdef ELYSIAN_ENGINE
#	include "elysian_lua_manager.hpp"
#endif

using namespace std;

namespace elysian {
    
	static char buffer[ELYSIAN_LEVEL_PATH_SIZE+50];

	Level::~Level(void) { 
#ifdef ELYSIAN_ENGINE
		gyVidTexDestroy(tileSheet);
		gyVidTexDestroy(normalSheet);
		gyVidTexDestroy(specularSheet);
#endif
	}

	bool Level::load(const char *const dir) {
		bool success = true;
    
		dbgprintf(LOG_VERBOSE, "Level::Load(%s)", dir);
		dbgPush();
		strcpy(path, dir);
    
		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_TILESET_PATH);
		success &= loadTileset(buffer);

#ifdef ELYSIAN_ENGINE
		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_TILESHEET_PATH);
		dbgprintf(LOG_VERBOSE, "Level::LoadTileSheet(%s)", buffer);
		dbgPush();
		if(!(tileSheet = gyVidTexCreateSheetUniform(gyFilePath(buffer),
													   ELYSIAN_TILESHEET_WIDTH,
													   ELYSIAN_TILESHEET_HEIGHT,
													   ELYSIAN_TILE_WIDTH,
													   ELYSIAN_TILE_HEIGHT)))
		{
			success = false;
			dbgprintf(LOG_CRITICAL, "FAILURE!");
		}
		dbgPop();

		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_NORMALSHEET_PATH);
		dbgprintf(LOG_VERBOSE, "Level::LoadNormalSheet(%s)", buffer);
		dbgPush();
		if(!(normalSheet = gyVidTexCreateSheetUniform(gyFilePath(buffer),
													   ELYSIAN_TILESHEET_WIDTH,
													   ELYSIAN_TILESHEET_HEIGHT,
													   ELYSIAN_TILE_WIDTH,
													   ELYSIAN_TILE_HEIGHT)))
		{
			success = false;
			dbgprintf(LOG_CRITICAL, "Fine. Don't bumpmap your shit.");
		}
		dbgPop();


		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_SPECULARSHEET_PATH);
		dbgprintf(LOG_VERBOSE, "Level::LoadSpecularSheet(%s)", buffer);
		dbgPush();
		if(!(specularSheet = gyVidTexCreateSheetUniform(gyFilePath(buffer),
													   ELYSIAN_TILESHEET_WIDTH,
													   ELYSIAN_TILESHEET_HEIGHT,
													   ELYSIAN_TILE_WIDTH,
													   ELYSIAN_TILE_HEIGHT)))
		{
			success = false;
			dbgprintf(LOG_CRITICAL, "No specular sheet? Aight.");
		}
		dbgPop();



#	ifdef ELYSIAN_ENGINE
		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_LEVEL_INIT_LUA_PATH);
		LuaManager::runScriptSandbox(buffer);
#	endif

#endif
		dbgPop();
		return success;
	}

	bool Level::save(const char *const dir) {
		bool success = true;

		dbgprintf(LOG_VERBOSE, "Level::save(%s)", dir);
		dbgPush();
    
		strcpy(buffer, dir);
		strcat(buffer, ELYSIAN_TILESET_PATH);
		success &= saveTileset(buffer);
    
		dbgPop();
		return success;
	}

	bool Level::loadTileset(const char *const filename) {
		FILE* file = fopen(gyFilePath(filename), "r");

		dbgprintf(LOG_VERBOSE, "Level::LoadTileset(%s)", filename);
		dbgPush();
    
		if(!file) {
			dbgprintf(LOG_CRITICAL, "FAILURE!\n");
			dbgPop();
			return false;
		}

		fgets(string, ELYSIAN_LEVEL_STRING_SIZE, file);
		fscanf(file, "%hd", &tileAmount);
		dbgprintf(LOG_VERBOSE, "tileAmount - %d", tileAmount);

		for(unsigned int i = 0; i < tileAmount; ++i) {
			int tempInt;
			fscanf(file, "%d", &tempInt);
			tileset[i].colliderType = (Tile::COLLIDER_TYPE)tempInt;
			fscanf(file, "%d", &tileset[i].animated);
			fscanf(file, "%f", &tileset[i].animTime);
			fscanf(file, "%hd", &tileset[i].nextIndex);

		}
	
		fclose(file);
		dbgPop();
		return true;
	}

	bool Level::saveTileset(const char *const filename) {
		FILE* file = fopen(gyFilePath(filename), "w");

		dbgprintf(LOG_VERBOSE, "Level::saveTileset(%s)", filename);
		dbgPush();
    
		if(!file) {
			dbgprintf(LOG_VERBOSE, "FAILURE!");
			dbgPop();
			return false;
		}

		fprintf(file, "%s\n", string);
		fprintf(file, "%d\n", tileAmount);
		dbgprintf(LOG_VERBOSE, "tileAmount - %d", tileAmount);

		for(unsigned int i = 0; i < tileAmount; ++i) {
			fprintf(file, "%d\n", tileset[i].colliderType);
			fprintf(file, "%d\n", tileset[i].animated);
			fprintf(file, "%f\n", tileset[i].animTime);
			fprintf(file, "%d\n", tileset[i].nextIndex);
		}

		fclose(file);
		dbgPop();
		return true;
	}

}
