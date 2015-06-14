#include <string.h>
#include "elysian_area.hpp"
#include "elysian_level.hpp"
#include "elysian_lua_manager.hpp"
#include <elysian_lua_wrappers.hpp>
#include "elysian_entity.hpp"
#include <glal/gyro_gl.h>
#include <glal/gyro_shaders_glal.h>
#include <elysian_generic.hpp>

using namespace std;

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AreaBase)

Entity* AreaBase::findEntity(const char *name) {
    for(auto it : entities) {
        if(strcmp(name, it->getName()) == 0)
            return it;
    }

    for(auto it : inactiveEntities) {
        if(strcmp(name, it->getName()) == 0)
            return it;
    }

    return nullptr;
}

void AreaBase::addEntity(Entity *entity) {
    for(auto& it: entities) {
        if(strcmp(it->getName(),  entity->getName()) == 0) return;
    }

    entity->setArea(static_cast<Area*>(this));
    entity->activate();
}

void AreaBase::addInactiveEntity(Entity *entity) {
    for(auto& it: inactiveEntities) {
        if(strcmp(it->getName(),  entity->getName()) == 0) return;
    }

    entity->setArea(static_cast<Area*>(this));
    inactiveEntities.push_back(entity);
}

void AreaBase::removeEntity(Entity *entity) {
    entity->deactivate();
    entity->setArea(nullptr);
    containerEraseOnMatch(entity, inactiveEntities);
}

/*
Entity*     getInactiveEntityByIndex(const unsigned index) const;
void        setInactiveEntityByIndex(const unsigned index, OOLUA::Lua_table_ref valueTable);
*/
/*
AreaBase::AreaBase(void) {
#ifdef GYRO_GLAL
#endif
}*/

AreaBase::~AreaBase(void) {
    glDeleteBuffers(1, &vertexVbo);
    //DELETE COMPONENT/ENTITY SHIT@!!!!
    dbgprintf("Area::~Area(%s) - FIX FOR WARPS LEAKING MEMORY!! -- Entity Data?", dir);
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
        glDeleteBuffers(1, &vertexVbo);
        vboPopulated = false;
        populateVbo();
    }

}

void AreaBase::vboEntryFromTileEntry(GYVertex * const vboEntry, const TileEntry * const tileEntry, const unsigned x, const unsigned y) const {
//const float zDelta = 1.1f;
    const GY_RECT_PLANE_TYPE planeType = tileEntry->getPlane();
    const GY_RECT_TEX_ORIENT texOrient = tileEntry->getOrient();
    //const Tile* tile = &level->tileset[tileEntry->getIndex()];

    const GYTexCoord2*const coords = gyVidTexCoordArrayFromFrame(&level->tileSheet->_coord->texCoord[tileEntry->getIndex()],
                                                                       texOrient);
    const GYVector4*const localVerts = glalRectVerts[planeType];
    GYVector4 worldVerts[4];

    const GYVector3*const tangent = &glalRectTangents[texOrient];

    const GYVector3*const normal = glalPlaneNormalVector(level->normalSheet? 1 : 0, planeType, texOrient);

    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslateScale(x*32.0f, y*32.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 32.0f, 32.0f, 32.0f);
    gyMatMultVec(localVerts, worldVerts, 4);
    gyMatPop();

    for(unsigned i = 0; i < 4; ++i) {
        vboEntry[i] = {
            worldVerts[i],
            coords[i],
            GYColor4 { 255, 255, 255, 255 },
            *normal,
            *tangent
        };
    }
}



static char buffer[ELYSIAN_AREA_PATH_SIZE+50];

bool AreaBase::load(const char *const dir) {
    assert(level);

    bool success = true;
    OOLUA::Lua_table_ref ref;

    dbgprintf("Area::Load(%s)", dir);
    dbgpush();

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
    strcat(buffer, ELYSIAN_AREA_TABLE_PATH);

    dbgprintf("Deserializing area_table.lua");
    dbgpush();
    if(runScriptSandbox(buffer, nullptr, 1)) {
        ref.pull(*_mainState);
        if(ref.valid()) {
            fromTable(ref);
        } else {
            dbgprintf("Invalid area table reference!");
        }
    }
    dbgpop();


    dbgpop();
    return success;
}

bool AreaBase::save(const char *const dir) {
    bool success    = true;
    //bool serialized;

    dbgprintf("Area::Save(%s)", dir);
    dbgpush();

    //Save/Save-As, maintain new directory
    strcpy(path, dir);

    strcpy(buffer, dir);
    strcat(buffer, ELYSIAN_AREA_MAP_PATH);
    success &= saveMap(buffer);

    strcpy(buffer, dir);
    strcat(buffer, ELYSIAN_AREA_TABLE_PATH);

   // OOLUA::Lua
    const char* serialData = _callGlobalFunction<const char*>("serialize", nullptr, RawTableReference(getLuaTableRef()));
    //dbgprintf();
    FILE* file = fopen(buffer, "w");
    fprintf(file, "%s", serialData);
    fclose(file);

    //success &= saveEntities(buffer);

    dbgpop();
    return success;
}

bool AreaBase::loadMap(const char *const filename) {
    GYFile *fp;
    dbgprintf("Area::LoadMap(%s)", filename);
    dbgpush();

    if (!gyFileOpen(filename, "rb", &fp)) {
        Debug::printf(Debug::DBG_ERROR, "FAILURE!");
        dbgpop();
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
    dbgpop();
    return true;
}

bool AreaBase::saveMap(const char *const filename) {
    dbgprintf("Area::SaveMap(%s)", filename);
    dbgpush();

    /*
     * This method should only ever be called from
     * the toolkit, so all I've done is remove the call
     * to gyFilePath. It should be safe to leave in
     * standard file i/o ops. - Eddie
     */
    FILE *file = fopen(filename, "wb");
    if(!file) {
        Debug::printf(Debug::DBG_WARNING, "FAILURE!");
        dbgpop();
        return false;
    }

    fwrite(dir, sizeof(dir), 1, file);
    fwrite(string, sizeof(string), 1, file);
    fwrite(&width, sizeof(width), 1, file);
    fwrite(&height, sizeof(height), 1, file);

    fwrite(tiles, sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS, 1, file);

    fclose(file);
    dbgpop();
    return true;
}

bool AreaBase::loadEntities(const char *const /*filename*/) {
#if 0
    ifstream file(gyFilePath(filename));
    Entity *currentEntity;
    unsigned int amount;

    dbgprintf(LOG_DBG_VERBOSE, "Area::LoadEntities(%s)", filename);
    dbgPush();

    if(!file.is_open())	{
        dbgprintf(LOG_DBG_WARNING, "FAILURE - could not open file!");
        dbgPop();
        return false;
    }

    file >> amount;
    dbgprintf(LOG_DBG_VERBOSE, "%d entities", amount);
    for(unsigned int i = 0; i < amount; ++i) {
        if(file.eof()) {
            dbgprintf(LOG_DBG_WARNING, "FAILURE! - Unexpected end of file");
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
    dbgprintf(LOG_DBG_VERBOSE, "Area::SaveEntities(%s)", filename);
    dbgPush();

    if(!file.is_open()) {
        dbgprintf(LOG_DBG_WARNING, "FAILURE! - Could not open file.");
        dbgPop();
        return false;
    }

    file << entities.size() << endl;
    for(unsigned int i = 0; i < entities.size(); ++i) {
        dbgprintf(LOG_DBG_VERBOSE, "Writing entity #%d", i);
        entities[i]->save(file);
    }

    dbgPop();
#endif
    return true;
}

void AreaBase::populateVbo(void) {
    if(vboPopulated) return;

    gyVidVertexArrayBind(0);

    glGenBuffers(1, &vertexVbo);

    GYVertex* data = (GYVertex*)malloc(sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4);
    GYVertex* curPtr = data;
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

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    free(data);

    _LightingShaderProgram* shaders[2] = {
        &glalLightingShader,
        &glalShadowMapShader
    };

    for(unsigned j = 0; j < 2; ++j) {
        glGenVertexArrays(1, &vao[j]);
        gyVidVertexArrayBind(vao[j]);

        glGenBuffers(1, &elementVbo[j]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVbo[j]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, elementsArray, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);

    glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_POS]);
    glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_TEX_COORD]);

    glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), 0);
    glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));


    if(j == 0) { //light VAO
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_COLOR]);
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_NORMAL]);
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_TANGENT]);

        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)(6*sizeof(float)));
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(7*sizeof(float)));
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(10*sizeof(float)));

    }
        }

   // glGenBuffers(1, &elementVbo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVbo);
 //   glBufferData(GL_ELEMENT_ARRAY_BUFFER, ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE*sizeof(GLushort), &elementsArray[0], GL_DYNAMIC_DRAW);

    gyVidVertexArrayBind(0);
    vboPopulated = true;
}

void AreaBase::activate(void) {
    if(activated) return;
    if(!level || !level->tileSheet) {
        Debug::printf(Debug::DBG_ERROR, "Parent level has no tilesheet. Area couldn't be activated!");
        return;
    }


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


void AreaBase::setVboEntry(const TileEntry*const tileEntry, const unsigned l, const unsigned x, const unsigned y) const {
    GYVertex entry[4];

    vboEntryFromTileEntry(entry, tileEntry, x, y);

    //ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x) = e;

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferSubData(GL_ARRAY_BUFFER, ELYSIAN_MAP_ENTRY_TO_VBO_INDEX(l, x, y)*sizeof(GYVertex), sizeof(GYVertex)*4, (void*)entry);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void AreaBase::setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e) {
    GYVertex entry[4];

    vboEntryFromTileEntry(entry, &e, x, y);

    ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x) = e;

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferSubData(GL_ARRAY_BUFFER, ELYSIAN_MAP_ENTRY_TO_VBO_INDEX(l, x, y)*sizeof(GYVertex), sizeof(GYVertex)*4, (void*)entry);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Entity* AreaBase::getActiveEntityByIndex(const unsigned index) const {
    return entities[index];
}


void AreaBase::setActiveEntityByIndex(const unsigned index, Entity*const entity) {
    if(index == entities.size()) { //allocate new entity
        //Entity* entity = Entity::create(valueTable);
        static_cast<Area*>(this)->addEntity(entity);
    } else if(index < entities.size()) {                        //initialize existing entity
        assert(false);
        //entities[index]->fromTable(valueTable);
    }
}

Entity* AreaBase::getInactiveEntityByIndex(const unsigned index) const {
    return inactiveEntities[index];
}

void AreaBase::setInactiveEntityByIndex(const unsigned index, Entity*const entity) {
    if(index == inactiveEntities.size()) { //allocate new entity
        //Entity* entity = Entity::create(valueTable);
        static_cast<Area*>(this)->addInactiveEntity(entity);
    } else if(index < inactiveEntities.size()) {                        //initialize existing entity
        assert(false);
        //inactiveEntities[index]->fromTable(valueTable);
    }
}

}
