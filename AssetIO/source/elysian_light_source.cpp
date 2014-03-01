#include "elysian_light_source.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"

namespace elysian {

LightSource::~LightSource(void) {
#ifdef ELYSIAN_ENGINE
	gyVidLightSourceUninit(static_cast<GYLightSource*>(this));
#endif
}

/*
typedef struct _GYLightProperties {
	GYVector4	ambientColor;
	GYVector4	diffuseColor;
	GYVector4	specularColor;
	GYVector3	attenuationFactors;
	float		spotExponent;
	int			computeDistanceAtte
*/
GYLightProperties LightAttrib::defaultProperties = {
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.7f, 0.7f, 0.7f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.00005f, 0.00005f, 0.00005f },
	1.0f,
};

//reimplemented from component
bool LightSource::load(std::ifstream& /*stream*/) { return true; }
bool LightSource::save(std::ofstream& /*stream*/) { return true; }

bool LightSource::addToLevel(void) {
	_curArea->lightSources.push_back(this);
	return true;
}

bool LightSource::removeFromLevel(void) {
	containerEraseOnMatch(this, _curArea->lightSources);
	return true;
}

void LightSource::setLightType(const GY_LIGHT_TYPE _type) {
	if(type == GY_LIGHT_DIRECTIONAL) position.w = 0.0f;

#ifdef ELYSIAN_ENGINE
	gyVidLightSourceInit(static_cast<GYLightSource*>(this), _type);
#endif
}

void LightSource::update(void) {
	if(type == GY_LIGHT_POINT || type == GY_LIGHT_SPOT) {
		position = {
			getEntity()->transform._pos.x,//-200.0f,
			getEntity()->transform._pos.y,//-200.0f,
			(getEntity()->getElev()+1)*32.0f,
			1.0f
		};
	}
}

}
