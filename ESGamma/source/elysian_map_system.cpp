#include "elysian_map_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_debug.hpp"
#include <gyro.h>

#include "elysian_level.hpp"
#include "elysian_area.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_font.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_zcoords.hpp"
#include "elysian_entity.hpp"

namespace elysian {

	const GYColor4 MapSystem::_tileColor = { 255, 255, 255, 255 }; //I s'pose this could be configurable?

	MapSystem::MapSystem(void): 
		_animate(true) { 
		for(unsigned i = 0; i < 4; ++i) _renderLayer[i] = true;
		//for(unsigned int i = 0; i < 256; ++i) _currentTile[i] = _currentObject[i] = i;
	}
		

	void MapSystem::update(void) {
		//Update entity elevation
		//Update entity z transitionals

		updateAnimation();
	}

	void MapSystem::updateAnimation(void) {
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
				}
			}
		}
	}

	void MapSystem::resetAnimation(void) {
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
	void MapSystem::renderOverlay(void) {
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

	bool MapSystem::getAnimate(void) const { return _animate; }
	void MapSystem::setAnimate(const bool v) { _animate = v; }

	bool MapSystem::toggleRenderLayer(const unsigned layer) {
		return (_renderLayer[layer] = !_renderLayer[layer]);

	}

//Took animation into account!
#if 0
	Tile *TerrainSystem::getCurrentTile(const unsigned char index) const {
		return(!isValidTile(index))? NULL : &_curLevel->tileset[_currentTile[index]];
	}

	Tile *TerrainSystem::getCurrentObject(const unsigned char index) const {
		return(!isValidObject(index))? NULL : &_curLevel->tileset[_currentObject[index]];
	}
#endif

}
