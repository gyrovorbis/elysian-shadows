#ifndef ELYSIAN_MAP_DEPTH_BUFFER_H
#define ELYSIAN_MAP_DEPTH_BUFFER_H

#include <cassert>
#include <cstring>
#include <crtp/elysian_area.hpp>
#include <level/elysian_zcoords.hpp>
#include <gyro_shader_api.h>

//Maximum elevation height an object can have
#define ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX 31
//Objects have integer zCoords, force Entities to floats to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_OBJECT_MIN_Y_OFFSET 0.001f
//Applied to every entity at the same buffer index to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_ENTITY_MIN_Z_OFFSET 0.001f
//This needs to be at least as tall as the rendered map area? Not really?
#define ELYSIAN_DEPTH_BUFFER_HEIGHT (ELYSIAN_AREA_MAP_HEIGHT+ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX)
//This needs to be at LEAST as wide as the rendered map area? Not really?
#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#   define ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH 100
#else
#   define ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH ELYSIAN_AREA_MAP_WIDTH*30
#endif
//Guess this could be a bit smaller? Maybe?
#define ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH 100

namespace elysian {

    enum NORMAL_TYPE { NORMAL_FLAT, NORMAL_FRONT, NORMAL_BOTH };

    //Minimal base class for object entries
    struct MapDepthBuffEntry {
        //float x, y;
        unsigned x, y; //tile location
        unsigned layer;
        uint32_t lights;
        TileEntry mapEntry;
        unsigned short renderPass = 0;
    };

    //Minimal base class for entity entries
    struct EntityDepthBuffEntry {
        float zCoordRelative; //required to sort the bitches
        uint32_t lights;
        unsigned short renderPass;
        bool xOverlap;
        //Entity *entity //don't need this in base anymore
    };

    /*
     MapDepthBuffer renders objects and entities occupying a map. The algorithm gives entities
     and objects "depth" by buffering them based on their relative screen positions and heights.
     It is an abstract class template so that both ESTk and ESGamma can implement their own
     specific rendering mechanisms while sharing the core algorithm.

     This is a very time-critical rendering operation, so we're CRTP-ing the bitch and opting
     for compile-time polymorphism over run-time. It's ugly, but it's far more efficient than
     using the standard virtual/interface approach for this abstraction.
     NO FUCKING VTABLES/LATE BINDING. --FG

     TEMPLATE PARAMETERS:
     T - MapDepthBuffer derived type    - MUST reimplement all private methods!
     O - ObjectDepthBuffEntry type      - MUST inherit from ObjectDepthBuffEntry!
     E - EntityDepthBuffEntry type      - MUST inherit from EntityDepthBuffEntry!
     */
    template<typename T, typename O, typename E>
    class MapDepthBuffer {
    private:

        //General interface
        void _getOnscreenTileRegion(int &startX, int &startY, int &endX, int &endY) const {
            return static_cast<const T*>(this)->getOnscreenTileRegion(startX, startY, endX, endY);
        }

        float _getBaseZCoord(void) const {
            return static_cast<const T*>(this)->getBaseZCoord();
        }

        //Object interface
        void _getObjectLayerEntry(const unsigned x, const unsigned y, O &entry) {
            return static_cast<T*>(this)->getObjectLayerEntry(x, y, entry);
        }

        void _renderObjectEntry(const O &entry, const float z) {
            return static_cast<T*>(this)->renderObjectEntry(entry, z);
        }

        void _beginObjectList(void) {
            static_cast<T*>(this)->beginObjectList();
        }

        void _endObjectList(void) {
            static_cast<T*>(this)->endObjectList();
        }

        //Entity interface
        unsigned _getEntityCount(void) {
            return static_cast<T*>(this)->getEntityCount();
        }

        bool _getOnscreenEntity(const unsigned index, E &entry, NORMAL_TYPE &normalType, float &lowestY, float &zCoord) const {
            return static_cast<const T*>(this)->getOnscreenEntity(index, entry, normalType, lowestY, zCoord);
        }

        void _renderEntityEntry(E &entry) {
            static_cast<T*>(this)->renderEntityEntry(entry);
        }

        void _renderCustomPassImmediate(void) {
             static_cast<T*>(this)->renderCustomPassImmediate();
        }

        void _renderCustomPassAtElev(const int elev, const NORMAL_TYPE normalType) const {
            static_cast<const T*>(this)->renderCustomPassAtElev(elev, normalType);
        }

        void _disableDepthTest(void) {
            //static_cast<T*>(this)->disableDepthTest();
        }

        void _enableDepthTest(void) {
            static_cast<T*>(this)->enableDepthTest();
        }

        uint32_t _getLightCount(void) const {
            return static_cast<const T*>(this)->getLightCount();
        }

        uint32_t _startLightPass(uint32_t& lightsLeft) {
            return static_cast<T*>(this)->startLightPass(lightsLeft);
        }

        bool _isShadowPass(void) const {
            return static_cast<const T*>(this)->isShadowPass();
        }

        bool _isDepthPrePass(void) const {
            return static_cast<const T*>(this)->isDepthPrePass();
        }

    protected:

        //Culling functions
        //Just cull in getObjectLayerEntry and getOnscreenEntity?

        //Bounding volumes for tileZRows
        //Or whatever the shit grouping if you don't want to do that

        //Object buffer
        unsigned	_objectCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
        O			_objectBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
        uint32_t	_objectRowLights[NORMAL_BOTH][ELYSIAN_DEPTH_BUFFER_HEIGHT];
        O			_objectBuffImmediate[ELYSIAN_DEPTH_BUFFER_HEIGHT*ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
        unsigned	_objectBuffImmediateCount = 0;

        unsigned	_queue[NORMAL_BOTH][ELYSIAN_DEPTH_BUFFER_HEIGHT*ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
        unsigned	_queued = 0;
        int			_nextRow = 0;
        int			_curRow = 0;

        //Entity buffer
        unsigned _entityCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
        E _entityBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH];
        uint32_t _entityRowLights[ELYSIAN_DEPTH_BUFFER_HEIGHT];

        uint32_t _lightsRendered = 0;
        uint32_t _lightsActive	= 0;

        int _scrStartX, _scrStartY, _scrEndX, _scrEndY;
        unsigned _depthBuffMaxVisible;
        unsigned _depthBuffMaxHeight;

        void _processTiles(void);
        void _processEntities(void);
        void _beginRenderNextTileRow(void);
        void _renderDepthBuffTileRow(const unsigned i, const NORMAL_TYPE normalType);
        void _renderDepthBuffEntityRow(const unsigned i);

        bool _tileEntryLightPass(O& tileEntry) {
            if((tileEntry.lights | _lightsRendered) == _lightsRendered || _isShadowPass() || _isDepthPrePass()) { //it's done!
                ++tileEntry.renderPass;
                _renderObjectEntry(tileEntry, ZCoordConstants::mapBase+(tileEntry.mapEntry.getElev())*32.0f /*zCoordBufferedBase+i+1.0f*/); //WILL THIS WORK FOR NO ELEVATION!?
                return true;
            } else if((tileEntry.lights & ~_lightsActive) == tileEntry.lights) { //can't do dick this pass
                return false;
            } else { //partial render this pass
                ++tileEntry.renderPass;
                tileEntry.lights &= ~_lightsActive;
                _renderObjectEntry(tileEntry, ZCoordConstants::mapBase+(tileEntry.mapEntry.getElev())*32.0f /*zCoordBufferedBase+i+1.0f*/);
                return false;
            }
        }

        void _renderOrBufferImmediateTileEntry(O& tileEntry) {
            if(!_tileEntryLightPass(tileEntry)) {
                _objectBuffImmediate[_objectBuffImmediateCount++] = tileEntry;
            }
        }

        void _renderOrQueueTileEntry(const unsigned index, O& tileEntry) {
            if(!_tileEntryLightPass(tileEntry)) {
                _queue[_nextRow][_queued++] = index;
            }
        }


    public:
        void render(void);
    };

    /* Algorithm Details:

     ZCOORD RANGE:
        From _getBaseZCoord() to (_getBaseZCoord() + ELYSIAN_DEPTH_BUFFER_HEIGHT),
        depending on how large of a region is returned from _getOnscreenTileRegion().
     */

#if 1
    template<typename T, typename O, typename E>
    inline void MapDepthBuffer<T, O, E>::render(void) {

        _getOnscreenTileRegion(_scrStartX, _scrStartY, _scrEndX, _scrEndY);

        //assert(_scrStartX <= _scrEndX);
        //assert(_scrStartY <= _scrEndY);

        //Maximum number of rows depth buffer could need based on visible region
        _depthBuffMaxVisible = _scrEndY - _scrStartY + 1 + ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX;
        //Cap max at depth buffer height
        _depthBuffMaxHeight = (_depthBuffMaxVisible > ELYSIAN_DEPTH_BUFFER_HEIGHT)? ELYSIAN_DEPTH_BUFFER_HEIGHT: _depthBuffMaxVisible;

        //Clear depth buffer counters
        memset(_objectCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        memset(_entityCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        _enableDepthTest();
        _processTiles();

        // ==== QUEUE AND IMMEDIATE RENDER ENTITIES ====

        const float     mapStartY   = _scrStartY*32.0f;
        const unsigned  entityCount = _getEntityCount();
        E               entityEntry;
        NORMAL_TYPE     normalType;
        float           entityBase;
        float           zCoord;

        //Determine entity Z coord and add to buff
        //If PERFECTLY tile-aligned, add a delta/epsilon to prevent duplicate Zing - or don't bother? Doesn't matter?
        for(unsigned i = 0; i < entityCount; ++i) {
            if(!_getOnscreenEntity(i, entityEntry, normalType, entityBase, zCoord)) continue; //not onscreen

            //FUCKING DISGUSTING, DIRTY, UGLY Z-FIGHTING HACK!!!!
            //float worldZCoord = (ZCoordConstants::mapBase + (elev)*32.0f);//-(((float)entityBase/32.0f)-((int)entityBase/32));

            //calculate relative Z coord
            //float relZCoord = ((entityBase+16.0f)-mapStartY)/32.0f + (int)((int)(zCoord+16)/32);
            float relZCoord = ((entityBase)-mapStartY)/32.0f;//-1;// + (zCoord)/32.0f;
            //Add depth buffer entry
            unsigned zIndex = (unsigned)relZCoord;
            if(zIndex >= _depthBuffMaxHeight) zIndex = _depthBuffMaxHeight-1;
            //assert(zIndex >= 0 && zIndex < depthBuffMaxHeight);

            _entityBuff[zIndex][_entityCount[zIndex]] = entityEntry;
            _entityBuff[zIndex][_entityCount[zIndex]].zCoordRelative = relZCoord;

            //render immediately
            if(zCoord < 32.0f && normalType == NORMAL_FLAT) _renderEntityEntry(_entityBuff[zIndex][_entityCount[zIndex]]);
            //queue and render later
            _entityRowLights[zIndex] = (!_entityCount[zIndex])? entityEntry.lights : _entityRowLights[zIndex] | entityEntry.lights;
            ++_entityCount[zIndex];
        }

        //Sort entity z entries per z coord (ascending order)
        //IMPLEMENT ME MORE EFFICIENTLY! BUBBLE SORT IS FUCKING GAY
        for(unsigned j = 0; j < _depthBuffMaxHeight; ++j) {
            for(unsigned i = 0; i < _entityCount[j]; ++i) {
                for(unsigned k = i+1; k < _entityCount[j]; ++k) {
                    if(_entityBuff[j][i].zCoordRelative > _entityBuff[j][k].zCoordRelative) {
                        E temp = _entityBuff[j][i];
                        _entityBuff[j][i] = _entityBuff[j][k];
                        _entityBuff[j][k] = temp;
                    }
                }
            }
        }

        for(unsigned j = 0; j < _depthBuffMaxHeight; ++j) {
            if(_entityCount[j]) {
                unsigned onscreenLights = 0;
                if(!gyVidShadowPass()) {
                    for(unsigned i = 0; i < 32; ++i) if(_entityRowLights[i]&(0x1<<i)) ++onscreenLights;
                }
                const bool multipass = (onscreenLights > GYRO_VIDEO_LIGHT_SOURCES_MAX);

                for(int i = 0; i < (int)_entityCount[j]-1; ++i) {
                    _entityBuff[j][i].xOverlap = (multipass)? true: false;
                }
                _entityBuff[j][_entityCount[j]-1].xOverlap = true; //always flag last as sentinel for renderer
            }
        }
        // ==== RENDER ENTITIES AND TILES QUEUED IN DEPTH BUFFER ====


        //Iterate through depth buffers and render from lowest to highest z coord
        for(int l = _depthBuffMaxHeight; l >= 0; --l) {
            unsigned i = (_isShadowPass() || _isDepthPrePass())? _depthBuffMaxHeight-l-1 : l;
            i = l;
_enableDepthTest();

            if(_isShadowPass() || _isDepthPrePass()) {

                _renderDepthBuffTileRow(i, NORMAL_FRONT);

                //_renderCustomPassAtElev(i, NORMAL_FRONT);

            } else {
                _renderDepthBuffTileRow(i, NORMAL_FLAT);

                _renderCustomPassAtElev(i, NORMAL_FLAT);
            }

            //if(_entityCount[i]) {
                //_endObjectList();
//static_cast<T*>(this)->unbindTileBuffer();
                //Entities come first, since they are always above the lowest point on the object
                //float entityZDelta = 0.0f;

                auto*const entityRow = _entityBuff[i];
                for(int j = 0; j < (int)_entityCount[i]; ++j) {
                    uint32_t lightsLeft = 0;
                    int endIndex = j-1;

                    do {
                        lightsLeft |= entityRow[++endIndex].lights;
                    } while(!entityRow[endIndex].xOverlap);

                    do {
                        const uint32_t activeLights = _startLightPass(lightsLeft);

                        for(int k = j; k <= endIndex; ++k) {
                            if(((entityRow[k].lights & (~activeLights)) != entityRow[k].lights) || entityRow[k].renderPass == 0) { //we can do something this pass
                                ++entityRow[k].renderPass;
                                _renderEntityEntry(entityRow[k]);
                                entityRow[k].lights &= ~activeLights;
                            }
                        }

                    } while(lightsLeft);

                    j = endIndex;
                }

            if(_isShadowPass() || _isDepthPrePass()) {
                _renderDepthBuffTileRow(i, NORMAL_FLAT);

              //  _renderCustomPassAtElev(i, NORMAL_FLAT);
            } else {
            _renderDepthBuffTileRow(i, NORMAL_FRONT);

             _renderCustomPassAtElev(i, NORMAL_FRONT);
            }
        }

    }
#else
    template<typename T, typename O, typename E>
    inline void MapDepthBuffer<T, O, E>::render(void) {

        _getOnscreenTileRegion(_scrStartX, _scrStartY, _scrEndX, _scrEndY);
        O objEntry;

        for(int i = ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX; i >= 0; --i) {
            for(unsigned y = _scrStartY; y <= _scrEndY; ++y) {
                for(unsigned x = _scrStartX; x <= _scrEndX; ++x) {
                    _getObjectlayerEntry(x, y, objEntry);
                    if(objEntry.mapEntry)

                }

            }
        }


        //assert(_scrStartX <= _scrEndX);
        //assert(_scrStartY <= _scrEndY);

        //Maximum number of rows depth buffer could need based on visible region
        _depthBuffMaxVisible = _scrEndY - _scrStartY + 1 + ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX;
        //Cap max at depth buffer height
        _depthBuffMaxHeight = (_depthBuffMaxVisible > ELYSIAN_DEPTH_BUFFER_HEIGHT)? ELYSIAN_DEPTH_BUFFER_HEIGHT: _depthBuffMaxVisible;

        //Clear depth buffer counters
        memset(_objectCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        memset(_entityCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        _enableDepthTest();
        _processTiles();
    }
#endif

    // ==== QUEUE AND IMMEDIATE RENDER TILES ====
    template<typename T, typename O, typename E>
    inline void MapDepthBuffer<T, O, E>::_processTiles(void) {
        uint32_t			lightsLeft			= 0;
        uint32_t            onscreenLights      = 0;
        const uint32_t		onscreenLightCount		= _getLightCount();
        _lightsRendered = 0;

        for(unsigned i = 0; i < onscreenLightCount; ++i) onscreenLights |= (1<<i);
        bool multipass = (onscreenLights > GYRO_VIDEO_LIGHT_SOURCES_MAX);
#if 0
        const unsigned outerLayerLoopEnd = multipass? ELYSIAN_AREA_MAP_LAYERS : 1;
        const unsigned innerLayerLoopEnd = multipass? 1 : ELYSIAN_AREA_MAP_LAYERS;
#else
        const unsigned outerLayerLoopEnd = multipass? 1 : ELYSIAN_AREA_MAP_LAYERS;
        const unsigned innerLayerLoopEnd = multipass? ELYSIAN_AREA_MAP_LAYERS : 1;
#endif

        if(!_isShadowPass()) _renderCustomPassAtElev(-1, NORMAL_FLAT);

        static_cast<T*>(this)->bindTileBuffer();

        int outerCount = 0;

        for(unsigned k = 0; k < outerLayerLoopEnd; ++k) {
            ++outerCount;
            _lightsRendered = 0;
            _objectBuffImmediateCount = 0;
            _queued = 0;
            lightsLeft = onscreenLights;
           // static_cast<T*>(this)->flushTileBuffer();
            _lightsRendered |= _lightsActive = _startLightPass(lightsLeft);


            _enableDepthTest();
            //_beginObjectList();
            for(int j = _scrStartY; j <= _scrEndY; ++j) {
                for(int i = _scrStartX; i <= _scrEndX; ++i) {
                    unsigned layer = k;
                    do {
                        O curMapEntry;
                        curMapEntry.x = i;
                        curMapEntry.y = j;
                        curMapEntry.layer = layer;
                        _getObjectLayerEntry(i, j, curMapEntry);
                        /* Had to move this check to client code, because the Toolkit actually does
                        render tile 0 decorations when selecting/selected/filling. It's moleculary
                        slower, but it's at least a clean solution.
                        */
                        if(!curMapEntry.mapEntry.getIndex()) continue; //not the blank object
                        int elevation = curMapEntry.mapEntry.getElev();//-1;

                        if(elevation == 0 && !curMapEntry.mapEntry.getPlane()) { //render tile immediately at elevation -1 (below entities)
                            //Immediate render, or buffer if it requires multiple passes
                            if(!_isShadowPass()) _renderOrBufferImmediateTileEntry(curMapEntry);
                        } else { //Add to depth buffer if elevated
                            unsigned zCoord = j-_scrStartY+elevation;
                            //if(zCoord >= depthBuffMaxHeight) zCoord = depthBuffMaxHeight-1;
                            assert(zCoord < _depthBuffMaxHeight);

                            _objectBuff[zCoord][_objectCount[zCoord]] = curMapEntry;
                            _objectRowLights[curMapEntry.mapEntry.getNormal()][zCoord] |= curMapEntry.lights;
                            ++_objectCount[zCoord];
                        }
                    } while(++layer < innerLayerLoopEnd);
                }
            }

            if(_objectBuffImmediateCount) {
                //Make a pass through immediate tile multiplass buffer
               // _disableDepthTest();
                _lightsRendered |= _lightsActive = _startLightPass(lightsLeft);
                for(unsigned j = 0; j < _objectBuffImmediateCount; ++j) {
                    _renderOrQueueTileEntry(j, _objectBuffImmediate[j]);
                }

                //Continue making light passes until there are no more immediate tiles queued
                while(_queued) {
                    const unsigned count = _queued;
                    _queued = 0;
                    _curRow = _nextRow;
                    _nextRow = !_curRow;
                    //static_cast<T*>(this)->flushTileBuffer();
                    _lightsRendered |= _lightsActive = _startLightPass(lightsLeft);
                    for(unsigned j = 0; j < count; ++j) {
                        _renderOrQueueTileEntry(j, _objectBuffImmediate[_queue[_curRow][j]]);
                    }
                }
            }
        }

        //static_cast<T*>(this)->flushTileBuffer();
        //static_cast<T*>(this)->unbindTileBuffer();
        if(!_isShadowPass()) _renderCustomPassImmediate();
    }

    template<typename T, typename O, typename E>
    inline void MapDepthBuffer<T, O, E>::_beginRenderNextTileRow(void) {
        //_queued = 0;
        //_lightsRendered = 0;
    }

    template<typename T, typename O, typename E>
    inline void MapDepthBuffer<T, O, E>::_renderDepthBuffTileRow(const unsigned int i, const NORMAL_TYPE normalType) {
        //Tiles - rendered with Z at the BOTTOM of the map tile location

        static_cast<T*>(this)->bindTileBuffer();
        unsigned int stateChanges = 0;

        uint32_t totalLights = _objectRowLights[normalType][i];
         bool multipass = (totalLights > GYRO_VIDEO_LIGHT_SOURCES_MAX);
        // multipass = false;
        const unsigned outerLayerLoopEnd = multipass? ELYSIAN_AREA_MAP_LAYERS : 1;

        for(unsigned l = 0; l < outerLayerLoopEnd; ++l) {
           // _beginObjectList();
            _queued = 0;
            _lightsRendered = 0;
            _objectRowLights[normalType][i] = totalLights;
            //static_cast<T*>(this)->flushTileBuffer();
            _lightsRendered |= _lightsActive = _startLightPass(_objectRowLights[normalType][i]);
            for(unsigned j = 0; j < _objectCount[i]; ++j) {
                if(multipass && _objectBuff[i][j].layer != l) continue;
                if(_objectBuff[i][j].mapEntry.getNormal() == normalType)
                    _renderOrQueueTileEntry(j, _objectBuff[i][j]);
            }
            if(_queued) {
                //_disableDepthTest();
                while(_queued) {
                    const unsigned count = _queued;
                    _queued = 0;
                    _curRow = _nextRow;
                    _nextRow = !_curRow;
                    //static_cast<T*>(this)->flushTileBuffer();
                    ++stateChanges;
                    _lightsRendered |= _lightsActive = _startLightPass(_objectRowLights[normalType][i]);
                    for(unsigned j = 0; j < count; ++j) {
                        _renderOrQueueTileEntry(j, _objectBuff[i][_queue[_curRow][j]]);
                    }
                }
            }
        }

       // Debug::printf(Debug::VERBOSE, "stateChanges - %d", stateChanges);

       // static_cast<T*>(this)->flushTileBuffer();
       //static_cast<T*>(this)->unbindTileBuffer();
    }
}


#endif
