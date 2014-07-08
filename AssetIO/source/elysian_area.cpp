#include <string.h>
#include "elysian_area.hpp"
#include "elysian_level.hpp"
#include "elysian_entity.hpp"
#ifdef ELYSIAN_ENGINE
#include "elysian_lua_manager.hpp"
#endif

using namespace std;

namespace elysian {

Entity* Area::findEntity(const char *name) {
	for(std::vector<Entity*>::iterator it = entities.begin();
		it != entities.end(); ++it) {
			if(strcmp(name, (*it)->getName()) == 0) 
				return *it;
	}
	return nullptr;
}

void Area::addEntity(Entity *entity) {
	//entities.push_back(entity);
	entity->activate();
}

void Area::removeEntity(Entity *entity) {
	entity->deactivate();
	for(std::vector<Entity*>::iterator it = entities.begin();
		it != entities.end(); ++it)
	{
		if(*it == entity) {
			entities.erase(it);
			return;
		}
	}
}

Area::Area(void) {
#ifdef GYRO_GLAL
#endif
}

Area::~Area(void) {
#ifdef GYRO_GLAL
	glDeleteBuffers(1, &vboBuff);
#endif
	//DELETE COMPONENT/ENTITY SHIT@!!!!
	dbgprintf(LOG_VERBOSE, "Area::~Area(%s) - FIX FOR WARPS LEAKING MEMORY!! -- Entity Data?", name);
	freeMap();
}

void Area::allocMap(void) {
	tiles = (TileEntry *)malloc(sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
	tileLayer2 = tiles + width*height;
	tileLayer3 = tiles + width*height*2;
	mapEntryCount = width*height*ELYSIAN_AREA_MAP_LAYERS;
	layerEntryCount = width*height;
}

void Area::freeMap(void) {
	free(tiles);
}

void Area::resizeMap(const unsigned w, const unsigned h) {
	TileEntry *ntiles = (TileEntry *)malloc(sizeof(TileEntry)*w*h*ELYSIAN_AREA_MAP_LAYERS);

	memset(ntiles, 0, sizeof(TileEntry)*w*h*ELYSIAN_AREA_MAP_LAYERS);

	unsigned endX = (w < width)? w : width;
	unsigned endY = (h < height)? h : height;

	for(unsigned y = 0; y < endY; ++y)
		for(unsigned x = 0; x < endX; ++x) {
			for(unsigned i = 0; i < ELYSIAN_AREA_MAP_LAYERS; ++i)
				ntiles[i*w*h+y*w+x] = getTileEntry(i, x, y);
		}

	freeMap();

	tiles = ntiles;
	tileLayer2 = tiles + w*h;
	tileLayer3 = tiles + w*h*2;
	width = w;
	height = h;
	mapEntryCount = w*h*ELYSIAN_AREA_MAP_LAYERS;
	layerEntryCount = w*h;

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
	LuaManager::runScriptSandbox(buffer);
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
	GYFile *fp;
	dbgprintf(LOG_VERBOSE, "Area::LoadMap(%s)", filename);
    dbgPush();

	if (!gyFileOpen(filename, "rb", &fp)) {
		dbgprintf(LOG_CRITICAL, "FAILURE!");
		dbgPop();
		return false;
	}

	gyFileRead(fp, name, sizeof name, NULL);
	gyFileRead(fp, string, sizeof string, NULL);
	gyFileRead(fp, &width, sizeof width, NULL);
	gyFileRead(fp, &height, sizeof height, NULL);

	allocMap();

	gyFileRead(fp, tiles, sizeof(TileEntry) * width * height * ELYSIAN_AREA_MAP_LAYERS, NULL);

	gyFileClose(&fp);

	populateVbo();
    dbgPop();
	return true;
}

bool Area::saveMap(const char *const filename) {
	dbgprintf(LOG_VERBOSE, "Area::SaveMap(%s)", filename);
	dbgPush();

	/*
	 * This method should only ever be called from
	 * the toolkit, so all I've done is remove the call
	 * to gyFilePath. It should be safe to leave in
	 * standard file i/o ops. - Eddie
	 */
	FILE *file = fopen(filename, "wb");
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

	fclose(file);
    dbgPop();
	return true;
}

bool Area::loadEntities(const char *const /*filename*/) {
#if 0
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
#endif
	return true;
}

bool Area::saveEntities(const char *const /*filename*/) {
#if 0
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
#endif
	return true;
}

#ifdef GYRO_GLAL
void Area::populateVbo(void) {

	glGenBuffers(1, &vboBuff);

	VboEntry* data = (VboEntry*)malloc(sizeof(VboEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS*4);
	VboEntry* curPtr = data;
	const GYTexCoord2* coords;

	for(unsigned z = 0; z < ELYSIAN_AREA_MAP_LAYERS; ++z) {
		for(unsigned y = 0; y < height; ++y) {
			for(unsigned x = 0; x < width; ++x) {
				TileEntry tileEntry = getTileEntry(z, x, y);

				coords = gyVidTexCoordArrayFromFrame(&level->tileSheet->_coord->texCoord[tileEntry.getIndex()],
																				   tileEntry.getOrient());

				curPtr[0].vert = GYVector4 { x*32.0f-16.0f, y*32.0f-((tileEntry.getNormal())? -16.0f : 16.0f)+tileEntry.getElev()*32.0f, tileEntry.getElev()*32.0f + ((tileEntry.getNormal())? 32.0f : 0.0f), 1.0f };
				curPtr[0].color = GYColor4 { 255, 255, 255, 255 };
				curPtr[0].texCoord = coords[0];

				curPtr[1].vert = GYVector4 { x*32.0f+16.0f, y*32.0f-((tileEntry.getNormal())? -16.0f : 16.0f)+tileEntry.getElev()*32.0f, tileEntry.getElev()*32.0f + ((tileEntry.getNormal())? 32.0f : 0.0f), 1.0f };
				curPtr[1].color = GYColor4 { 255, 255, 255, 255 };
				curPtr[1].texCoord = coords[1];

				curPtr[2].vert = GYVector4 { x*32.0f-16.0f, y*32.0f+((tileEntry.getNormal())?16.0f : 16.0f) +tileEntry.getElev()*32.0f, tileEntry.getElev()*32.0f, 1.0f };
				curPtr[2].color = GYColor4 { 255, 255, 255, 255 };
				curPtr[2].texCoord = coords[2];

				curPtr[3].vert = GYVector4 { x*32.0f+16.0f, y*32.0f+((tileEntry.getNormal())? 16.0f : 16.0f)+tileEntry.getElev()*32.0f, tileEntry.getElev()*32.0f, 1.0f };
				curPtr[3].color = GYColor4 { 255, 255, 255, 255 };
				curPtr[3].texCoord = coords[3];

				curPtr += 4;

			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VboEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS*4, data, GL_STATIC_DRAW);

	free(data);
}

#endif


}
