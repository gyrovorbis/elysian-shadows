#include <string.h>
#include <AssetIO/proxy/elysian_area_proxy.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/crtp/elysian_level.hpp>
#include <ESGamma/crtp/elysian_lua_manager.hpp>
#include <AssetIO/utility/elysian_generic.hpp>
#include <AssetIO/crtp/elysian_behavior.hpp>
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
#include <gl/gyro_gl.h>
#include <gl/gyro_shaders_gl.h>
//#include <glal/gyro_video_glal.h>
#endif
#include <libGyro/gyro_file_api.h>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <stdlib.h>

using namespace std;

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(AreaBase)

unsigned AreaBase::getAuxOggTrackCount(void) const { return 2; }
unsigned AreaBase::getBgmOggTrackCount(void) const { return 3; }

AreaBase::AreaBase(void) {
#if 0
    for(unsigned i = 0; i < ELYSIAN_AUDIO_BGM_TRACKS; ++i) {
        strcpy(_bgmOggTracks[i], "none");
    }

    for(unsigned i = 0; i < ELYSIAN_AUDIO_AUX_TRACKS; ++i) {
        strcpy(_auxOggTracks[i], "none");
    }
#endif
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
    DbgEvent dbg("Deleting Area [%s].", path);
    behaviorsOnUnload();
    //DELETE COMPONENT/ENTITY SHIT@!!!!
    for(auto it: entities) {
        delete it;
    }
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

void AreaBase::vboEntryFromTileEntry(GYVertex * const vboEntry, const TileEntry * const tileEntry, const unsigned x, const unsigned y, const unsigned l) const {
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
//const float zDelta = 1.1f;
    const GY_RECT_PLANE_TYPE planeType = tileEntry->getPlane();
    const GY_RECT_TEX_ORIENT texOrient = tileEntry->getOrient();
    //const Tile* tile = &level->tileset[tileEntry->getIndex()];

    const uint16_t index = tileEntry->getIndex();

    const GYTexCoord2*const coords = gyVidTexCoordArrayFromFrame(&level->tileSheet->_coord->texCoord[index],
                                                                       texOrient);
    const GYVector4*const localVerts = glalRectVerts[planeType];
    GYVector4 worldVerts[4];

    const GYVector3*const tangent = &glalRectTangents[texOrient];

    const GYVector3*const normal = glalPlaneNormalVector(level->normalSheet? 1 : 0, planeType, texOrient);

    gyMatPush();
    gyMatLoadIdentity();
    gyMatTranslateScale(x*32.0f, y*32.0f + tileEntry->getElev()*32.0f+l*ELYSIAN_AREA_LAYER_DEPTH_EPSILON, tileEntry->getElev()*32.0f+l*ELYSIAN_AREA_LAYER_DEPTH_EPSILON, 32.0f, 32.0f, 32.0f);
    gyMatMultVec(localVerts, worldVerts, 4);
    gyMatPop();

    for(unsigned i = 0; i < 4; ++i) {
#if 0
        vboEntry[i] = {
            worldVerts[i],
            coords[i],
            GYColor4 { 1, 2, 3, 4 },
                        66.0f,
            {1.0f, 2.0, 3.0f },//*normal,
            { 11.0f, 12.0f, 13.0f }//*tangent,
            //index != 0? glalOrientNormalMapZFactor(texOrient) : 0.0f //normal factor of 0.0f is secret sentinal to compute that this is a blank tiles
        };
#else

        uint8_t color = index? 255 : 0;
        vboEntry[i] = {
            worldVerts[i],
            coords[i],
            GYColor4 { color, color, color, color },
            index? glalOrientNormalMapZFactor(texOrient) : -2.0f,
            *normal,
            *tangent
        };
#endif

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

    behaviorsOnLoad();

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

    success &= LuaManager::saveTable(LuaVariant::fromWeakRef(getLuaTableRef()), buffer);

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

    GYVertex* data =  //(GYVertex*)std::aligned_alloc(64, sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4);//
    (GYVertex*)malloc(sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4);
    GYVertex* curPtr = data;
//	const GYTexCoord2* coords;

   // for(unsigned z = 0; z < ELYSIAN_AREA_MAP_LAYERS; ++z) {
        for(unsigned y = 0; y < height; ++y) {
            for(unsigned x = 0; x < width; ++x) {
                for(unsigned z = 0; z < ELYSIAN_AREA_MAP_LAYERS; ++z) {
                    TileEntry tileEntry = getTileEntry(z, x, y);

                    vboEntryFromTileEntry(curPtr, &tileEntry, x, y, z);

                    curPtr += 4;
                }

            }
        }
   // }

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*width*height*ELYSIAN_AREA_MAP_LAYERS*4, data, ELYSIAN_AREA_ARRAY_VBO_TYPE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    free(data);

    _LightingShaderProgram* shaders[2] = {
        &glalLightingShader,
        &glalShadowMapShader
    };

#if 0
    typedef struct GYVertex {
        GYVector4   pos;        //16 bytes      16
        GYTexCoord2 texCoord;   //8 bytes       24
        GYColor4    color;      //4 bytes       28
        GYVector3   normal;     //12 bytes      44  32
        float       normalZ;    //4 bytes       45  36
        GYVector3   tangent;    //12 bytes      46  48
    } GYVertex;                 //68 bytes
#endif

    for(unsigned j = 0; j < 2; ++j) {
        glGenVertexArrays(1, &vao[j]);
        gyVidVertexArrayBind(vao[j]);

        glGenBuffers(1, &elementVbo[j]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVbo[j]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, elementsArray, ELYSIAN_AREA_ELEMENT_ARRAY_VBO_TYPE);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);

    glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_POS]);
    glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_TEX_COORD]);

    assert(offsetof(GYVertex, pos) == 0);
    glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)offsetof(GYVertex, pos));
    assert(offsetof(GYVertex, texCoord) == 16);
    glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(offsetof(GYVertex, texCoord)));

    if(j == 0) { //light VAO
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_COLOR]);
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_NORMAL]);
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_TANGENT]);
        glEnableVertexAttribArray(shaders[j]->attribLoc[GLAL_SHADER_NORMAL_Z]);

        assert(offsetof(GYVertex, color) == 24);
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)offsetof(GYVertex, color));

        assert(offsetof(GYVertex, normalZ) == 28);
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_NORMAL_Z], 1, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)offsetof(GYVertex, normalZ));

        assert(offsetof(GYVertex, normal) == 32);
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)offsetof(GYVertex, normal));

        assert(offsetof(GYVertex, tangent) == 48 && offsetof(GYVertex, tangent) % 16 == 0);
        glVertexAttribPointer(shaders[j]->attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(offsetof(GYVertex, tangent)));

        assert(sizeof(GYVertex) == 64);
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
    //if(e.getValue() == 0) return;
    vboEntryFromTileEntry(entry, &e, x, y, l);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    glBufferSubData(GL_ARRAY_BUFFER, mapLocationToVBOIndex(x, y, l)*sizeof(GYVertex), sizeof(GYVertex)*4, &entry);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

}

Entity* AreaBase::getActiveEntityByIndex(const unsigned index) const {
    return (int)index < (int)entities.size()? entities[index] : nullptr;
}


void AreaBase::setActiveEntityByIndex(const unsigned index, Entity*const entity) {
    if(index == entities.size()) { //allocate new entity
        //Entity* entity = Entity::create(valueTable);
        static_cast<Area*>(this)->addEntity(entity);
    } else if(index < entities.size()) {                        //initialize existing entity
        GY_ASSERT(false);
        //entities[index]->fromTable(valueTable);
    }
}

Entity* AreaBase::getInactiveEntityByIndex(const unsigned index) const {
    return (int)index < (int)inactiveEntities.size()? inactiveEntities[index] : nullptr;
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

void AreaBase::behaviorsOnLoad(void) {
    DbgEvent dbg("Calling Behaviors' ON_LOAD triggers.");
    for(auto entity: entities) {
        for(auto behav: entity->getBehaviors()) {
            if(behav->getTriggers() & Behavior::ON_LOAD) behav->onLoad();
        }

    }

}

void AreaBase::behaviorsOnUnload(void) {
    DbgEvent dbg("Calling Behaviors' ON_UNLOAD triggers.");
    for(auto entity: entities) {
        for(auto behav: entity->getBehaviors()) {
            behav->onUnload();
        }

    }

}

}
