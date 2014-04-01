#ifndef ELYSIAN_TERRAIN_SYSTEM_HPP
#define ELYSIAN_TERRAIN_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_entity.hpp"
#include "elysian_level.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_renderable.hpp"
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
		void disableDepthTest(void) const;
		void enableDepthTest(void) const;


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
		return ZCoordConstants::mapBase;
	}
	
	inline void MapSystem::getObjectLayerEntry(const unsigned /*x*/, const unsigned /*y*/, MapDepthBuffEntry &entry) const {
		entry.mapEntry = _curArea->getTileEntry(entry.layer, entry.x, entry.y);
		entry.lights = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
	}
	
	inline void MapSystem::renderObjectEntry(const MapDepthBuffEntry &entry, const float z) const {
		if(!entry.mapEntry.value) return; //Do not render tile 0, even if the depth buff algorithm tries to.
		if(!_renderLayer[entry.layer]) return; //Don't render if this layer has been toggled off.
		float delta = 0;//(entry.renderPass);

		gyMatPush();

		gyVidRectTexOrient(entry.mapEntry.getOrient());

	//	if(entry.lightState == MapDepthBuffEntry::LIGHT_STATE::PARTIAL) {
			//glBlendFunc(GL_ONE, GL_ONE);
		//}

		static bool blended = false;

		if(entry.renderPass > 1 && !blended) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			blended = true;
		}
		else if(entry.renderPass == 1 && blended) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			blended = false;
		}

		if(entry.mapEntry.getElev() > 0) {
			gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta-32.0f, 32.0f, 32.0f, 32.0f);
			gyVidRectNormalType(GY_RECT_NORMAL_DEPTH);
					gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		} else {
			gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta, 32.0f, 32.0f, 1.0f);
			gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
					gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		}


		//if(entry.lightState == MapDepthBuffEntry::LIGHT_STATE::PARTIAL)


/*		if(entry.mapEntry.getElev() > 0) {
			gyVidNormalArray(Renderable::depthNormals);
			gyVidQuad(Renderable::depthRectVerts, _animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		} else {
			gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		}*/
		gyMatPop();
	}
	
	inline void MapSystem::beginObjectList(void) const {
		gyVidTexBind(_curLevel->tileSheet);
		gyVidNormalMapBind(_curLevel->normalSheet);
		gyVidSpecularMapBind(_curLevel->specularSheet);
		gyVidQuadListBegin(&_tileColor);
	}
	
	inline void MapSystem::endObjectList(void) const {
		gyVidQuadListEnd();
		gyVidNormalMapBind(nullptr);
		gyVidSpecularMapBind(nullptr);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
		//entry.entity->renderAtZ(entry.zCoord);
		entry.entity->transform.setPosZ(entry.zCoordWorld);
		entry.entity->render();
	}

	inline void MapSystem::disableDepthTest(void) const {
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(10.0f, 10.0f);
	}

	inline void MapSystem::enableDepthTest(void) const {
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_POLYGON_OFFSET_FILL);
	}

}

#endif
