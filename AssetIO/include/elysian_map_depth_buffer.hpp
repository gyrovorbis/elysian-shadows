#ifndef ELYSIAN_MAP_DEPTH_BUFFER_H
#define ELYSIAN_MAP_DEPTH_BUFFER_H

#include <cassert>
#include <cstring>
#include "elysian_area.hpp"

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
    
    enum DEPTH_TYPE { DEPTH_NONE, DEPTH_BUFFERED };
    
    //Minimal base class for object entries
	struct MapDepthBuffEntry {
        //float x, y;
		unsigned x, y; //tile location
		unsigned layer;
		MapEntry mapEntry;
    };
    
    //Minimal base class for entity entries
    struct EntityDepthBuffEntry {
        float zCoord; //required to sort the bitches
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
		
        bool _getOnscreenEntity(const unsigned index, E &entry, DEPTH_TYPE &type, float &lowestY, unsigned char &elev) const {
            return static_cast<const T*>(this)->getOnscreenEntity(index, entry, type, lowestY, elev);
        }
        
		void _renderEntityEntry(E &entry) {
			static_cast<T*>(this)->renderEntityEntry(entry);
        }
        
        //Object buffer
        unsigned _objectCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
        O _objectBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_OBJECT_WIDTH];
        
        //Entity buffer
        unsigned _entityCount[ELYSIAN_DEPTH_BUFFER_HEIGHT];
        E _entityBuff[ELYSIAN_DEPTH_BUFFER_HEIGHT][ELYSIAN_DEPTH_BUFFER_ENTITY_WIDTH];
        
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

		//Calculate z coordinates
		const float zCoordImmediateObject	= _getBaseZCoord();
		const float zCoordImmediateEntity	= zCoordImmediateObject + 0.5f;
		const float zCoordBufferedBase		= zCoordImmediateObject + 1.0f;
        
		//Calculate depth buffer boundaries
		unsigned startX, startY, endX, endY;
		_getOnscreenTileRegion(startX, startY, endX, endY);
		//Maximum number of rows depth buffer could need based on visible region
		unsigned depthBuffMaxVisible = endY - startY + 1 + ELYSIAN_DEPTH_BUFFER_OBJECT_ELEVATION_MAX;
		//Cap max at depth buffer height
		const unsigned depthBuffMaxHeight = (depthBuffMaxVisible > ELYSIAN_DEPTH_BUFFER_HEIGHT)? ELYSIAN_DEPTH_BUFFER_HEIGHT: depthBuffMaxVisible;

		
		//Clear depth buffer counters
		memset(_objectCount, 0, sizeof(unsigned)*depthBuffMaxHeight);
		memset(_entityCount, 0, sizeof(unsigned)*depthBuffMaxHeight);
        
        
		// ==== QUEUE AND IMMEDIATE RENDER OBJECTS ==== 
		_beginObjectList();
		for(unsigned j = startY; j <= endY; ++j) {
			for(unsigned i = startX; i <= endX; ++i) {
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
					//if(curObjEntry.mapVal) { //not the blank object
					int elevation = curMapEntry.mapEntry.getElev()-1;
                    
					if(elevation == -1) { //render objects immediately at elevation -1 (below entities)
						_renderObjectEntry(curMapEntry, zCoordImmediateObject);
					} else { //Buffer if higher elevation
						unsigned zCoord = j-startY+elevation;
						//if(zCoord >= depthBuffMaxHeight) zCoord = depthBuffMaxHeight-1;
						assert(zCoord < depthBuffMaxHeight);
                        
						_objectBuff[zCoord][_objectCount[zCoord]] = curMapEntry;
						++_objectCount[zCoord];
					}
					//}
				}
			}
		}
		_endObjectList();
        
        // ==== QUEUE AND IMMEDIATE RENDER ENTITIES ==== 
        
        const float mapStartY = startY*32.0f;
		const unsigned entityCount = _getEntityCount();
        E entityEntry;
        DEPTH_TYPE entityDepthType;
        float entityBase;
		unsigned char elev;
        
		//Determine entity Z coord and add to buff
		//If PERFECTLY tile-aligned, add a delta/epsilon to prevent duplicate Zing - or don't bother? Doesn't matter?
		for(unsigned i = 0; i < entityCount; ++i) {
			if(!_getOnscreenEntity(i, entityEntry, entityDepthType, entityBase, elev)) continue; //not onscreen
            
			//FACTOR IN AN ENTITY'S ELEVATION!!!!

            if(entityDepthType == DEPTH_NONE) { //Render immediately
				if(!elev) {
					entityEntry.zCoord = zCoordImmediateEntity;
					_renderEntityEntry(entityEntry);
				} else {
					entityEntry.zCoord = zCoordImmediateEntity + elev;
					_entityBuff[elev][_entityCount[elev]] = entityEntry;
					++_entityCount[elev];
				}    
            } else { //Buffer and render later
                //calculate relative Z coord
                float zCoord = (entityBase-mapStartY)/32.0f + elev;
                    
                //Add depth buffer entry
                unsigned zIndex = (unsigned)zCoord;
				if(zIndex >= depthBuffMaxHeight) zIndex = depthBuffMaxHeight-1;
                //assert(zIndex >= 0 && zIndex < depthBuffMaxHeight);
                
                _entityBuff[zIndex][_entityCount[zIndex]] = entityEntry;
                _entityBuff[zIndex][_entityCount[zIndex]].zCoord = zCoordBufferedBase + zCoord;
                ++_entityCount[zIndex];
			}
		}
        
		//Sort entity z entries per z coord (ascending order)
		//IMPLEMENT ME MORE EFFICIENTLY! BUBBLE SORT IS FUCKING GAY
		for(unsigned j = 0; j < depthBuffMaxHeight; ++j) {
			for(unsigned i = 0; i < _entityCount[j]; ++i) {
				for(unsigned k = i+1; k < _entityCount[j]; ++k) {
					if(_entityBuff[j][i].zCoord > _entityBuff[j][k].zCoord) {
						E temp = _entityBuff[j][i];
						_entityBuff[j][i] = _entityBuff[j][k];
						_entityBuff[j][k] = temp;
					}
				}
			}
		}
        
        // ==== RENDER ENTITIES AND OBJECTS QUEUED IN DEPTH BUFFER ==== 

		//Iterate through depth buffers and render from lowest to highest z coord
		for(unsigned i = 0; i < depthBuffMaxHeight; ++i) {

			//Entities come first, since they are always above the lowest point on the object
			float entityZDelta = 0.0f;
			for(unsigned j = 0; j < _entityCount[i]; ++j) {
				//_entityBuff[i][j].zCoord += entityZDelta;
				_renderEntityEntry(_entityBuff[i][j]);
				//Give each entity a tiny delta to prevent duplicate z from same y coord
				entityZDelta += ELYSIAN_DEPTH_BUFFER_ENTITY_ENTITY_MIN_Z_OFFSET;
			}
                      
			//Objects - rendered with Z at the BOTTOM of the map tile location
			_beginObjectList();
			for(unsigned j = 0; j < _objectCount[i]; ++j) {
				_renderObjectEntry(_objectBuff[i][j], zCoordBufferedBase+i+1.0f);
			}
			_endObjectList();

		}
	}
    
}


#endif
