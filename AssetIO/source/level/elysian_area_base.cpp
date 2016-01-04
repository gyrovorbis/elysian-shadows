#include <string.h>
#include <proxy/elysian_area_proxy.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <crtp/elysian_level.hpp>
#include <crtp/elysian_lua_manager.hpp>
#include <utility/elysian_generic.hpp>
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
#include <glal/gyro_gl.h>
#include <glal/gyro_shaders_glal.h>
#include <glal/gyro_video_glal.h>
#endif
#include <gyro_file_api.h>
#include <system/elysian_audio_system.hpp>


using namespace std;

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AreaBase)

unsigned AreaBase::getAuxOggTrackCount(void) const { return 2; }
unsigned AreaBase::getBgmOggTrackCount(void) const { return 3; }

AreaBase::AreaBase(void) {
    for(unsigned i = 0; i < ELYSIAN_AUDIO_BGM_TRACKS; ++i) {
        strcpy(_bgmOggTracks[i], "none");
    }

    for(unsigned i = 0; i < ELYSIAN_AUDIO_AUX_TRACKS; ++i) {
        strcpy(_auxOggTracks[i], "none");
    }
}


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

void AreaBase::setMapWidth(const unsigned short /*w*/) {

}

void AreaBase::setMapHeight(const unsigned short /*h*/) {

}

void AreaBase::validateEntityName(Entity *entity) {
    auto tokenize = [](const char*const src, char*const strdest, unsigned long& numdest) {
        const auto len = strlen(src);
        int numPos = (int)len-1;
        for(; numPos >= 0; --numPos) {
            if(!isdigit(src[numPos])) break;
        }
        if(numPos < 0) numPos = 0;
        //Don't overflow and cause an exception in stoul()
        if(len-numPos-1 >= 9) numPos = len-10; //10 digits max for uint32, use 9 for buffer and simplicity
#if GYRO_PLATFORM != GYRO_PLATFORM_DC && GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
        numdest = (numPos < (int)len-1)? stoul(&src[numPos+1]) : 2ul;
#else
        numdest = (numPos < (int)len-1)? atoi(&src[numPos+1]) : 2ul; //god fucking damnit
#endif
        strncpy(strdest, src, numPos+1);
        strdest[numPos+1] = '\0';
    };

    char strBuff[ELYSIAN_ENTITY_NAME_SIZE];
    char concatBuff[ELYSIAN_ENTITY_NAME_SIZE];
    unsigned long num;

    if(findEntity(entity->getName())) {

        tokenize(entity->getName(), strBuff, num);

        sprintf(concatBuff, "%s%lu", strBuff, num);
        while(findEntity(concatBuff)) {
            sprintf(concatBuff, "%s%lu", strBuff, ++num);
        }

        entity->setName(concatBuff);
    }
}

void AreaBase::addEntity(Entity *entity) {
    validateEntityName(entity);
    entity->setArea(static_cast<Area*>(this));
    entity->activate();
}

void AreaBase::addInactiveEntity(Entity *entity) {
    validateEntityName(entity);
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
    //DELETE COMPONENT/ENTITY SHIT@!!!!
    dbgprintf("Area::~Area(%s) - FIX FOR WARPS LEAKING MEMORY!! -- Entity Data?", dir);
    freeMap();
    freeVbo();
}

void AreaBase::allocMap(void) {
    tiles = (TileEntry *)malloc(sizeof(TileEntry)*width*height*ELYSIAN_AREA_MAP_LAYERS);
    tileLayer2 = tiles + width*height;
    tileLayer3 = tiles + width*height*2;
    tileEntryCount = width*height*ELYSIAN_AREA_MAP_LAYERS;
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
    tileEntryCount   = w*h*ELYSIAN_AREA_MAP_LAYERS;
    layerEntryCount = w*h;

    //gotta repopulate map VBO
    if(activated) {
        freeVbo();
        populateVbo();
    }

}

void AreaBase::vboEntryFromTileEntry(GYVertex * const vboEntry, const TileEntry * const tileEntry, const unsigned x, const unsigned y) const {
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
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
#endif
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
        Debug::printf(Debug::LOG_ERROR, "FAILURE!");
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
        Debug::printf(Debug::LOG_WARNING, "FAILURE!");
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

    dbgprintf(LOG_LOG_VERBOSE, "Area::LoadEntities(%s)", filename);
    dbgPush();

    if(!file.is_open())	{
        dbgprintf(LOG_LOG_WARNING, "FAILURE - could not open file!");
        dbgPop();
        return false;
    }

    file >> amount;
    dbgprintf(LOG_LOG_VERBOSE, "%d entities", amount);
    for(unsigned int i = 0; i < amount; ++i) {
        if(file.eof()) {
            dbgprintf(LOG_LOG_WARNING, "FAILURE! - Unexpected end of file");
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
    dbgprintf(LOG_LOG_VERBOSE, "Area::SaveEntities(%s)", filename);
    dbgPush();

    if(!file.is_open()) {
        dbgprintf(LOG_LOG_WARNING, "FAILURE! - Could not open file.");
        dbgPop();
        return false;
    }

    file << entities.size() << endl;
    for(unsigned int i = 0; i < entities.size(); ++i) {
        dbgprintf(LOG_LOG_VERBOSE, "Writing entity #%d", i);
        entities[i]->save(file);
    }

    dbgPop();
#endif
    return true;
}

void AreaBase::freeVbo(void) {
#if GYRO_PLATFORM != GYRO_PLATFORM_DC && GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
    glDeleteBuffers(1, &vertexVbo);
    glDeleteBuffers(1, &elementVbo[0]);
    glDeleteBuffers(1, &elementVbo[1]);

    glDeleteVertexArrays(1, &vao[0]);
    glDeleteVertexArrays(1, &vao[1]);

    for(unsigned i = 0; i < 2; ++i) {
        elementsArraySize[i] = 0;
        lastElemArraySize[i] = 0;
        lastOffset[i] = 0;
        minIndex[i] = INT_MAX;
        maxIndex[i] = 0;
    }
    vboPopulated = false;
#endif
}

void AreaBase::populateVbo(void) {
    if(vboPopulated) return;
#if GYRO_PLATFORM != GYRO_PLATFORM_DC && GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4, data, ELYSIAN_AREA_ARRAY_VBO_TYPE);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, elementsArray, ELYSIAN_AREA_ELEMENT_ARRAY_VBO_TYPE);

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
#endif
    vboPopulated = true;
}

void AreaBase::activate(void) {
    if(activated) return;
    if(!level || !level->tileSheet) {
        Debug::printf(Debug::LOG_ERROR, "Parent level has no tilesheet. Area couldn't be activated!");
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

void AreaBase::setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e) {
    ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x) = e;

#ifdef GYRO_GLAL
    GYVertex entry[4];
    if(e.getValue() == 0) return;
    vboEntryFromTileEntry(entry, &e, x, y);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferSubData(GL_ARRAY_BUFFER, ELYSIAN_TILE_ENTRY_TO_VBO_INDEX(l, x, y)*sizeof(GYVertex), sizeof(GYVertex)*4, &entry);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

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
