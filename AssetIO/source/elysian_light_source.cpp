#include "elysian_light_source.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"

namespace elysian {
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
	{ 0.7f, 0.7f, 0.7f, 0.7f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
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

void LightSource::update(void) {
	if(_type == LIGHT_POINT) {
		position = {
			getEntity()->transform._pos.x,//-200.0f,
			getEntity()->transform._pos.y,//-200.0f,
			getEntity()->transform.getPosZ(),
			1.0f
		};
	}
}

}
