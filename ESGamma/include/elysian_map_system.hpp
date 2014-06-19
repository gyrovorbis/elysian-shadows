#ifndef ELYSIAN_TERRAIN_SYSTEM_HPP
#define ELYSIAN_TERRAIN_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_map_depth_buffer.hpp"
#include "elysian_entity.hpp"
#include "elysian_level.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_renderable.hpp"
#include "elysian_particle_system.hpp"
#include <gyro.h>

#ifdef GYRO_GLAL
#	include <glal/gyro_shaders_glal.h>

#define ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE	100000
#endif
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
		unsigned	_stripIndexArray[ELYSIAN_MAP_SYSTEM_GLAL_STRIP_INDEX_ARRAY_SIZE];
		unsigned	_stripIndexArraySize = 0;
		static const GYColor4 _tileColor;
        static GY_RECT_NORMAL_TYPE _curNormalType;
		bool _renderLayer[4];
        bool _blendFunc = false;
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
		void getObjectLayerEntry(const unsigned x, const unsigned y, MapDepthBuffEntry &entry);
		void renderObjectEntry(const MapDepthBuffEntry &entry, const float z);
		void beginObjectList(void);
		void endObjectList(void);
		unsigned getEntityCount(void) const;
		bool getOnscreenEntity(const unsigned index, GammaEntityDepthBuffEntry &entry, NORMAL_TYPE &normalType, float &lowestY, float &zCoord) const;
        void renderEntityEntry(const GammaEntityDepthBuffEntry &entry) const;
		void disableDepthTest(void) const;
		void enableDepthTest(void) const;
		void renderCustomPassAtElev(const unsigned char elev) const;


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
	
	inline void MapSystem::getObjectLayerEntry(const unsigned /*x*/, const unsigned /*y*/, MapDepthBuffEntry &entry) {
		entry.mapEntry = _curArea->getTileEntry(entry.layer, entry.x, entry.y);
		entry.lights = getLightSys()->getLightMapEntryMapCoord(entry.y, entry.x);
	}
	
	inline void MapSystem::renderObjectEntry(const MapDepthBuffEntry &entry, const float z) {
		if(!entry.mapEntry.value) return; //Do not render tile 0, even if the depth buff algorithm tries to.
		if(!_renderLayer[entry.layer]) return; //Don't render if this layer has been toggled off.
		float delta = 0;//(entry.renderPass);

		//gyMatPush();

		//gyVidRectTexOrient(entry.mapEntry.getOrient());

	//	if(entry.lightState == MapDepthBuffEntry::LIGHT_STATE::PARTIAL) {
			//glBlendFunc(GL_ONE, GL_ONE);
		//}

        if(entry.renderPass > 1 && !_blendFunc) {
			endObjectList();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            _blendFunc = true;
		}
        else if(entry.renderPass == 1 && _blendFunc) {
			endObjectList();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            _blendFunc = false;
		}
#if 1
		GY_RECT_NORMAL_TYPE newNormal = entry.mapEntry.getNormal() ? GY_RECT_NORMAL_FLAT : GY_RECT_NORMAL_FRONT;

        if(newNormal != _curNormalType) {
			endObjectList();
			//gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta, 32.0f, 32.0f, 32.0f);
			gyVidRectNormalType(newNormal);
            _curNormalType = newNormal;
		}
		/*} else {
		//	gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta, 32.0f, 32.0f, 1.0f);
			gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
		}*/

		/*glBindBuffer(GL_ARRAY_BUFFER, getCurArea()->vboBuff);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(VboEntry), 0);
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_POS]);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(VboEntry), (void*)(4*sizeof(float)));
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_TEX_COORD]);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VboEntry), (void*)(6*sizeof(float)));
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_COLOR]);*/

		const unsigned int indexBase = (entry.layer*getCurArea()->height*getCurArea()->width +
						   entry.y*getCurArea()->width + entry.x)*4;

		unsigned int indices[4] = { indexBase, indexBase+1,indexBase+2, indexBase+3};

		if(_stripIndexArraySize) {
			_stripIndexArray[_stripIndexArraySize]		= _stripIndexArray[_stripIndexArraySize-1];
			_stripIndexArray[_stripIndexArraySize+1]	= indices[0];
			_stripIndexArraySize += 2;
		}

		for(unsigned i = 0; i < 4; ++i) _stripIndexArray[_stripIndexArraySize+i] = indices[i];

		_stripIndexArraySize += 4;

		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)indexes);

		//glDrawArrays(GL_TRIANGLE_STRIP, indexBase, 4);

		//gyVidRect(_animTiles[entry.mapEntry.getIndex()].curIndex);
#else
		if(entry.mapEntry.getElev() > 0) {
			gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta-32.0f, 32.0f, 32.0f, 32.0f);
			gyVidRectNormalType(GY_RECT_NORMAL_FRONT);
					gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		} else {
			gyMatTranslateScale(entry.x*32.0f, entry.y*32.0f, z+delta, 32.0f, 32.0f, 1.0f);
			gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
					gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		}
#endif

		//if(entry.lightState == MapDepthBuffEntry::LIGHT_STATE::PARTIAL)


/*		if(entry.mapEntry.getElev() > 0) {
			gyVidNormalArray(Renderable::depthNormals);
			gyVidQuad(Renderable::depthRectVerts, _animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		} else {
			gyVidRect(_animTiles[((TileEntry *)&entry.mapEntry)->getIndex()].curIndex);
		}*/
	//	gyMatPop();
	}
	
	inline void MapSystem::beginObjectList(void) {
		gyVidMaterialBind(nullptr);
		gyVidTexBind(_curLevel->tileSheet);
		gyVidNormalMapBind(_curLevel->normalSheet);
		gyVidSpecularMapBind(_curLevel->specularSheet);
        gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
        _curNormalType = GY_RECT_NORMAL_FLAT;
		//gyVidQuadListBegin(&_tileColor);
		glBindBuffer(GL_ARRAY_BUFFER, getCurArea()->vboBuff);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(VboEntry), 0);
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_POS]);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(VboEntry), (void*)(4*sizeof(float)));
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_TEX_COORD]);
		glVertexAttribPointer(curLightShader->attribLoc[SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VboEntry), (void*)(6*sizeof(float)));
		glEnableVertexAttribArray(curLightShader->attribLoc[SHADER_COLOR]);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _blendFunc = false;
	}
	
	inline void MapSystem::endObjectList(void) {

		glDrawElements(GL_TRIANGLE_STRIP, _stripIndexArraySize, GL_UNSIGNED_INT, (void*)_stripIndexArray);
		_stripIndexArraySize = 0;
		//glDrawArrays(GL_TRIANGLE_STRIP, indexBase, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//gyVidQuadListEnd();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _blendFunc = false;
	}
	
	inline unsigned MapSystem::getEntityCount(void) const {
		return _curArea->entities.size();
	}

	inline bool MapSystem::getOnscreenEntity(const unsigned index, GammaEntityDepthBuffEntry &entry, NORMAL_TYPE &normalType, float &lowestY, float& zCoord) const {
		Entity &entity = *(_curArea->entities)[index];
		if(getCameraSys()->isOnscreen(entity)) {
			entry.entity = &entity;
			normalType = entity.getNormalType();
			lowestY = entity.getDepthBaseY();
            zCoord = entity.transform._pos.z;
			return true;
		}
		return false;
	}
	
	inline void MapSystem::renderEntityEntry(const GammaEntityDepthBuffEntry &entry) const {
		//entry.entity->renderAtZ(entry.zCoord);
		//entry.entity->transform.setPosZ(entry.zCoordWorld);
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

	inline void MapSystem::renderCustomPassAtElev(const unsigned char elev) const {
		getPartSys()->renderAtElev(elev);
	}

}

#endif
