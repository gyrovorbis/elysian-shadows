#include "elysian_light_system.hpp"
#include "elysian_light_source.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include <gyro_shader_api.h>

#include "elysian_level.hpp"

namespace elysian {

LightSystem::LightSystem(void) {
	setGlobalAmbientColor(_globalAmbience);
	//gyVidLightingSetEnable(1);
}

unsigned LightSystem::getLightCount(void) { return _curArea->lightSources.size(); }

void LightSystem::update(void) {
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

void LightSystem::_renderShadowVolumes(void) {
	const auto entities = _curArea->entities;
	for(auto it : entities) {
		if(it->isShadowVolume()) it->renderShadowVolume();
	}
}

static void renderCube(float x, float y, float z,
					   float sx, float sy, float sz,
					   float rotz) {
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


void LightSystem::render(void) {
/*
	gyVidTexBind(nullptr);
	gyVidMaterialBind(nullptr);
	gyVidNormalMapBind(nullptr);

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

	_renderShadowVolumes();

	*/
}

void LightSystem::shadowRenderPasses(void) {
	const auto sources = _curArea->lightSources;

	gyVidLightSourceBeginShadowPasses();

	for(auto it : sources) {
		switch(it->getLightType()) {
		case GY_LIGHT_DIRECTIONAL:
		case GY_LIGHT_SPOT:
			gyVidLightSourceBeginShadowPass(it, GY_CUBE_MAP_FACE_MAX);
			render();
			break;
		case GY_LIGHT_POINT:
			for(unsigned i = GY_CUBE_MAP_RIGHT; i < GY_CUBE_MAP_FACE_MAX; ++i) {
				gyVidLightSourceBeginShadowPass(it, (GY_CUBE_MAP_FACE)i);
				//if(i != GY_CUBE_MAP_BACK) {
					render();
			//	}
			}
			break;
		}
	}

	gyVidLightSourceEndShadowPasses();

}

void LightSystem::addLightSourcesToScene(void) {
	gyVidSetGlobalAmbience(&_globalAmbience);
	const auto sources = _curArea->lightSources;
	gyVidLightSourceClear();
	for(auto it : sources) {
		gyVidLightSourceAdd(it);
	}
}

void LightSystem::removeLightSourcesFromScene(void) {
	const static GYVector4 fullAmbience = GYVector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	gyVidSetGlobalAmbience(&fullAmbience);
	gyVidLightSourceClear();

}


}
