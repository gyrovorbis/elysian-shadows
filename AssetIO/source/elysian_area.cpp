#include <fstream>
#include <string.h>
#include "elysian_area.hpp"
#include "elysian_level.hpp"
#include "elysian_entity.hpp"
#ifdef ELYSIAN_ENGINE
#include "elysian_lua_manager.hpp"
#endif

using namespace std;

namespace elysian {

Area::Area(void):
	tiles(NULL),
	tileLayer2(NULL),
	tileLayer3(NULL),
	lights(NULL),
	level(NULL),
	width(0),
	height(0)
{ }

Entity* Area::findEntity(const char *name) {
	for(std::vector<Entity*>::iterator it = entities.begin();
		it != entities.end(); ++it) {
			if(strcmp(name, (*it)->getName()) == 0) 
				return *it;
	}
	return NULL;
}

void Area::addEntity(Entity *entity) {
	entities.push_back(entity);
	entity->activate();
}

void Area::removeEntity(Entity *entity) {
	for(std::vector<Entity*>::iterator it = entities.begin();
		it != entities.end(); ++it)
	{
		if(*it == entity) {
			entities.erase(it);
			return;
		}
	}
}

Area::~Area(void) {
	//DELETE COMPONENT/ENTITY SHIT@!!!!
	dbgprintf(LOG_VERBOSE, "Area::~Area(%s) - FIX FOR WARPS LEAKING MEMORY!! -- Entity Data?", name);
	freeMap();
}

void Area::allocMap(void) {
	tiles = (TileEntry *)malloc(sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
	tileLayer2 = tiles + width*height;
	tileLayer3 = tiles + width*height*2;
	lights = (LightEntry *)malloc(sizeof(LightEntry)*width*height);
}

void Area::freeMap(void) {
	free(tiles);
	free(lights);
}

void Area::resizeMap(const unsigned w, const unsigned h) {
	TileEntry *ntiles = (TileEntry *)malloc(sizeof(TileEntry)*w*h*ELYSIAN_AREA_MAP_LAYERS);
	LightEntry *nlights = (LightEntry *)malloc(sizeof(LightEntry)*w*h);

	memset(ntiles, 0, sizeof(TileEntry)*w*h*ELYSIAN_AREA_MAP_LAYERS);
	memset(nlights, 0, sizeof(LightEntry)*w*h);

	unsigned endX = (w < width)? w : width;
	unsigned endY = (h < height)? h : height;

	for(unsigned y = 0; y < endY; ++y)
		for(unsigned x = 0; x < endX; ++x) {
			for(unsigned i = 0; i < ELYSIAN_AREA_MAP_LAYERS; ++i)
				ntiles[i*w*h+y*w+x] = getTileEntry(i, x, y);
			nlights[y*w+x] = lights[y*w+x];
		}

	freeMap();

	tiles = ntiles;
	lights = nlights;
	tileLayer2 = tiles + w*h;
	tileLayer3 = tiles + w*h*2;
	width = w;
	height = h;

}

static char buffer[ELYSIAN_AREA_PATH_SIZE+50];

bool Area::load(const char *const dir, const Level *lev) {
    bool success = true;

    dbgprintf(LOG_VERBOSE, "Area::Load(%s)", dir);
    dbgPush();
    
    level = lev;
	strcpy(path, dir);
    
	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_MAP_PATH);
	success &= loadMap(buffer);

	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_ENTITY_PATH);
	success &= loadEntities(buffer);

#ifdef ELYSIAN_AREA_USE_INIT_LUA
#	ifdef ELYSIAN_ENGINE
	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_INIT_LUA_PATH);
	LuaManager::loadScript(buffer, false);
#	endif
#endif

    dbgPop();
	return success;
}

bool Area::save(const char *const dir) {
	bool success = true;

	dbgprintf(LOG_VERBOSE, "Area::Save(%s)", dir);
    dbgPush();
    
    //Save/Save-As, maintain new directory
    strcpy(path, dir);
    
	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_MAP_PATH);
	success &= saveMap(buffer);

	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_ENTITY_PATH);
	success &= saveEntities(buffer);

    dbgPop();
	return success;
}

bool Area::loadMap(const char *const filename) {
	dbgprintf(LOG_VERBOSE, "Area::LoadMap(%s)", filename);
    dbgPush();

	FILE *file = fopen(gyFilePath(filename), "rb");
	
	if(!file) {
		dbgprintf(LOG_CRITICAL, "FAILURE!");
        dbgPop();
        return false;
	}

	fread(name, sizeof(name), 1, file);
	fread(string, sizeof(string), 1, file);
	fread(&width, sizeof(width), 1, file);
	fread(&height, sizeof(height), 1, file);

	allocMap();

	fread(tiles, sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS, 1, file);
	fread(lights, sizeof(LightEntry)*width*height, 1, file);

	fclose(file);
    dbgPop();
	return true;
}

bool Area::saveMap(const char *const filename) {
	dbgprintf(LOG_VERBOSE, "Area::SaveMap(%s)", filename);
	dbgPush();

	FILE *file = fopen(gyFilePath(filename), "wb");
	if(!file) {
		dbgprintf(LOG_CRITICAL, "FAILURE!");
        dbgPop();
        return false;
	}

	fwrite(name, sizeof(name), 1, file);
	fwrite(string, sizeof(string), 1, file);
	fwrite(&width, sizeof(width), 1, file);
	fwrite(&height, sizeof(height), 1, file);

	fwrite(tiles, sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS, 1, file);
	fwrite(lights, sizeof(LightEntry)*width*height, 1, file);

	fclose(file);
    dbgPop();
	return true;
}

bool Area::loadEntities(const char *const filename) {
	ifstream file(gyFilePath(filename));
	Entity *currentEntity;
	unsigned int amount;

	dbgprintf(LOG_VERBOSE, "Area::LoadEntities(%s)", filename);
    dbgPush();
    
	if(!file.is_open())	{
        dbgprintf(LOG_CRITICAL, "FAILURE - could not open file!");
        dbgPop();
        return false;
    }
	
	file >> amount;
	dbgprintf(LOG_VERBOSE, "%d entities", amount);
	for(unsigned int i = 0; i < amount; ++i) {
		if(file.eof()) {
			dbgprintf(LOG_CRITICAL, "FAILURE! - Unexpected end of file");
            dbgPop();
			return false;
		}
		currentEntity = new Entity;
		currentEntity->load(file);
	}

    dbgPop();
	return true;
}

bool Area::saveEntities(const char *const filename) {
	ofstream file(gyFilePath(filename));
	dbgprintf(LOG_VERBOSE, "Area::SaveEntities(%s)", filename);
    dbgPush();
    
	if(!file.is_open()) {
		dbgprintf(LOG_CRITICAL, "FAILURE! - Could not open file.");
        dbgPop();
		return false;
	}
	
	file << entities.size() << endl;
	for(unsigned int i = 0; i < entities.size(); ++i) {
		dbgprintf(LOG_VERBOSE, "Writing entity #%d", i);
		entities[i]->save(file);
	}

    dbgPop();
	return true;
}

}
