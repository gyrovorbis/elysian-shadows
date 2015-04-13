#include <string.h>
#include <gyro_file_api.h>
#include <elysian_level.hpp>
#include <elysian_lua_manager.hpp>

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

    LevelBase::~LevelBase(void) {
        gyVidTexDestroy(tileSheet);
        gyVidTexDestroy(normalSheet);
        gyVidTexDestroy(specularSheet);
    }

    bool LevelBase::load(const char *const dir) {
        bool success = true;

        dbgprintf(LOG_VERBOSE, "Level::Load(%s)", dir);
        dbgPush();
        strcpy(path, dir);

        strcpy(buffer, dir);
        strcat(buffer, ELYSIAN_TILESET_PATH);
        success &= loadTileset(buffer);

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
            //success = false;
            dbgprintf(LOG_VERBOSE, "Fine. Don't bumpmap your shit.");
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
            //success = false;
            dbgprintf(LOG_VERBOSE, "No specular sheet? Aight.");
        }
        dbgPop();

#ifdef ELYSIAN_ENGINE
        strcpy(buffer, dir);
        strcat(buffer, ELYSIAN_LEVEL_INIT_LUA_PATH);
        LuaManager::runScriptSandbox(buffer);
#endif

        dbgPop();
        return success;
    }

    bool LevelBase::save(const char *const dir) {
        bool success = true;

        dbgprintf(LOG_VERBOSE, "Level::save(%s)", dir);
        dbgPush();

        strcpy(buffer, dir);
        strcat(buffer, ELYSIAN_TILESET_PATH);
        success &= saveTileset(buffer);

        dbgPop();
        return success;
    }

    bool LevelBase::loadTileset(const char *const filename) {
        GYFile *fp;
        char *buffer, *ptr;
        int ptr_inc = 0;
        size_t len;

        dbgprintf(LOG_VERBOSE, "Level::LoadTileset(%s)", filename);
        dbgPush();

        if(!gyFileOpen(filename, "r", &fp)) {
            dbgprintf(LOG_CRITICAL, "FAILURE!\n");
            dbgPop();
            return false;
        }

        gyFileLength(fp, &len);
        buffer = ptr = (char *) malloc(len + 1);
        memset(buffer, 0, sizeof(char) * (len + 1));
        gyFileRead(fp, buffer, len, NULL);

        sgets(string, ELYSIAN_LEVEL_STRING_SIZE, &ptr);
        sscanf(ptr, "%hd%n", &tileAmount, &ptr_inc);
        ptr += ptr_inc;
        dbgprintf(LOG_VERBOSE, "tileAmount - %d", tileAmount);

        for(unsigned int i = 0; i < tileAmount; ++i) {
            int tempInt;
            sscanf(ptr, "%d%n", &tempInt, &ptr_inc);
            ptr += ptr_inc;
            tileset[i].colliderType = (Tile::COLLIDER_TYPE)tempInt;
            sscanf(ptr, "%d%n", (int*)&tileset[i].animated, &ptr_inc);
            ptr += ptr_inc;
            sscanf(ptr, "%f%n", &tileset[i].animTime, &ptr_inc);
            ptr += ptr_inc;
            sscanf(ptr, "%hd%n", &tileset[i].nextIndex, &ptr_inc);
            ptr += ptr_inc;
            sscanf(ptr, "%d%n", (int*)&tileset[i].standOn, &ptr_inc);
            ptr += ptr_inc;
            sscanf(ptr, "%d%n", (int*)&tileset[i].transparency, &ptr_inc);
            ptr += ptr_inc;
        }

        free(buffer);
        gyFileClose(&fp);
        dbgPop();
        return true;
    }

    bool LevelBase::saveTileset(const char *const filename) {
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
            fprintf(file, "%d\n", (int)tileset[i].standOn);
            fprintf(file, "%d\n", (int)tileset[i].transparency);
        }

        fclose(file);
        dbgPop();
        return true;
    }

}
