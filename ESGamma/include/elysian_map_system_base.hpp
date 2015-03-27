#ifndef ELYSIAN_MAP_SYSTEM_BASE_HPP
#define ELYSIAN_MAP_SYSTEM_BASE_HPP

#include "glal/gyro_glal.h"
#include "elysian_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_renderable.hpp"
#include "elysian_particle_system.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_entity.hpp"
#include "elysian_light_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>

#ifdef GYRO_GLAL
#	include <glal/gyro_shaders_glal.h>
#   include <glal/gyro_video_glal.h>

#endif

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

ELYSIAN_ASSET_TYPEDEF(MapSystem)
//ELYSIAN_ASSET_TYPEDEF(MapDepthBuffEntryImpl, struct)
//ELYSIAN_ASSET_TYPEDEF(EntityDepthBuffEntryImpl, struct)

namespace elysian {

	struct Tile;
	struct Object;
	
    struct MapDepthBuffEntryImplBase: public MapDepthBuffEntry {};

    struct EntityDepthBuffEntryImplBase: public EntityDepthBuffEntry {
		Entity *entity;
	};

    template<typename P=MapSystem, typename T=MapDepthBuffEntryImplBase, typename E=EntityDepthBuffEntryImplBase>
    class MapSystemBase:
		public System, 
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
        unsigned _renderCount;
        bool    _blendPass = false;
        
		void updateAnimation(void);

		//Represents the current animation state of every Tile
		struct AnimTile {
			float elapsedTime;
			uint16_t curIndex;
		} _animTiles[ELYSIAN_TILESHEET_TILES];
       
	public:

        static void staticInitialize(void);
        static void staticUninitialize(void);

                    MapSystemBase(void);
        
        void        update(void);
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

        void        flushTileBuffer(void);
        void        bindTileBuffer(void);
        void        unbindTileBuffer(void);


		//=== /MapDepthBuffer Implementation ===

		//Toggles
        bool        toggleRenderLayer(const unsigned layer);
        bool        getAnimate(void) const;
        void        setAnimate(const bool v);
        bool        getRenderTileLayer(const unsigned short int layer) const;
        void        setRenderTileLayer(const unsigned short int layer, const bool v);
        unsigned    getRenderCount(void) const { return _renderCount; }

	};




//-----INLINEZ-----

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::renderCustomPassImmediate(void) { enableDepthTest(); }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::getOnscreenTileRegion(int &startX, int &startY, int &endX, int &endY) const {
		getCameraSys()->getVisibleTileRegion(startX, startY, endX, endY);
	}
	
    template<typename P, typename T, typename E>
    inline float MapSystemBase<P, T, E>::getBaseZCoord(void) const {
		return ZCoordConstants::mapBase;
	}
	
    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::getObjectLayerEntry(const unsigned /*x*/, const unsigned /*y*/, T &entry) {
		entry.mapEntry = _curArea->getTileEntry(entry.layer, entry.x, entry.y);
		entry.lights = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
        entry.renderPass = 0;
	}
	
    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::renderObjectEntry(const T& entry, const float /*z*/) {
		if(!_renderLayer[entry.layer]) return; //Don't render if this layer has been toggled off.

         auto& elemArraySize = _curArea->elementsArraySize[_curArea->elementsCurSegment];
         auto* elemArray     = &_curArea->elementsArray[_curArea->elementsCurSegment*_curArea->elementsPerSegment];

        if(!gyVidShadowPass()) {
            const GY_RECT_PLANE_TYPE      newPlane = entry.mapEntry.getPlane();
            const GY_RECT_TEX_ORIENT      newOrient = entry.mapEntry.getOrient();
            /*const GY_ALPHA_LEVEL          newAlpha = (entry.renderPass > 1 ||
                                                      _curLevel->tileset[entry.mapEntry.getIndex()].getTransparency())?
                                                        GY_ALPHA_FULL : GY_ALPHA_NONE;*/

            //Blending enabled state change
          /*  if(newAlpha != glalAlphaLevel) {
              flushTileBuffer();
             gyVidAlphaLevel(newAlpha);
            }*/

            //Blending mode state change
            if(entry.renderPass > 1 && !_blendPass) {
                disableDepthTest();
            }
            //We don't give a fuck about blend modes when blending is off
            else if(entry.renderPass == 1 && _blendPass) {
                enableDepthTest();
            }

            //Normal map texture flip state change
            gyVidRectPlaneType(newPlane);
            gyVidRectTexOrient(newOrient);
            const float newZFactor = glalOrientNormalMapZFactor(newOrient);
            if(newZFactor != glalNormalMapZFactor) {
                flushTileBuffer();
                glalVidNormalMapZFactorUpdateUniform(newZFactor);
            }
        }

        const unsigned int indexBase = (entry.layer*getCurArea()->height*getCurArea()->width +
                           entry.y*getCurArea()->width + entry.x)*4;

        const unsigned int indices[4] = { indexBase, indexBase+1,indexBase+2, indexBase+3 };

        //combine previous triangle strip with this one by using degenerate triangles if we're not the first
        if(elemArraySize) {
            elemArray[elemArraySize]		= elemArray[elemArraySize-1];
            elemArray[elemArraySize+1]	= indices[0];
            elemArraySize += 2;
        }

        elemArray[elemArraySize]      = indices[0];
        elemArray[elemArraySize+1]    = indices[1];
        elemArray[elemArraySize+2]    = indices[2];
        elemArray[elemArraySize+3]    = indices[3];
        elemArraySize += 4;

        if(elemArraySize + 6 >= _curArea->elementsPerSegment) {
            flushTileBuffer();
        }
}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::beginObjectList(void) {
       // flushTileBuffer();
        bindTileBuffer();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::bindTileBuffer(void) {
      //  assert(_curArea->elementsArraySize[_curArea->elementsCurSegment] == 0);

        gyVidMaterialBind(nullptr);
		gyVidTexBind(_curLevel->tileSheet);
        gyVidNormalMapBind(_curLevel->normalSheet);
		gyVidSpecularMapBind(_curLevel->specularSheet);

        glBindVertexArray(_curArea->vao[gyVidShadowPass()]);
	}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::endObjectList(void) {
        flushTileBuffer();
        unbindTileBuffer();
    }
	
    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::flushTileBuffer(void) {
        auto& elemArraySize = _curArea->elementsArraySize[_curArea->elementsCurSegment];
        auto* elemArray     = &_curArea->elementsArray[_curArea->elementsCurSegment*_curArea->elementsPerSegment];
        if(elemArraySize == 0) return;


        GLenum result;
        result = glClientWaitSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment], GL_SYNC_FLUSH_COMMANDS_BIT, 100000000);
      while(result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED) {
          result = glClientWaitSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment], 0, 100);
      }

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

        glalVertexRenderCount += elemArraySize;

        //assert(elemArraySize <= maxi);
       // assert(elemArraySize < ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE);

          glDeleteSync(_curArea->elementsArraySyncs[_curArea->elementsCurSegment]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _curArea->elementVbo);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, );
       GLushort* ptr = (GLushort*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, (GLintptr)(_curArea->elementsPerSegment*sizeof(GLushort)*_curArea->elementsCurSegment), sizeof(GLushort)*elemArraySize, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
       memcpy(ptr, elemArray, sizeof(GLushort)*elemArraySize);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

      //  glDrawElements(GL_TRIANGLE_STRIP, elemArraySize, GL_UNSIGNED_SHORT, elemArray /*nullptr*/);


                //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
       // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
        glDrawElements(GL_TRIANGLE_STRIP, elemArraySize, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort)*_curArea->elementsPerSegment*_curArea->elementsCurSegment));

        _curArea->elementsArraySyncs[_curArea->elementsCurSegment] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        //glDrawArrays(GL_TRIANGLE_STRIP, 0, elemArraySize);
        elemArraySize = 0;

        if(++_curArea->elementsCurSegment >= ELYSIAN_MAP_SYSTEM_SEGMENTS_PER_VBO) {
            _curArea->elementsCurSegment = 0;
        }

	}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::unbindTileBuffer(void) {
        gyVidBindDefaultVertexArray();
    }
	
    template<typename P, typename T, typename E>
    inline unsigned MapSystemBase<P, T, E>::getEntityCount(void) {
        if(!_renderLayer[3] || gyVidShadowPass()) return 0;
		_onscreenEntityPool.clear();
		auto& entities = _curArea->entities;

		for(auto entity : entities) {
            if(getLightSys()->isShadowPass() && !entity->isShadowVolume()) {
                continue;
            } //else if(getCameraSys()->isOnscreen(entity)) {
                _onscreenEntityPool.claimFreeEntry() = entity;
            //}
		}
        //Debug::printf(Debug::VERBOSE, "ONSCREEN ENTITIES - %d", _onscreenEntityPool.size());
		return _onscreenEntityPool.size();
	}

    template<typename P, typename T, typename E>
    inline bool MapSystemBase<P, T, E>::getOnscreenEntity(const unsigned index, E &entry, NORMAL_TYPE &normalType, float &lowestY, float& zCoord) const {
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
    inline void MapSystemBase<P, T, E>::renderEntityEntry(const E &entry) {
		//entry.entity->renderAtZ(entry.zCoord);
		//entry.entity->transform.setPosZ(entry.zCoordWorld);

        if(!gyVidShadowPass()) {
            const GY_ALPHA_LEVEL          newAlpha = (entry.renderPass > 1)?
                                                        GY_ALPHA_FULL : GY_ALPHA_PUNCHTHRU;

                //Blending enabled state change
            if(newAlpha != glalAlphaLevel) {
                  glalStripComboBreaker();
                  gyVidAlphaLevel(newAlpha);
            }
            if(entry.renderPass > 1 && !_blendPass) {
                glalStripComboBreaker();
                disableDepthTest();
            } else if(entry.renderPass == 1 && _blendPass) {
                glalStripComboBreaker();
                enableDepthTest();
            }
        }

        entry.entity->render();
	}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::disableDepthTest(void) {
        if(gyVidShadowPass()) return;
        flushTileBuffer();
        gyVidDepthTest(false);
        //glEnable(GL_POLYGON_OFFSET_FILL);
        //glPolygonOffset(2.0f, 2.0f);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);
        gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidBlendMode(GY_BLEND_ONE);
        _blendPass = true;
	}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::enableDepthTest(void) {
        if(gyVidShadowPass()) return;
        flushTileBuffer();
        gyVidDepthTest(false);
        glDepthMask(GL_TRUE);
        //glEnable(GL_BLEND);
        //glDisable(GL_BLEND);
         glDepthFunc(GL_LEQUAL);
         gyVidAlphaLevel(GY_ALPHA_NONE);
        //glDisable(GL_POLYGON_OFFSET_FILL);
        gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
        _blendPass = false;
	}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::renderCustomPassAtElev(const int elev, const NORMAL_TYPE normalType) const {
        if(normalType == NORMAL_FLAT && elev != -1) getPartSys()->renderAtElev(elev);
	}

    template<typename P, typename T, typename E>
    inline uint32_t MapSystemBase<P, T, E>::startLightPass(uint32_t& lightsLeft) { return getLightSys()->startLightPass(lightsLeft); }

    template<typename P, typename T, typename E>
    inline uint32_t MapSystemBase<P, T, E>::getLightCount(void) const { return getLightSys()->getLightCount(); }

    template<typename P, typename T, typename E>
    inline bool MapSystemBase<P, T, E>::getRenderTileLayer(const unsigned short layer) const { return _renderLayer[layer]; }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::setRenderTileLayer(const unsigned short layer, const bool v) { _renderLayer[layer] = v; }



    template<typename P, typename T, typename E>
    const GYColor4      MapSystemBase<P, T, E>::_tileColor = { 255, 255, 255, 255 }; //I s'pose this could be configurable?
/*    template<typename P, typename T, typename E>
    GY_RECT_PLANE_TYPE  MapSystemBase<P, T, E>::_curPlaneType = GY_RECT_PLANE_BOTTOM;
    template<typename P, typename T, typename E>
    GY_RECT_TEX_ORIENT  MapSystemBase<P, T, E>::_curTexOrient = GY_RECT_TEX_ORIENT_NORMAL;
    template<typename P, typename T, typename E>
    TEXTURE_FLIP  MapSystemBase<P, T, E>::_curTexFlip = TEXTURE_FLIP_NONE; */

    template<typename P, typename T, typename E>
    inline MapSystemBase<P, T, E>::MapSystemBase(void):
        _animate(true) {
        for(unsigned i = 0; i < 4; ++i) _renderLayer[i] = true;
        //for(unsigned int i = 0; i < 256; ++i) _currentTile[i] = _currentObject[i] = i;
    }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::update(void) {
        //Update entity elevation
        //Update entity z transitionals

        updateAnimation();
    }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::updateAnimation(void) {
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
                        for(unsigned y = 0; y < getCurArea()->height; ++y) {
                            for(unsigned x = 0; x < getCurArea()->width; ++x) {
                                if(getCurArea()->getTileEntry(l, x, y).getIndex() == i) {
                                    getCurArea()->animateVboTileEntry(l, x, y, curTile.nextIndex);
                                }
                            }
                        }
                    }


                }
            }
        }
    }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::resetAnimation(void) {
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
    inline void MapSystemBase<P, T, E>::renderOverlay(void) {
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
    inline bool MapSystemBase<P, T, E>::getAnimate(void) const { return _animate; }
    template<typename P, typename T, typename E>
    void MapSystemBase<P, T, E>::setAnimate(const bool v) { _animate = v; }

    template<typename P, typename T, typename E>
    inline bool MapSystemBase<P, T, E>::toggleRenderLayer(const unsigned layer) {
        return (_renderLayer[layer] = !_renderLayer[layer]);
    }

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::staticInitialize(void) {}

    template<typename P, typename T, typename E>
    inline void MapSystemBase<P, T, E>::staticUninitialize(void) {}

    template<typename P, typename T, typename E>
    inline bool MapSystemBase<P, T, E>::isShadowPass(void) const { return gyVidShadowPass(); }


}

#endif
