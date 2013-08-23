#ifndef ELYSIAN_TERRAIN_SYSTEM_HPP
#define ELYSIAN_TERRAIN_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_entity.hpp"
#include "elysian_level.hpp"
#include "elysian_zcoords.hpp"
#include <gyro.h>
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
	
	struct GammaEntityDepthBuffEntry: public EntityDepthBuffEntry {
		Entity *entity;
	};
    
	class MapSystem: 
		public System, 
		public MapDepthBuffer<MapSystem, MapDepthBuffEntry, GammaEntityDepthBuffEntry> 
	{ 
	private:
		static const GYColor4 _tileColor;
		bool _renderLayer[4];
        //animation
		//toggles
		bool _animate;
        //stats
        unsigned _renderCount;
        
		void updateAnimation(void);

		//Represents the current animation state of every Tile
		struct AnimTile {
			float elapsedTime;
			uint16_t curIndex;
		} _animTiles[ELYSIAN_TILESHEET_TILES];
       
	public:
		MapSystem(void);
        
		void update(void);
		void renderOverlay(void);
		void resetAnimation(void);
		//void dbgDump(void);

		//=== MapDepthBuffer Implementation ===

        void getOnscreenTileRegion(unsigned &startX, unsigned &startY, unsigned &endX, unsigned &endY) const;
        float getBaseZCoord(void) const;
		void getObjectLayerEntry(const unsigned x, const unsigned y, MapDepthBuffEntry &entry) const;
        void renderObjectEntry(const MapDepthBuffEntry &entry, const float z) const;
        void beginObjectList(void) const;
        void endObjectList(void) const;
		unsigned getEntityCount(void) const;
        bool getOnscreenEntity(const unsigned index, GammaEntityDepthBuffEntry &entry, DEPTH_TYPE &type, float &lowestY, unsigned char &elev) const;
        void renderEntityEntry(const GammaEntityDepthBuffEntry &entry) const;


		//=== /MapDepthBuffer Implementation ===

		//Toggles
		bool toggleRenderLayer(const unsigned layer);
		bool getAnimate(void) const;
		void setAnimate(const bool v);
		bool getRenderTileLayer(const unsigned short int layer) const;
		void setRenderTileLayer(const unsigned short int layer, const bool v);
        unsigned getRenderCount(void) const { return _renderCount; }

	};




//-----INLINEZ-----

	
	inline void MapSystem::getOnscreenTileRegion(unsigned &startX, unsigned &startY, unsigned &endX, unsigned &endY) const {
		getCameraSys()->getVisibleTileRegion(startX, startY, endX, endY);
	}
	
	inline float MapSystem::getBaseZCoord(void) const {
		return ZCoordConstants::object1;
	}
	
	inline void MapSystem::getObjectLayerEntry(const unsigned x, const unsigned y, MapDepthBuffEntry &entry) const {
		entry.mapEntry = _curArea->getTileEntry(entry.layer, entry.x, entry.y);
	}
	
	inline void MapSystem::renderObjectEntry(const MapDepthBuffEntry &entry, const float z) const {
		if(!entry.mapEntry.value) return; //Do not render tile 0, even if the depth buff algorithm tries to.
		if(!_renderLayer[entry.layer]) return; //Don't render if this layer has been toggled off.

		gyMatPush();
		gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z, 32.0f, 32.0f, 1.0f);
		gyVidRectTexOrient(entry.mapEntry.getOrient());
		gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		gyMatPop();
	}
	
	inline void MapSystem::beginObjectList(void) const {
		gyVidTexBind(_curLevel->tileSheet);
		gyVidQuadListBegin(&_tileColor);
	}
	
	inline void MapSystem::endObjectList(void) const {
		gyVidQuadListEnd();
	}
	
	inline unsigned MapSystem::getEntityCount(void) const {
		return _curArea->entities.size();
	}
	
	inline bool MapSystem::getOnscreenEntity(const unsigned index, GammaEntityDepthBuffEntry &entry, DEPTH_TYPE &type, float &lowestY, unsigned char &elev) const {
		Entity &entity = *(_curArea->entities)[index];
		if(getCameraSys()->isOnscreen(entity)) {
			entry.entity = &entity;
			type = entity.getDepthType();
			lowestY = entity.getDepthBaseY();
			elev = entity.getElev();
			return true;
		}
		return false;
	}
	
	inline void MapSystem::renderEntityEntry(const GammaEntityDepthBuffEntry &entry) const {
		entry.entity->renderAtZ(entry.zCoord);
	}

}

#endif