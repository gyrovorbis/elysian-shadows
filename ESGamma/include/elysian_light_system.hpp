#ifndef ELYSIAN_LIGHT_SYSTEM_H
#define ELYSIAN_LIGHT_SYSTEM_H

#include "elysian_system.hpp"
#include "elysian_area.hpp"
#include <gyro_matrix_api.h>

#define ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN		32
#define ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_WIDTH			ELYSIAN_AREA_MAP_WIDTH
#define ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_HEIGHT			ELYSIAN_AREA_MAP_HEIGHT

namespace elysian {

class LightSource;
struct CameraState;

class LightSystem: public System {
	bool			_shadowPass		= false;
	bool			_enabled		= true;
	bool			_renderDebug	= false;
	GYVector4		_globalAmbience = { 0.0f, 0.0f, 0.0f, 1.0f };
	unsigned		_onscreenLightCount	= 0;
	LightSource*	_onscreenLights[ELYSIAN_LIGHT_SYSTEM_MAX_LIGHTS_ONSCREEN] = { nullptr };
    uint32_t		_lightMap[ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_HEIGHT][ELYSIAN_LIGHT_SYSTEM_LIGHT_MAP_WIDTH] = { {0} };
	unsigned		_screenTileXOffset	= 0;
	unsigned		_screenTileYOffset	= 0;
	uint32_t		_activeLights		= 0;
	unsigned		_activeLightCount	= 0;

	unsigned		_shaderUniformChangesThisFrame = 0;
	unsigned		_shaderUniformChangesLastFrame = 0;

	void _renderShadowVolumes(void);
	void _addToLightMap(LightSource*const src);
	void _sortOnscreenLights(void);

public:

	LightSystem(void);
	void shadowRenderPasses(void);
	unsigned getLightCount(void) const;
	unsigned getOnscreenLightCount(void) const;
	bool	isShadowPass(void) const;

	void addLightSourcesToScene(void);
	void removeLightSourcesFromScene(void);

	void update(void);
	void render(void);

	void setGlobalAmbientColor(const GYVector4& color);

	void setEnabled(const bool val);

	void setRenderDebugEnabled(const bool val);

	//uint32_t getLightMapEntryScreenCoord(const unsigned y, const unsigned x) const;
	uint32_t getLightMapEntryMapCoord(const unsigned y, const unsigned x) const;

	void getLightPassTileRegion(unsigned& startX, unsigned& startY, unsigned& endX, unsigned& endY) const;
	uint32_t startLightPass(uint32_t& lightsLeft);

	unsigned getShaderUniformChangesLastFrame(void) const;
};


inline void LightSystem::setRenderDebugEnabled(const bool val) {
	_renderDebug = val;
}
/*
inline uint32_t LightSystem::getLightMapEntryScreenCoord(const unsigned y, const unsigned x) const {
	return _lightMap[y][x];
}*/

inline uint32_t LightSystem::getLightMapEntryMapCoord(const unsigned y, const unsigned x) const {
	return (_shadowPass)? 0 :_lightMap[y-_screenTileYOffset][x-_screenTileXOffset];
}

inline unsigned LightSystem::getOnscreenLightCount(void) const {
	return (_shadowPass)? 0 : _onscreenLightCount;
}

inline unsigned LightSystem::getShaderUniformChangesLastFrame(void) const {
	return _shaderUniformChangesLastFrame;
}

inline bool LightSystem::isShadowPass(void) const { return _shadowPass; }

}

#endif // ELYSIAN_LIGHT_SYSTEM_H
