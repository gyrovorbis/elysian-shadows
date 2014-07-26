#include "elysian_light_system.hpp"
#include "elysian_light_source.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_level.hpp"
#include "elysian_map_system.hpp"
#include <gyro_shader_api.h>
#include <cfloat>

namespace elysian {

LightSystem::LightSystem(void) {
	setGlobalAmbientColor(_globalAmbience);
	//gyVidLightingSetEnable(1);

    auto& proc = get<SUBPROC_ATTRIB_INTERP_POOL> ( static_cast<SubProcessManager<LightSystem,
                                              LightAttribInterpPool>& >(*this));

    proc.start(this);
}


void LightSystem::_addToLightMap(LightSource*const src) {
	CameraState*const		 lightCam	= src->getCameraState();
	const uint32_t			lightMask	= (1<<(_onscreenLightCount-1));

	for(unsigned y = lightCam->tileStartY; y <= lightCam->tileEndY; ++y) {
		for(unsigned x = lightCam->tileStartX; x <= lightCam->tileEndX; ++x) {
			const unsigned xIndex = x-_screenTileXOffset;
			const unsigned yIndex = y-_screenTileYOffset;
			_lightMap[yIndex][xIndex] |= lightMask;
		}
	}
}

unsigned LightSystem::getLightCount(void) const { return (_shadowPass)? 0 :  _curArea->lightSources.size(); }

void LightSystem::update(void) {
    const float deltaTime = getTimerSys()->getDeltaTime();
    updateSubProcesses(deltaTime);

	const auto sources = _curArea->lightSources;
	for(auto it : sources) {
		it->update();
	}
}

void LightSystem::setGlobalAmbientColor(const GYVector4 &color) {
	_globalAmbience = color;
	gyVidSetGlobalAmbience(&color);
}

void LightSystem::setEnabled(const bool val) {
	_enabled = val;
	gyVidLightingSetEnable(val);
}


static void renderCube(float x, float y, float z,
					   float sx, float sy, float sz,
					   float /*rotz*/) {
	static const float verts[17*4] = {
		-1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		-1, -1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		-1, -1, -1, 1.0f,
		1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		 1,  1, -1, 1.0f,
		 -1,  1,  1, 1.0f,
		 1,  1,  1, 1.0f,
		 1, -1,  1, 1.0f,
		 1,  1, -1, 1.0f,
		 1, -1, -1, 1.0f,
		-1, -1, -1, 1.0f,
		-1,  1, -1, 1.0f,
		-1, -1,  1, 1.0f,
		-1,  1,  1, 1.0f
	};

	static const GYColor4 colors[17] = {
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 0, 255, 255 },
		{ 255, 255, 0, 255 }
	};

	//GYVector4 modelViewVertices[17];

	//gyMatMultVec((GYVector4*)verts, modelViewVertices, 17);


	gyMatPush();
	//gyMatLoadIdentity();
	gyMatTranslate(x, y, z);
	gyMatScale(sx, sy, sz);
	//gyMatRotateZ(rotz);
	gyVidColorArray(colors);
	gyVidTriangleStrip((GYVector4*)verts, 0, 17);

	//glDrawArrays(GL_TRIANGLES, 0, 12*3);

	gyMatPop();
}



void LightSystem::_renderShadowVolumes(void) {

	gyVidTexBind(nullptr);
	gyVidMaterialBind(nullptr);
	gyVidNormalMapBind(nullptr);
/*
	renderCube(350.0f, 150.0f, 100.0f,
			   25.0f, 25.0f, 20.0f,
			   0.0f);

	renderCube(200.0f, 200.0f, 200.0f,
			   25.0f, 25.0f, 40.0f,
			   3.0f);

	renderCube(400.0f, 300.0f, 100.0f,
			   25.0f, 25.0f, 20.0f,
			   2.14f);

	renderCube(200.0f, 400.0f, 300.0f,
			   50.0f, 50.0f, 60.0f,
			   0.0f);

	renderCube(400.0f, 400.0f, 200.0f,
			   25.0f, 25.0f, 100.0f,
			   0.0f);

	renderCube(350.0f+600.0f, 150.0f+600.0f, 20.0f,
			   25.0f, 25.0f, 10.0f,
			   0.0f);

	renderCube(400.0f+600.0f, 300.0f+600.0f, 20.0f,
			   25.0f, 25.0f, 10.0f,
			   2.14f);

	renderCube(150.0f+600.0f, 750.0f, 60.0f,
			   25.0f, 25.0f, 50.0f,
			   0.0f);

	renderCube(400.0f+600.0f, 400.0f+600.0f, 100.0f,
			   25.0f, 25.0f, 50.0f,
			   0.0f);
*/
	const auto entities = _curArea->entities;
	for(auto it : entities) {
		if(it->isShadowVolume()) it->renderShadowVolume();
	}
}


void LightSystem::render(void) {
	if(!_renderDebug) return;
	_renderShadowVolumes();

}

void LightSystem::shadowRenderPasses(void) {

	const auto sources	= _curArea->lightSources;
	CameraSystem* camSys = getCameraSys();
	CameraState* cam	= camSys->getCurrentState();
	_screenTileXOffset	= cam->tileStartX;
	_screenTileYOffset	= cam->tileStartY;

	_onscreenLightCount = 0;
	memset(_lightMap, 0, sizeof(_lightMap));

	_shadowPass = true;
	gyVidLightSourceBeginShadowPasses();
	camSys->pushState();

	for(auto it : sources) {
		//if(!ONSCREEN) continue;!!!!
		if(_onscreenLightCount >= ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN) continue;

		switch(it->getLightType()) {
		case GY_LIGHT_SPOT:
		//	if(!cam->isWithinScreenRegion(it->position.x, it->position.y, it->getAttrib().getRadius(), it->getAttrib().getRadius())) break;
		case GY_LIGHT_DIRECTIONAL:
			_onscreenLights[_onscreenLightCount++] = it;
			it->updateBoundingVolume(cam, GY_CUBE_MAP_FACE_MAX);

			if(it->isShadowCaster()) {
				gyVidLightSourceBeginShadowPass(it, GY_CUBE_MAP_FACE_MAX);
				getCameraSys()->setCurrentCameraState(it->getCameraState());
				//_renderShadowVolumes();
				getMapSys()->render();
            }
			_addToLightMap(it);
			break;
		case GY_LIGHT_POINT:
			//if(cam->isWithinScreenRegion(it->position.x, it->position.y, it->getAttrib().getRadius(), it->getAttrib().getRadius())) {
				_onscreenLights[_onscreenLightCount++] = it;

					for(unsigned i = GY_CUBE_MAP_RIGHT; i < GY_CUBE_MAP_FACE_MAX; ++i) {
						it->updateBoundingVolume(cam, (GY_CUBE_MAP_FACE)i);

						if(it->isShadowCaster()) {
							gyVidLightSourceBeginShadowPass(it, (GY_CUBE_MAP_FACE)i);

							it->getCameraState()->updateBoundingSubregions(*cam);

							getCameraSys()->setCurrentCameraState(it->getCameraState());
							//_renderShadowVolumes();
							getMapSys()->render();

						}
					}
					_addToLightMap(it);

			//}
			break;
		}
	}

	camSys->popState();
	gyVidLightSourceEndShadowPasses();
	gyVidSetGlobalAmbience(&_globalAmbience);
	_shadowPass = false;
	//_sortOnscreenLights();
}

void LightSystem::addLightSourcesToScene(void) {
	const auto sources = _curArea->lightSources;
	gyVidLightSourceClear();
	for(auto it : sources) {
		gyVidLightSourceAdd(it);
	}
}

void LightSystem::removeLightSourcesFromScene(void) {
	const static GYVector4 fullAmbience = GYVector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	gyVidSetGlobalAmbience(&fullAmbience);
	_activeLights = 0;
	_activeLightCount = 0;
	_shaderUniformChangesLastFrame = _shaderUniformChangesThisFrame;
	_shaderUniformChangesThisFrame = 0;
	gyVidLightSourceClear();

}


void LightSystem::getLightPassTileRegion(unsigned& startX, unsigned& startY, unsigned& endX, unsigned& endY) const {
	startX	= ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_WIDTH;
	endX	= 0;
	startY	= ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_HEIGHT;
	endY	= 0;

	uint32_t lights		= _activeLights;

	for(unsigned i = 0; i < _onscreenLightCount; ++i) {
		if(!(lights & (1<<i))) continue;
		const CameraState*const cam = _onscreenLights[i]->getCameraState();

		if(cam->tileStartX < startX)	startX = cam->tileStartX;
		if(cam->tileEndX > endX)		endX = cam->tileEndX;
		if(cam->tileStartY < startY)	startY = cam->tileStartY;
		if(cam->tileEndY < endY)		endY = cam->tileEndY;

	}
}

uint32_t LightSystem::startLightPass(uint32_t& lightsLeft) {
	//Number of enabled bits in a mask
	auto bitCount = [&](const uint32_t mask) {
		unsigned count = 0;
		for(unsigned i = 0; i < _onscreenLightCount; ++i)
			if(mask&(1<<i)) ++count;
		return count;
	};
	//Retrieves and removes bit position of first active bit
	auto pullFirstIndexFromMask = [&](uint32_t& mask) {
		for(int i = 0; i < (int)_onscreenLightCount; ++i) {
			if(mask&(1<<i)) {
				mask &= ~(1<<i);
				return i;
			}
		}
		return 0;
	};
	//Converts index (bit position) to bitmask
	auto indexToMask = [](const unsigned index) {
		const uint32_t mask = (1<<index);
		return mask;
	};

	//Lights we can keep from previous pass
	const uint32_t	keepActiveLights		= (lightsLeft&_activeLights);
	const unsigned	keepActiveLightCount	= bitCount(keepActiveLights);
	//New lights we can potentially add
	uint32_t		addLights				= lightsLeft^keepActiveLights;
	unsigned		addLightsCount			= bitCount(addLights);
	//Lights we're removing
	uint32_t		removedLights			= (_activeLights&(~keepActiveLights));
	unsigned		removedLightsCount		= bitCount(removedLights);

	if(!lightsLeft) return 0;

	//If we're either adding or removing lights, state change
	if(addLightsCount != 0 /*|| removedLightsCount != 0 || keepActiveLightCount != GYRO_VIDEO_LIGHT_SOURCES_MAX*/) {
		//New values we're calculating
		uint32_t		newActiveLights = keepActiveLights;
		unsigned		newActiveLightCount = keepActiveLightCount;

		//Number of new lights we can actually add to this pass
		if(addLightsCount > GYRO_VIDEO_LIGHT_SOURCES_MAX) addLightsCount = GYRO_VIDEO_LIGHT_SOURCES_MAX;
		const int roomLeft = GYRO_VIDEO_LIGHT_SOURCES_MAX - keepActiveLightCount;
		if(roomLeft) {
			if(roomLeft < (int)addLightsCount) addLightsCount = roomLeft;
		} else {
			addLightsCount = 0;
		}

		//if(removedLightsCount > addLightsCount) removedLightsCount = addLightsCount;

		//Iterate over lights we're removing
		for(unsigned i = 0; i < removedLightsCount; ++i) {
			LightSource*const removedLight = _onscreenLights[pullFirstIndexFromMask(removedLights)];

			//Replace instead of remove, if we have a light to add
			if(addLightsCount > 0) {
				const unsigned addedIndex = pullFirstIndexFromMask(addLights);
				gyVidLightSourceReplace(removedLight, _onscreenLights[addedIndex]);
				newActiveLights |= indexToMask(addedIndex);
				++newActiveLightCount;
				--addLightsCount;
				++_shaderUniformChangesThisFrame;
			} else { //Remove
				gyVidLightSourceRemove(removedLight);
				++_shaderUniformChangesThisFrame;
			}
		}

		//Lights to add to this pass
		for(unsigned i = addLightsCount; i > 0; --i) {
			if(newActiveLightCount == GYRO_VIDEO_LIGHT_SOURCES_MAX) break;
			const unsigned addedIndex = pullFirstIndexFromMask(addLights);
			gyVidLightSourceAdd(_onscreenLights[addedIndex]);
			newActiveLights |= indexToMask(addedIndex);
			++newActiveLightCount;
			++_shaderUniformChangesThisFrame;
		}

		//Update pass state info
		_activeLights = newActiveLights;
		_activeLightCount = newActiveLightCount;

	}

	//Remove current pass from mask representing lights that still need to be rendered
	lightsLeft &= ~_activeLights;

	return _activeLights;

}

void LightSystem::_sortOnscreenLights(void) {
	bool used[ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN] = { false };
	LightSource* lights[ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN];
	unsigned sortedCount = 0;

	//don't bother if we're only doing a single pass
	if(_onscreenLightCount < GYRO_VIDEO_LIGHT_SOURCES_MAX) return;

	memcpy(lights, _onscreenLights, sizeof(lights));

	/* Since the first render pass for non-buffered tiles covers the entire map,
	 * regardless of lights, we can get more out of the algorithm by grouping the
	 * largest lights in the first pass.
	 *
	 * Since subsequent passes are made by combining bounding box geometry into a
	 * large bounding box, we can also avoid overdraw by grouping close light
	 * sources contigouously.
	 */

	//Directional lights take up entire viewport, start with those bitches.
	for(unsigned i = 0; i < _onscreenLightCount; ++i) {
		if((int)lights[i]->getType() == (int)GY_LIGHT_DIRECTIONAL) {
			_onscreenLights[sortedCount++] = lights[i];
			used[i] = true;
			++sortedCount;
		}
	}

	int bigShitNeeded = GYRO_VIDEO_LIGHT_SOURCES_MAX - sortedCount;

	//Throw big shit into first pass!
	//Could be done in a single iteration...
	while(bigShitNeeded > 0) {
		unsigned largestArea = 0;
		unsigned largestIndex = 0;

		for(unsigned i = 0; i < _onscreenLightCount; ++i) {
			if(used[i]) continue;
			const CameraState* const cam = lights[i]->getCameraState();
			const unsigned area = (cam->tileEndX-cam->tileStartX)*(cam->tileEndY-cam->tileStartY);
			if(area > largestArea) {
				largestArea = area;
				largestIndex = i;
			}
		}

		used[largestIndex] = true;
		_onscreenLights[sortedCount++] = lights[largestIndex];
		--bigShitNeeded;
	}

	int leftOverLights = _onscreenLightCount - sortedCount;

	//Try to group close shit together
	GYVector2 pivot = { 0.0f, 0.0f };
	while(leftOverLights > 0) {
		float minDist = FLT_MAX; //~0.0f
		unsigned minIndex = 0;

		for(unsigned i = 0; i < _onscreenLightCount; ++i) {
			if(used[i]) continue;
			GYVector2 distVec;
			const GYVector4& pos4 = lights[i]->getPosition();

			gyVec2Sub(&distVec, &pivot, (GYVector2*)&pos4);
			float dist = gyVec2Magnitude(&distVec);

			if(dist < minDist) {
				minDist = dist;
				minIndex = i;
			}
		}

		used[minIndex] = true;
		_onscreenLights[sortedCount++] = lights[minIndex];
		--leftOverLights;
		pivot.x = lights[minIndex]->getPosition().x;
		pivot.y = lights[minIndex]->getPosition().y;
	}
}

}

