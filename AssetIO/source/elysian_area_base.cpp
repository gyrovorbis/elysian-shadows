#include <string.h>
#include "elysian_asset_typedefs.hpp"
#include "elysian_area.hpp"
#include "elysian_level.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"

using namespace std;

namespace elysian {

Entity* AreaBase::findEntity(const char *name) {
	for(std::vector<Entity*>::iterator it = entities.begin();
		it != entities.end(); ++it) {
			if(strcmp(name, (*it)->getName()) == 0) 
				return *it;
	}
	return nullptr;
}

void AreaBase::addEntity(Entity *entity) {
    entities.push_back(entity);
    entity->activate();
}

void AreaBase::removeEntity(Entity *entity) {
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
/*
AreaBase::AreaBase(void) {
#ifdef GYRO_GLAL
#endif
}*/

AreaBase::~AreaBase(void) {
    glDeleteBuffers(1, &vboBuff);
	//DELETE COMPONENT/ENTITY SHIT@!!!!
    dbgprintf(LOG_VERBOSE, "Area::~Area(%s) - FIX FOR WARPS LEAKING MEMORY!! -- Entity Data?", dir);
	freeMap();
}

void AreaBase::allocMap(void) {
	tiles = (TileEntry *)malloc(sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
	tileLayer2 = tiles + width*height;
	tileLayer3 = tiles + width*height*2;
	mapEntryCount = width*height*ELYSIAN_AREA_MAP_LAYERS;
	layerEntryCount = width*height;
}

void AreaBase::freeMap(void) {
	free(tiles);
}

void AreaBase::resizeMap(const unsigned w, const unsigned h) {
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

    tiles           = ntiles;
    tileLayer2      = tiles + w*h;
    tileLayer3      = tiles + w*h*2;
    width           = w;
    height          = h;
    mapEntryCount   = w*h*ELYSIAN_AREA_MAP_LAYERS;
	layerEntryCount = w*h;

    //gotta repopulate map VBO
    if(activated) {
        glDeleteBuffers(1, &vboBuff);
        vboPopulated = false;
        populateVbo();
    }

}

static char buffer[ELYSIAN_AREA_PATH_SIZE+50];

bool AreaBase::load(const char *const dir) {
    assert(level);

    bool success = true;

    dbgprintf(LOG_VERBOSE, "Area::Load(%s)", dir);
    dbgPush();
    
	strcpy(path, dir);
    strcpy(buffer, dir);

    char* token = strtok(buffer, "/");
    char* prevToken = token;
    while(token) {
        prevToken = token;
        token = strtok(nullptr, "/");
    }
    strcpy(this->dir, prevToken);
    
	strcpy(buffer, dir);
    strcat(buffer, ELYSIAN_AREA_MAP_PATH);
	success &= loadMap(buffer);

	strcpy(buffer, dir);
	strcat(buffer, ELYSIAN_AREA_ENTITY_PATH);
	success &= loadEntities(buffer);

    dbgPop();
	return success;
}

bool AreaBase::save(const char *const dir) {
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

bool AreaBase::loadMap(const char *const filename) {
	GYFile *fp;
	dbgprintf(LOG_VERBOSE, "Area::LoadMap(%s)", filename);
    dbgPush();

	if (!gyFileOpen(filename, "rb", &fp)) {
		dbgprintf(LOG_CRITICAL, "FAILURE!");
		dbgPop();
		return false;
	}

    gyFileRead(fp, string, sizeof string, nullptr);
    gyFileRead(fp, string, sizeof string, nullptr);
    gyFileRead(fp, &width, sizeof width, nullptr);
    gyFileRead(fp, &height, sizeof height, nullptr);

	allocMap();

    gyFileRead(fp, tiles, sizeof(TileEntry) * width * height * ELYSIAN_AREA_MAP_LAYERS, nullptr);

	gyFileClose(&fp);

    //populateVbo();
    dbgPop();
	return true;
}

bool AreaBase::saveMap(const char *const filename) {
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

    fwrite(dir, sizeof(dir), 1, file);
	fwrite(string, sizeof(string), 1, file);
	fwrite(&width, sizeof(width), 1, file);
	fwrite(&height, sizeof(height), 1, file);

	fwrite(tiles, sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS, 1, file);

	fclose(file);
    dbgPop();
	return true;
}

bool AreaBase::loadEntities(const char *const /*filename*/) {
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

bool AreaBase::saveEntities(const char *const /*filename*/) {
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

void AreaBase::populateVbo(void) {
    if(vboPopulated) return;

 //   glGenVertexArrays(1, &vao);
  //  glBindVertexArray(vao);

    glGenBuffers(1, &vboBuff);

	VboEntry* data = (VboEntry*)malloc(sizeof(VboEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS*4);
	VboEntry* curPtr = data;
//	const GYTexCoord2* coords;

	for(unsigned z = 0; z < ELYSIAN_AREA_MAP_LAYERS; ++z) {
		for(unsigned y = 0; y < height; ++y) {
			for(unsigned x = 0; x < width; ++x) {
				TileEntry tileEntry = getTileEntry(z, x, y);

                vboEntryFromTileEntry(curPtr, &tileEntry, x, y);

				curPtr += 4;

			}
		}
	}

    glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VboEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS*4, data, GL_STATIC_DRAW);


/*
    glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_POS]);
    glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD]);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR]);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL]);
    //glEnableVertexAttribArray(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT]);

        glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
        glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(VboEntry), 0);
            glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
            glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(VboEntry), (void*)(4*sizeof(float)));


    glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
        glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VboEntry), (void*)(6*sizeof(float)));

        glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(VboEntry), (void*)(7*sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
        glVertexAttribPointer(glalCurLightShader->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(VboEntry), (void*)(10*sizeof(float)));

*/
    glGenBuffers(1, &elementVbo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE*sizeof(GLushort), &elementsArray[0], GL_DYNAMIC_DRAW);
   // glBindVertexArray(0);

    vboPopulated = true;
}

void AreaBase::activate(void) {
    if(activated) return;


#if 1
#ifdef ELYSIAN_AREA_USE_INIT_LUA
    strcpy(buffer, path); //dir);
    strcat(buffer, ELYSIAN_AREA_INIT_LUA_PATH);
    LuaManager::runScriptSandbox(buffer);
#endif
#endif

    populateVbo();

    activated = true;
}


}
