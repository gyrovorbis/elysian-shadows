#ifndef ELYSIAN_MAP_DEPTH_BUFFER_H
#define ELYSIAN_MAP_DEPTH_BUFFER_H

#include <cassert>
#include <cstring>
#include "elysian_area.hpp"
#include "elysian_system.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_light_system.hpp"

//Maximum elevation height an object can have
#define ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX 15
//Objects have integer zCoords, force Entities to floats to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_OBJECT_MIN_Y_OFFSET 0.001f
//Applied to every entity at the same buffer index to prevent duplicate z
#define ELYSIAN_DEPTH_BUFFER_ENTITY_ENTITY_MIN_Z_OFFSET 0.001f
//This needs to be at least as tall as the rendered map area? Not really?
#define ELYSIAN_DEPTH_BUFFER_HEIGHT ELYSIAN_AREA_MAP_HEIGHT
//This needs to be at LEAST as wide as the rendered map area? Not really?
#if GYRO_PLATFORM == GYRO_PLATFORM_DC
#   define ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH 100
#else
#   define ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH ELYSIAN_AREA_MAP_WIDTH
#endif
//Guess this could be a bit smaller? Maybe?
#define ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH 20

namespace elysian {
    
	enum NORMAL_TYPE { NORMAL_FLAT, NORMAL_FRONT };
    
    //Minimal base class for object entries
	struct MapDepthBuffEntry {
		//float x, y;
		unsigned x, y; //tile location
		unsigned layer;
		uint32_t lights;
		TileEntry mapEntry;
		unsigned renderPass = 0;
    };
    
    //Minimal base class for entity entries
    struct EntityDepthBuffEntry {
		float zCoordRelative; //required to sort the bitches
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
	class MapDepthBuffer: public elysian::System {
    private:
        
        //General interface
        void _getOnscreenTileRegion(unsigned &startX, unsigned &startY, unsigned &endX, unsigned &endY) const {
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
        
        void _beginObjectList(void) const {
			static_cast<const T*>(this)->beginObjectList();
        }
        
        void _endObjectList(void) const {
			static_cast<const T*>(this)->endObjectList();
        }

        //Entity interface
		unsigned _getEntityCount(void) const {
			return static_cast<const T*>(this)->getEntityCount();
		}
		
		bool _getOnscreenEntity(const unsigned index, E &entry, NORMAL_TYPE &normalType, float &lowestY, float &zCoord) const {
			return static_cast<const T*>(this)->getOnscreenEntity(index, entry, normalType, lowestY, zCoord);
		}

		void _renderEntityEntry(E &entry) {
			static_cast<T*>(this)->renderEntityEntry(entry);
		}

		void _renderCustomPassAtElev(const unsigned char elev) const {
			static_cast<const T*>(this)->renderCustomPassAtElev(elev);
		}

		void _disableDepthTest(void) const {
			static_cast<const T*>(this)->disableDepthTest();
		}

		void _enableDepthTest(void) const {
			static_cast<const T*>(this)->enableDepthTest();
		}

		//Culling functions
		//Just cull in getObjectLayerEntry and getOnscreenEntity?

		//Bounding volumes for tileZRows
		//Or whatever the shit grouping if you don't want to do that

		//Object buffer
		unsigned	_objectCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
		O			_objectBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
		uint32_t	_objectRowLights[ELYSIAN_DEPTH_BUFFER_HEIGHT];
		O			_objectBuffImmediate[ELYSIAN_DEPTH_BUFFER_HEIGHT*ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
		unsigned	_objectBuffImmediateCount = 0;

		unsigned	_queue[2][ELYSIAN_DEPTH_BUFFER_HEIGHT*ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
		unsigned	_queued = 0;
		int			_nextRow = 0;
		int			_curRow = 0;

		//Entity buffer
		unsigned _entityCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
		E _entityBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH];

		uint32_t _lightsRendered = 0;
		uint32_t _lightsActive	= 0;

		unsigned _scrStartX, _scrStartY, _scrEndX, _scrEndY;
		unsigned _depthBuffMaxVisible;
		unsigned _depthBuffMaxHeight;

		void _processTiles(void);
		void _processEntities(void);
		void _beginRenderNextTileRow(void);
		void _renderDepthBuffTileRow(const unsigned i, const NORMAL_TYPE normalType);
		void _renderDepthBuffEntityRow(const unsigned i);

		bool _tileEntryLightPass(O& tileEntry) {
			++tileEntry.renderPass;
			if((tileEntry.lights | _lightsRendered) == _lightsRendered) { //it's done!
				_renderObjectEntry(tileEntry, ZCoordConstants::mapBase+(tileEntry.mapEntry.getElev())*32.0f /*zCoordBufferedBase+i+1.0f*/); //WILL THIS WORK FOR NO ELEVATION!?
				return true;
			} else if((tileEntry.lights & ~_lightsRendered) == tileEntry.lights) { //can't do dick this pass
				return false;
			} else { //partial render this pass
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
	template<typename T, typename O, typename E>
	inline void MapDepthBuffer<T, O, E>::render(void) {

		_getOnscreenTileRegion(_scrStartX, _scrStartY, _scrEndX, _scrEndY);

		//Maximum number of rows depth buffer could need based on visible region
		_depthBuffMaxVisible = _scrEndY - _scrStartY + 1 + ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX;
		//Cap max at depth buffer height
		_depthBuffMaxHeight = (_depthBuffMaxVisible > ELYSIAN_DEPTH_BUFFER_HEIGHT)? ELYSIAN_DEPTH_BUFFER_HEIGHT: _depthBuffMaxVisible;

		//Clear depth buffer counters
		memset(_objectCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);
		memset(_entityCount, 0, sizeof(unsigned)*_depthBuffMaxHeight);

		_processTiles();

        
        // ==== QUEUE AND IMMEDIATE RENDER ENTITIES ==== 
        
		const float mapStartY = _scrStartY*32.0f;
		const unsigned entityCount = _getEntityCount();
        E entityEntry;
		NORMAL_TYPE normalType;
        float entityBase;
		float zCoord;
        
		//Determine entity Z coord and add to buff
		//If PERFECTLY tile-aligned, add a delta/epsilon to prevent duplicate Zing - or don't bother? Doesn't matter?
		for(unsigned i = 0; i < entityCount; ++i) {
			if(!_getOnscreenEntity(i, entityEntry, normalType, entityBase, zCoord)) continue; //not onscreen

			//FUCKING DISGUSTING, DIRTY, UGLY Z-FIGHTING HACK!!!!
			//float worldZCoord = (ZCoordConstants::mapBase + (elev)*32.0f);//-(((float)entityBase/32.0f)-((int)entityBase/32));

			//calculate relative Z coord
			float relZCoord = (entityBase-mapStartY)/32.0f + zCoord;
			//Add depth buffer entry
			unsigned zIndex = (unsigned)relZCoord;
			if(zIndex >= _depthBuffMaxHeight) zIndex = _depthBuffMaxHeight-1;
			//assert(zIndex >= 0 && zIndex < depthBuffMaxHeight);

			_entityBuff[zIndex][_entityCount[zIndex]] = entityEntry;
			_entityBuff[zIndex][_entityCount[zIndex]].zCoordRelative = relZCoord;

			//render immediately
			if(zCoord < 32.0f && normalType == NORMAL_FLAT) _renderEntityEntry(_entityBuff[zIndex][_entityCount[zIndex]]);
			//queue and render later
			else ++_entityCount[zIndex];

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
        
		// ==== RENDER ENTITIES AND TILES QUEUED IN DEPTH BUFFER ====


		_disableDepthTest();
		//Iterate through depth buffers and render from lowest to highest z coord
		for(unsigned i = 0; i < _depthBuffMaxHeight; ++i) {

			_beginRenderNextTileRow();

			_renderDepthBuffTileRow(i, NORMAL_FLAT);

			_renderCustomPassAtElev(i);

			//Entities come first, since they are always above the lowest point on the object
			float entityZDelta = 0.0f;
			for(unsigned j = 0; j < _entityCount[i]; ++j) {
				//_entityBuff[i][j].zCoord += entityZDelta;
				_renderEntityEntry(_entityBuff[i][j]);
				//Give each entity a tiny delta to prevent duplicate z from same y coord
				entityZDelta += ELYSIAN_DEPTH_BUFFER_ENTITY_ENTITY_MIN_Z_OFFSET;
			}

			_renderDepthBuffTileRow(i, NORMAL_FRONT);
                      
		}
		_enableDepthTest();
	}

	// ==== QUEUE AND IMMEDIATE RENDER TILES ====
	template<typename T, typename O, typename E>
	inline void MapDepthBuffer<T, O, E>::_processTiles(void) {
		LightSystem*const	lightSys			= getLightSys();
		uint32_t			lightsLeft			= 0;
		const uint32_t		onscreenLights		= lightSys->getLightCount();
		_lightsRendered = 0;

		_objectBuffImmediateCount = 0;

		for(unsigned i = 0; i < onscreenLights; ++i) lightsLeft |= (1<<i);

		_beginObjectList();

		_lightsRendered |= _lightsActive = lightSys->startLightPass(lightsLeft);
		for(unsigned j = _scrStartY; j <= _scrEndY; ++j) {
			for(unsigned i = _scrStartX; i <= _scrEndX; ++i) {
				for(unsigned k = 0; k < 3; ++k) {
					O curMapEntry;
					curMapEntry.x = i;
					curMapEntry.y = j;
					curMapEntry.layer = k;
					_getObjectLayerEntry(i, j, curMapEntry);
					/* Had to move this check to client code, because the Toolkit actually does
					render tile 0 decorations when selecting/selected/filling. It's moleculary
					slower, but it's at least a clean solution.
					*/
					//if(!curMapEntry.mapEntry.getValue()) continue; //not the blank object
					int elevation = curMapEntry.mapEntry.getElev();//-1;

					if(elevation == 0 && !curMapEntry.mapEntry.getNormal()) { //render tile immediately at elevation -1 (below entities)
						//Immediate render, or buffer if it requires multiple passes
						_renderOrBufferImmediateTileEntry(curMapEntry);
					} else { //Add to depth buffer if elevated
						unsigned zCoord = j-_scrStartY+elevation;
						//if(zCoord >= depthBuffMaxHeight) zCoord = depthBuffMaxHeight-1;
						assert(zCoord < _depthBuffMaxHeight);

						_objectBuff[zCoord][_objectCount[zCoord]] = curMapEntry;
						_objectRowLights[zCoord] |= curMapEntry.lights;
						++_objectCount[zCoord];
					}
				}
			}
		}

		if(!_objectBuffImmediateCount) return;

		//Make a pass through immediate tile multiplass buffer
		_lightsRendered |= _lightsActive = lightSys->startLightPass(lightsLeft);
		for(unsigned j = 0; j < _objectBuffImmediateCount; ++j) {
			_renderOrQueueTileEntry(j, _objectBuffImmediate[j]);
		}

		//Continue making light passes until there are no more immediate tiles queued
		while(_queued) {
			const unsigned count = _queued;
			_queued = 0;
			_curRow = _nextRow;
			_nextRow = !_curRow;
			_lightsRendered |= _lightsActive = lightSys->startLightPass(lightsLeft);
			for(unsigned j = 0; j < count; ++j) {
				_renderOrQueueTileEntry(j, _objectBuffImmediate[_queue[_curRow][j]]);
			}
		}

		_endObjectList();
	}

	template<typename T, typename O, typename E>
	inline void MapDepthBuffer<T, O, E>::_beginRenderNextTileRow(void) {
		_lightsRendered = 0;
	}

	template<typename T, typename O, typename E>
	inline void MapDepthBuffer<T, O, E>::_renderDepthBuffTileRow(const unsigned int i, const NORMAL_TYPE normalType) {
		//Tiles - rendered with Z at the BOTTOM of the map tile location

		_beginObjectList();

		LightSystem*const	lightSys			= getLightSys();

		_lightsRendered |= _lightsActive = lightSys->startLightPass(_objectRowLights[i]);
		for(unsigned j = 0; j < _objectCount[i]; ++j) {
			if(_objectBuff[i][j].mapEntry.getNormal() == normalType)
				_renderOrQueueTileEntry(j, _objectBuff[i][j]);
		}
		while(_queued) {
			const unsigned count = _queued;
			_queued = 0;
			_curRow = _nextRow;
			_nextRow = !_curRow;
			_lightsRendered |= _lightsActive = lightSys->startLightPass(_objectRowLights[i]);
			for(unsigned j = 0; j < count; ++j) {
				_renderOrQueueTileEntry(j, _objectBuff[i][_queue[_curRow][j]]);
			}
		}

		_endObjectList();

	}
    
}


#endif
