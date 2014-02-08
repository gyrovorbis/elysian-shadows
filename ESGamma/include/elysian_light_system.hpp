#ifndef ELYSIAN_LIGHT_SYSTEM_H
#define ELYSIAN_LIGHT_SYSTEM_H

#include "elysian_system.hpp"
#include <gyro_matrix_api.h>

namespace elysian {

class LightSystem: public System {
	bool		_enabled		= true;
	bool		_renderDebug	= false;
	GYVector4	_globalAmbience = { 0.2f, 0.2f, 0.5f, 1.0f };

	void _renderShadowVolumes(void);

public:

	LightSystem(void);
	void shadowRenderPasses(void);
	unsigned getLightCount(void);

	void addLightSourcesToScene(void);
	void removeLightSourcesFromScene(void);

	void update(void);
	void render(void);

	void setGlobalAmbientColor(const GYVector4& color);

	void setEnabled(const bool val);

	void setRenderDebugEnabled(const bool val);
};


inline void LightSystem::setRenderDebugEnabled(const bool val) {
	_enabled = val;
}

}

#endif // ELYSIAN_LIGHT_SYSTEM_H
