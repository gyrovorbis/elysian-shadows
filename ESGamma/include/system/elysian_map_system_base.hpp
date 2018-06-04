#ifndef ELYSIAN_MAP_SYSTEM_BASE_HPP
#define ELYSIAN_MAP_SYSTEM_BASE_HPP

#include "elysian_system.hpp"
#include "crtp/elysian_camera_system.hpp"
#include <crtp/elysian_depth_buff_entry.hpp>
#include <level/elysian_map_depth_buffer.hpp>
#include <level/elysian_zcoords.hpp>
#include <crtp/elysian_renderable.hpp>
#include <crtp/elysian_particle_system.hpp>
#include <utility/elysian_mem_pool.hpp>
#include <crtp/elysian_entity.hpp>
#include "elysian_light_system.hpp"
#include <level/elysian_tile.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_level.hpp>
#include <elysian_timer_system.hpp>

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include <stdint.h>

//#define NEW_RENDERER
//#define DEPTH_PREPASS

#define ELYSIAN_MAP_SYSTEM_ONSCREEN_ENTITY_POOL_SIZE	1000

/*
//Objects have integer zCoords, force Entities to floats to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_OBJECT_MIN_Y_OFFSET 0.001f
//Applied to every entity at the same buffer index to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_ENTITY_MIN_Z_OFFSET 0.001f
//This needs to be at LEAST as wide as the rendered map area? Not really?
#define ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH 40
//Guess this could be a bit smaller? Maybe?
#define ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH 20
*/


namespace elysian {

    struct Tile;
    struct Object;

    class GLMapSystemCrtp {
    protected:
        enum DEPTH_PASS_STATE {
            DEPTH_Z_PREPASS,
            LIGHTING_PASS,
            POST_PASSES
        } _depthPassState = DEPTH_Z_PREPASS;

        uint32_t* _elements = nullptr;
        int _size = 0;
        int _lastSize = 0;
        intptr_t _lastOffset = 0;

        void _drawElements(void);
        void _orphanVbo(void);
        void _bindArrayVbo(int buffer);
        void _setDepthPrepassState(DEPTH_PASS_STATE state);

    public:
        void deactivateArea(void);

    };

    extern int _mapCount;

    template<typename P=MapSystem, typename T=MapDepthBuffEntryImplBase, typename E=EntityDepthBuffEntryImplBase>
    class MapSystemCrtp:
        public System,
        public GLMapSystemCrtp,
        public MapDepthBuffer<P, T, E>
    {
    private:
        Pool<Entity*>				_onscreenEntityPool	= Pool<Entity*>(ELYSIAN_MAP_SYSTEM_ONSCREEN_ENTITY_POOL_SIZE);
        static const GYColor4		_tileColor;
        bool _renderLayer[4];
        //animation
        //toggles
        bool _animate;
        //stats

        void updateAnimation(void);

        //Represents the current animation state of every Tile
        struct AnimTile {
            float elapsedTime;
            uint16_t curIndex;
        } _animTiles[ELYSIAN_TILESHEET_TILES];

    public:

        static void staticInitialize(void);
        static void staticUninitialize(void);

                    MapSystemCrtp(void);

        void        update(void);
        void        render(void) {
#ifdef DEPTH_PREPASS
            if(!isShadowPass()) {
            _mapCount = 0;
            _setDepthPrepassState(DEPTH_Z_PREPASS);
                MapDepthBuffer<P, T, E>::render();
                _setDepthPrepassState(LIGHTING_PASS);
                MapDepthBuffer<P, T, E>::render();
                _setDepthPrepassState(POST_PASSES);
            } else {
                _mapCount = 0;
                MapDepthBuffer<P, T, E>::render();
            }
#else
            _mapCount = 0;
                MapDepthBuffer<P, T, E>::render();
#endif
        }

        void        renderOverlay(void);
        void        resetAnimation(void);
        //void dbgDump(void);

        //=== MapDepthBuffer Implementation ===

        void        getOnscreenTileRegion(int &startX, int &startY, int &endX, int &endY) const;
        float       getBaseZCoord(void) const;
        void        getObjectLayerEntry(const unsigned x, const unsigned y, T &entry);
        void        renderObjectEntry(const T &entry, const float z);
        void        beginObjectList(void);
        void        endObjectList(void);
        unsigned    getEntityCount(void);
        bool        getOnscreenEntity(const unsigned index, E &entry, NORMAL_TYPE &normalType, float &lowestY, float &zCoord) const;
        void        renderEntityEntry(const E &entry);
        void        disableDepthTest(void);
        void        enableDepthTest(void);
        void        renderCustomPassImmediate(void);
        void        renderCustomPassAtElev(const int elev, const NORMAL_TYPE normalType) const;
        uint32_t    startLightPass(uint32_t& lightsLeft);
        uint32_t    getLightCount(void) const;
        bool        isShadowPass(void) const;
        bool        isDepthPrePass(void) const;

        void        flushTileBuffer(void);
        void        bindTileBuffer(void);
        void        unbindTileBuffer(void);

        static void flushVertexBatchCallback(void* self);

        //=== /MapDepthBuffer Implementation ===

        //Toggles
        bool        toggleRenderLayer(const unsigned layer);
        bool        getAnimate(void) const;
        void        setAnimate(const bool v);
        bool        getRenderTileLayer(const unsigned short int layer) const;
        void        setRenderTileLayer(const unsigned short int layer, const bool v);

    };


    class MapSystemBase: public MapSystemCrtp<MapSystem, MapDepthBuffEntryImpl, EntityDepthBuffEntryImpl> {
    public:
        void update(void);

#ifdef NEW_RENDERER
        void render(void);
#endif
    };


    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::flushVertexBatchCallback(void* self) {
        reinterpret_cast<MapSystemCrtp<P, T, E>*>(self)->flushTileBuffer();
    }


//-----INLINEZ-----

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::renderCustomPassImmediate(void) { enableDepthTest(); }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::getOnscreenTileRegion(int &startX, int &startY, int &endX, int &endY) const {
        getCameraSys()->getVisibleTileRegion(startX, startY, endX, endY);
    }

    template<typename P, typename T, typename E>
    inline float MapSystemCrtp<P, T, E>::getBaseZCoord(void) const {
        return ZCoordConstants::mapBase;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::getObjectLayerEntry(const unsigned /*x*/, const unsigned /*y*/, T &entry) {
        entry.mapEntry = _curArea->getTileEntry(entry.layer, entry.x, entry.y);
        entry.lights = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
        entry.renderPass = 0;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::renderObjectEntry(const T& entry, const float /*z*/) {
        if(!_renderLayer[entry.layer]) return; //Don't render if this layer has been toggled off.

         auto& elemArraySize  = _curArea->elementsArraySize[/*gyVidShadowPass()*/0];
         auto* elemArray     = _curArea->elementsArray[/*gyVidShadowPass()*/0];

                    gyVidDepthTest(true);
        if(!gyVidShadowPass()) {
            const GY_RECT_PLANE_TYPE      newPlane = entry.mapEntry.getPlane();
            const GY_RECT_TEX_ORIENT      newOrient = entry.mapEntry.getOrient();
            const GY_ALPHA_LEVEL          newAlpha = (entry.renderPass > 1 ||
                                                      _curLevel->tileset[entry.mapEntry.getIndex()].getTransparency())?
                                                        GY_ALPHA_FULL : GY_ALPHA_NONE;
            const GY_BLEND_MODE           newBlend = (newAlpha == GY_ALPHA_FULL)? GY_BLEND_ONE_MINUS_SRC_ALPHA : GY_BLEND_ONE;

            //Blending enabled state change
          /*  if(newAlpha != glalAlphaLevel) {
              flushTileBuffer();
             gyVidAlphaLevel(newAlpha);
            }*/
            //Blending mode state change
            if(entry.renderPass > 1  || newAlpha != GY_ALPHA_NONE) {
                //disableDepthTest();
            }
            //We don't give a fuck about blend modes when blending is off
            else if(entry.renderPass == 1) {
               enableDepthTest();
            }

           gyVidDepthTest(true);
          //  gyVidBlendMode(newBlend);
            gyVidAlphaLevel(newAlpha);
            gyVidRectPlaneType(newPlane);
            gyVidRectTexOrient(newOrient);
        }

#if 0
        const unsigned int indexBase = (entry.layer*getCurrentArea()->height*getCurrentArea()->width +
                           entry.y*getCurrentArea()->width + entry.x)*4;
#else
        const unsigned indexBase = getCurrentArea()->mapLocationToVBOIndex(entry.x, entry.y, entry.layer);
#endif

        const unsigned indices[4] = { indexBase, indexBase+1,indexBase+2, indexBase+3 };

        //combine previous triangle strip with this one by using degenerate triangles if we're not the first
        if(elemArraySize) {
            elemArray[elemArraySize]		= elemArray[elemArraySize-1];
            elemArray[elemArraySize+1]	= indices[0];
            elemArraySize += 2;
        }

        auto checkIndex = [&](const uint32_t index) {
            if((int)index < _curArea->minIndex[gyVidShadowPass()]) _curArea->minIndex[gyVidShadowPass()] = (int)index;
            if((int)index > _curArea->maxIndex[gyVidShadowPass()]) _curArea->maxIndex[gyVidShadowPass()] = (int)index;
            return index;
        };

        elemArray[elemArraySize]      = checkIndex(indices[0]);
        elemArray[elemArraySize+1]    = checkIndex(indices[1]);
        elemArray[elemArraySize+2]    = checkIndex(indices[2]);
        elemArray[elemArraySize+3]    = checkIndex(indices[3]);
        elemArraySize += 4;

        if(elemArraySize + 6 >= ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE) {
            flushTileBuffer();
            elemArraySize = 0;
            _curArea->lastOffset[/*gyVidShadowPass()*/0] = 0;
           // _curArea->minIndex = INT_MAX;
           // _curArea->maxIndex = 0;
            _curArea->lastElemArraySize[/*gyVidShadowPass()*/0] = 0;
            gyVidVertexArrayBind(_curArea->vao[gyVidShadowPass()]);
                _bindArrayVbo(_curArea->vertexVbo);
            _orphanVbo();
        }
}

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::beginObjectList(void) {
        if(!_renderLayer[0] && !_renderLayer[1] && !_renderLayer[2]) return; //Don't render if this layer has been toggled off.
       // flushTileBuffer();
        bindTileBuffer();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::bindTileBuffer(void) {
      //  assert(_curArea->elementsArraySize[_curArea->elementsCurSegment] == 0);

        gyVidMaterialBind(nullptr);
        gyVidTexBind(_curLevel->tileSheet);
        gyVidNormalMapBind(_curLevel->normalSheet);
        gyVidSpecularMapBind(_curLevel->specularSheet);

        gyVidVertexBatchBegin(&flushVertexBatchCallback, this);

       // glBindVertexArray(_curArea->vao[gyVidShadowPass()]);

       // glBindVertexArray(_curArea->vao[gyVidShadowPass()][0]);
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::endObjectList(void) {
        if(!_renderLayer[0] && !_renderLayer[1] && !_renderLayer[2]) return; //Don't render if this layer has been toggled off.

        flushTileBuffer();
        unbindTileBuffer();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::flushTileBuffer(void) {

        _size = _curArea->elementsArraySize[/*gyVidShadowPass()*/0];
        _elements     = _curArea->elementsArray[/*gyVidShadowPass()*/0];
        _lastSize = _curArea->lastElemArraySize[/*gyVidShadowPass()*/0];
        _lastOffset = _curArea->lastOffset[0];
        if(_size-_lastSize == 0) return;



   /*     GLenum result;
        result = glClientWaitSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment], GL_SYNC_FLUSH_COMMANDS_BIT, 100000000);
      while(result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED) {
          result = glClientWaitSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment], 0, 100);
      }*/

            //result = glClientWaitSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment], 0, 100);

      //  }
    /*    switch(result) {
        case GL_ALREADY_SIGNALED:
            Debug::printf(Debug::VERBOSE, "ALREADY SIGNALED");
            break;
        case GL_CONDITION_SATISFIED:
                        Debug::printf(Debug::VERBOSE, "condition satisfied");
            break;
        case GL_TIMEOUT_EXPIRED:
                        Debug::printf(Debug::VERBOSE, "timeout expired");
            break;
        case GL_WAIT_FAILED:
                        Debug::printf(Debug::VERBOSE, "wait failed");
break;
        default:
                    Debug::printf(Debug::VERBOSE, "else");
            break;

        }*/

#if GYRO_PLATFORM != GYRO_PLATFORM_DC
        glalVertexRenderCount += _curArea->elementsArraySize[0];
#endif

        //assert(elemArraySize <= maxi);
       // assert(elemArraySize < ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE);
        gyVidVertexArrayBind(_curArea->vao[gyVidShadowPass()]);
        _bindArrayVbo(_curArea->vertexVbo);
         // glDeleteSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment]);
          //  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _curArea->elementVbo);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, );
        _drawElements();

      //glDrawRangeElements(GL_TRIANGLE_STRIP, _curArea->minIndex[gyVidShadowPass()], _curArea->maxIndex[gyVidShadowPass()],  (elemArraySize-_curArea->lastElemArraySize[gyVidShadowPass()]), GL_UNSIGNED_SHORT, (const void*)_curArea->lastOffset[gyVidShadowPass()]);
        _curArea->lastElemArraySize[/*gyVidShadowPass()*/0] = _curArea->elementsArraySize[0];
        _curArea->lastOffset[/*gyVidShadowPass()*/0] = _curArea->elementsArraySize[0]*sizeof(uint32_t);

        _curArea->minIndex[0] = INT_MAX;
        _curArea->maxIndex[0] = 0;

     //   _curArea->elementsArraySyncs[_curArea->elementsCurSegment] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        //glDrawArrays(GL_TRIANGLE_STRIP, 0, elemArraySize);
/*
        if(++_curArea->elementsCurSegment >= ELYSIAN_MAP_SYSTEM_SEGMENTS_PER_VBO) {
            _curArea->elementsCurSegment = 0;
        }
*/
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::unbindTileBuffer(void) {
        //gyVidVertexBatchEnd();
       // gyVidBindDefaultVertexArray();
    }

    template<typename P, typename T, typename E>
    inline unsigned MapSystemCrtp<P, T, E>::getEntityCount(void) {
        if(!_renderLayer[3]) return 0;
        _onscreenEntityPool.clear();
        auto& entities = _curArea->entities;

        for(auto entity : entities) {
            if(entity->getParentEntity()) continue; //will be rendered with the parent!
            if(getLightSys()->isShadowPass() && !entity->isShadowVolume()) {
                continue;
            }
            if(getCameraSys()->isOnscreen(*entity)) {
                _onscreenEntityPool.claimFreeEntry() = entity;
            }
        }
        //Debug::printf(Debug::VERBOSE, "ONSCREEN ENTITIES - %d", _onscreenEntityPool.size());
        return _onscreenEntityPool.size();
    }

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::getOnscreenEntity(const unsigned index, E &entry, NORMAL_TYPE &normalType, float &lowestY, float& zCoord) const {
        Entity &entity = *(_onscreenEntityPool)[index];
        //if(getCameraSys()->isOnscreen(entity)) {
            entry.entity = &entity;
            entry.renderPass = 0;
            entry.lights = getLightSys()->getLightMask(entity);
            normalType = (NORMAL_TYPE)entity.getPlaneType();
            lowestY = entity.getDepthBaseY();
            zCoord = entity._transform._pos.z;
            return true;
        //}
        //return false;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::renderEntityEntry(const E &entry) {
       // entry.entity->transform.setPosZ(entry.zCoordWorld);

        //Blending mode state change
        if(entry.renderPass > 1) {
          //  disableDepthTest();
        }
        //We don't give a fuck about blend modes when blending is off
        else if(entry.renderPass == 1) {
           enableDepthTest();
        }

         entry.entity->render();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::disableDepthTest(void) {
        if(gyVidShadowPass()) return;
       // flushTileBuffer();
       // gyVidDepthTest(false);
        //glEnable(GL_POLYGON_OFFSET_FILL);
        //glPolygonOffset(2.0f, 2.0f);
      //  glDepthMask(GL_FALSE);
     //   glDepthFunc(GL_EQUAL);
   //  gyVidAlphaLevel(GY_ALPHA_FULL);
     //  gyVidBlendMode(GY_BLEND_ONE);
  //      _blendPass = true;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::enableDepthTest(void) {
        //if(gyVidShadowPass()) return;
      //  flushTileBuffer();
        gyVidDepthTest(true);
      //  glDepthMask(GL_TRUE);
        //glEnable(GL_BLEND);
        //glDisable(GL_BLEND);
        // glDepthFunc(GL_LEQUAL);
         gyVidAlphaLevel(GY_ALPHA_NONE);
        //glDisable(GL_POLYGON_OFFSET_FILL);
        gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
      //  _blendPass = false;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::renderCustomPassAtElev(const int elev, const NORMAL_TYPE normalType) const {
        if(normalType == NORMAL_FLAT && elev != -1) getPartSys()->renderAtElev(elev);
    }

    template<typename P, typename T, typename E>
    inline uint32_t MapSystemCrtp<P, T, E>::startLightPass(uint32_t& lightsLeft) { return getLightSys()->startLightPass(lightsLeft); }

    template<typename P, typename T, typename E>
    inline uint32_t MapSystemCrtp<P, T, E>::getLightCount(void) const { return gyVidShadowPass()? 0 : getLightSys()->getLightCount(); }

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::getRenderTileLayer(const unsigned short layer) const { return _renderLayer[layer]; }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::setRenderTileLayer(const unsigned short layer, const bool v) { _renderLayer[layer] = v; }



    template<typename P, typename T, typename E>
    const GYColor4      MapSystemCrtp<P, T, E>::_tileColor = { 255, 255, 255, 255 }; //I s'pose this could be configurable?
/*    template<typename P, typename T, typename E>
    GY_RECT_PLANE_TYPE  MapSystemCrtp<P, T, E>::_curPlaneType = GY_RECT_PLANE_BOTTOM;
    template<typename P, typename T, typename E>
    GY_RECT_TEX_ORIENT  MapSystemCrtp<P, T, E>::_curTexOrient = GY_RECT_TEX_ORIENT_NORMAL;
    template<typename P, typename T, typename E>
    TEXTURE_FLIP  MapSystemCrtp<P, T, E>::_curTexFlip = TEXTURE_FLIP_NONE; */

    template<typename P, typename T, typename E>
    inline MapSystemCrtp<P, T, E>::MapSystemCrtp(void):
        _animate(true) {
        for(unsigned i = 0; i < 4; ++i) _renderLayer[i] = true;
        //for(unsigned int i = 0; i < 256; ++i) _currentTile[i] = _currentObject[i] = i;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::update(void) {
        //Update entity elevation
        //Update entity z transitionals

     //   updateAnimation();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::updateAnimation(void) {
        //Cache time since last frame
        float deltaTime = getTimerSys()->getDeltaTime();

        for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) {
            if(_curLevel->tileset[_animTiles[i].curIndex].animated) {
                Tile& curTile = _curLevel->tileset[_animTiles[i].curIndex];

                //update timer
                _animTiles[i].elapsedTime += deltaTime;

                //Change tile index and reset timer
                if(_animTiles[i].elapsedTime >= curTile.animTime) {
                    _animTiles[i].curIndex = curTile.nextIndex;
                    _animTiles[i].elapsedTime = 0.0f;

                    //FUCKING KILL ME!!!!!!!!!!!!!!!!!!!!!1111111
                    for(unsigned l = 0; l < 3; ++l) {
                        for(unsigned y = 0; y < getCurrentArea()->height; ++y) {
                            for(unsigned x = 0; x < getCurrentArea()->width; ++x) {
                                if(getCurrentArea()->getTileEntry(l, x, y).getIndex() == i) {
                                    getCurrentArea()->animateVboTileEntry(l, x, y, curTile.nextIndex);
                                }
                            }
                        }
                    }


                }
            }
        }
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::resetAnimation(void) {
        for(unsigned i = 0; i < ELYSIAN_TILESHEET_TILES; ++i) {
            _animTiles[i].elapsedTime = 0.0f;
            _animTiles[i].curIndex = i;
        }
    }

/* VERY FUCKING TIME-CRITICAL. THAT'S WHY IT LOOKS LIKE SHIT.
   We are purposely avoiding the matrix stack. Those calculations
   aren't necessary for the tiny bit of simple translation this
   rendering needs.

   The Dreamcast handles Z ordering in hardware, so it's faster to render both tile
   and object layers together.
   OpenGL makes us do it, so every layer has to be rendered independently for proper
   Z-ordering.

   AWPTIMIZE
   - no more checking if layer is being rendered MID LOOP
   - Don't look shit up through _curArea anymore
   - Vector instructions for the additions?
   - Check value, not index for empty tiles
   - Your "floating point" additions are probably fucking reested. Since the map can be sparse, you can skip those
     operations most of the time by just calculating per position instead of accumulating.
    */
    //MapSystem::render(); -- Delegated to DepthBuffer now!


    //Debug toggle this shit?
    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::renderOverlay(void) {
        //void DrawString(CAMERA camera, FORMAT format, ALIGNMENT align, const char *str, ...);
        //guiSystem->verdana.setColor(gyro::GREEN);
        /*guiSystem->verdana.setAlpha(1.0f, 1.0f, 1.0f, 1.0f);
        guiSystem->verdana.transform.setPosition(32.0f, 200.0f);
        guiSystem->verdana.DrawString(Font::NO_CAMERA, Font::NO_FORMAT, Font::L_ALIGN, "Render Calls - %d", total);
        guiSystem->verdana.transform.setPosition(32.0f, 220.0f);
        guiSystem->verdana.DrawString(Font::NO_CAMERA, Font::NO_FORMAT, Font::L_ALIGN, "Start - <%d, %d>", startX, startY);
        guiSystem->verdana.transform.setPosition(32.0f, 240.0f);
        guiSystem->verdana.DrawString(Font::NO_CAMERA, Font::NO_FORMAT, Font::L_ALIGN, "End -   <%d, %d>", endX, endY);*/
    }

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::getAnimate(void) const { return _animate; }
    template<typename P, typename T, typename E>
    void MapSystemCrtp<P, T, E>::setAnimate(const bool v) { _animate = v; }

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::toggleRenderLayer(const unsigned layer) {
        return (_renderLayer[layer] = !_renderLayer[layer]);
    }

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::staticInitialize(void) {}

    template<typename P, typename T, typename E>
    inline void MapSystemCrtp<P, T, E>::staticUninitialize(void) {}

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::isShadowPass(void) const { return gyVidShadowPass(); }

    template<typename P, typename T, typename E>
    inline bool MapSystemCrtp<P, T, E>::isDepthPrePass(void) const { return _depthPassState == DEPTH_Z_PREPASS; }

#ifdef NEW_RENDERER
    inline void MapSystemBase::render(void) {



                    _mapCount = 0;
        getOnscreenTileRegion(_scrStartX, _scrStartY, _scrEndX, _scrEndY);
        MapDepthBuffEntryImpl tileEntry;

        //Maximum number of rows depth buffer could need based on visible region
        _depthBuffMaxVisible = _scrEndY - _scrStartY + 1 + ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX;
        //Cap max at depth buffer height
        _depthBuffMaxHeight = (_depthBuffMaxVisible > ELYSIAN_DEPTH_BUFFER_HEIGHT)? ELYSIAN_DEPTH_BUFFER_HEIGHT: _depthBuffMaxVisible;

        //Clear depth buffer counters
        memset(_objectCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        memset(_entityCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
        //_processTiles();

        enableDepthTest();

        //beginObjectList();
        bindTileBuffer();
        for(int e = ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX; e >= 0; --e) {
            for(int y = _scrStartY; y <= _scrEndY; ++y) {
                for(int x = _scrStartX; x <= _scrEndX; ++x) {
                    for(int l = 0; l < ELYSIAN_AREA_MAP_LAYERS; ++l) {
                        if(x < 0 || y < 0) {
                            assert(0);
                            continue;
                        }
                        tileEntry.x = x;
                        tileEntry.y = y;
                        tileEntry.layer = l;
                        getObjectLayerEntry(x, y, tileEntry);
                        tileEntry.renderPass = 1;
                        if(!tileEntry.mapEntry.getIndex()) continue;
                        if(tileEntry.mapEntry.getElev() == e) {
                            if(tileEntry.mapEntry.getPlane() == GY_RECT_PLANE_BOTTOM && e == 0 && isShadowPass()) continue;
                            renderObjectEntry(tileEntry, 0.0f);
                        }
                    }
                }
            }
        }
        gyVidVertexBatchFlush();
        endObjectList();
        gyVidVertexBatchFlush();
        EntityDepthBuffEntryImpl entityEntry;
        NORMAL_TYPE normalType;
        float lowestY;
        float zCoord;
#if 0
        for(unsigned i = 0; i < getEntityCount(); ++i) {
            if(!getOnscreenEntity(i, entityEntry, normalType, lowestY, zCoord)) continue;
            entityEntry.renderPass = 1;
            renderEntityEntry(entityEntry);
                    gyVidVertexBatchFlush();
        }


         for(int e = ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX; e >= 0; --e) {
            if(!isShadowPass()) renderCustomPassAtElev(e, NORMAL_FLAT);
                                 gyVidVertexBatchFlush();
            renderCustomPassAtElev(e, NORMAL_FRONT);
                     gyVidVertexBatchFlush();
         }

#endif
    }

    #endif
}

#endif
