#include "elysian_light_source.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"
#include <cfloat>

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
	200.0f
};

void LightAttrib::setAttenuationFactors(const GYVector3& vec) {
	//quadtratic coefficients
	const float a = vec.z;
	const float b = vec.y;
	const float c = (-1.0f/ELYSIAN_LIGHT_SOURCE_ATTENUATION_EPISOLON + vec.x);

	const float root1 = (-b + sqrt(b*b - 4.0f*a*c))/(2.0f*a);
	//const float root2 = (-b - sqrt(b*b - 4.0f*a*c))/(2.0f*a);


	attenuationFactors = vec;
	_radius = root1;
}

//reimplemented from component
bool LightSource::load(FILE* /*file*/) { return true; }
bool LightSource::save(FILE* /*file*/) { return true; }

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
			getEntity()->transform._pos.y+32.0f,//-200.0f,
			(getEntity()->getElev()+1)*32.0f,
			1.0f
		};
	}
}

void LightSource::updateBoundingVolume(const CameraState *const cam, const GY_CUBE_MAP_FACE face) {
	CameraState* thisCam = static_cast<CameraState*>(camera);
	GYViewVolume* vol = &thisCam->viewVolume;

	if(type == GY_LIGHT_DIRECTIONAL) {
		vol->ntl.x = FLT_MIN; vol->ntl.y = FLT_MIN;
		vol->ntr.x = FLT_MAX; vol->ntr.y = FLT_MIN;
		vol->nbl.x = FLT_MIN; vol->nbl.y = FLT_MAX;
		vol->nbr.x = FLT_MAX; vol->nbr.y = FLT_MAX;

	} else if(type == GY_LIGHT_SPOT) {
		vol->ntl.x = FLT_MIN; vol->ntl.y = FLT_MIN;
		vol->ntr.x = FLT_MAX; vol->ntr.y = FLT_MIN;
		vol->nbl.x = FLT_MIN; vol->nbl.y = FLT_MAX;
		vol->nbr.x = FLT_MAX; vol->nbr.y = FLT_MAX;
	} else { // GY_LIGHT_POINT

		const float halfW = properties->_radius;//maxDist*0.5f;

		vol->ntl.x = position.x - halfW; vol->ntl.y = position.y - halfW;
		vol->ntr.x = position.x + halfW; vol->ntr.y = position.y - halfW;
		vol->nbl.x = position.x - halfW; vol->nbl.y = position.y + halfW;
		vol->nbr.x = position.x + halfW; vol->nbr.y = position.y + halfW;

		switch(face) {
		case GY_CUBE_MAP_RIGHT:
			vol->ntl.x += halfW;
			vol->ntr.x += halfW;
			vol->nbl.x += halfW;
			vol->nbr.x += halfW;
			break;
		case GY_CUBE_MAP_LEFT:
			vol->ntl.x -= halfW;
			vol->ntr.x -= halfW;
			vol->nbl.x -= halfW;
			vol->nbr.x -= halfW;
			break;

		case GY_CUBE_MAP_BACK:
			vol->ntl.y -= halfW;
			vol->ntr.y -= halfW;
			vol->nbl.y -= halfW;
			vol->nbr.y -= halfW;
			break;

		case GY_CUBE_MAP_FRONT:
			vol->ntl.y += halfW;
			vol->ntr.y += halfW;
			vol->nbl.y += halfW;
			vol->nbr.y += halfW;
			break;

		case GY_CUBE_MAP_BOTTOM:
		case GY_CUBE_MAP_TOP:
		default:
			break;
		}

	}

	vol->ftl = vol->ntl;
	vol->ftr = vol->ntr;
	vol->fbl = vol->nbl;
	vol->fbr = vol->nbr;


	thisCam->updateBoundingSubregions(*cam);


}

}
