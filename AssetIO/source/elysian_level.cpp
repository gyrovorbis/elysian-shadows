#include <string.h>
#include "elysian_level.hpp"
#ifdef ELYSIAN_ENGINE
#	include "elysian_lua_manager.hpp"
#endif

using namespace std;

extern "C" {
	/* via: http://stackoverflow.com/a/2069241 */
	static char *sgets( char * str, int num, char **input )
	{
		char *next = *input;
		int  numread = 0;

		while ( numread + 1 < num && *next ) {
			int isnewline = ( *next == '\n' );
			*str++ = *next++;
			numread++;
			/* newline terminates the line but is included */
			if ( isnewline )
				break;
		}

		if ( numread == 0 )
			return NULL;  /* "eof" */

		/* must have hit the null terminator or end of line */
		*str = '\0';  /* null terminate this tring */
		/* set up input for next call */
		*input = next;
		return str;
	}
}

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
		if(!(tileSheet = gyVidTexCreateSheetUniform(buffer,
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
		if(!(normalSheet = gyVidTexCreateSheetUniform(buffer,
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
		if(!(specularSheet = gyVidTexCreateSheetUniform(buffer,
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
		GYFile *fp;
		char *buffer;
		size_t len;

		dbgprintf(LOG_VERBOSE, "Level::LoadTileset(%s)", filename);
		dbgPush();

		if(!gyFileOpen(GY_PATH_ASSETS, filename, "r", &fp)) {
			dbgprintf(LOG_CRITICAL, "FAILURE!\n");
			dbgPop();
			return false;
		}

		gyFileLength(fp, &len);
		buffer = (char *) malloc(len + 1);
		memset(buffer, 0, sizeof buffer);
		gyFileRead(fp, buffer, len, NULL);

		sgets(string, ELYSIAN_LEVEL_STRING_SIZE, &buffer);
		sscanf(buffer, "%hd", &tileAmount);
		dbgprintf(LOG_VERBOSE, "tileAmount - %d", tileAmount);

		for(unsigned int i = 0; i < tileAmount; ++i) {
			int tempInt;
			sscanf(buffer, "%d", &tempInt);
			tileset[i].colliderType = (Tile::COLLIDER_TYPE)tempInt;
			sscanf(buffer, "%d", &tileset[i].animated);
			sscanf(buffer, "%f", &tileset[i].animTime);
			sscanf(buffer, "%hd", &tileset[i].nextIndex);

		}

		free(buffer);
		gyFileClose(&fp);
		dbgPop();
		return true;
	}

	bool Level::saveTileset(const char *const filename) {
		FILE* file = fopen(filename, "w");

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
