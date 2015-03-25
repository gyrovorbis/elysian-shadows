#include <cfloat>
#include "elysian_light_source.hpp"
#include "elysian_generic.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LightAttrib)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LightSource)

LightSource::~LightSource(void) {
	gyVidLightSourceUninit(static_cast<GYLightSource*>(this));
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
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.00005f, 0.00005f, 0.00005f },
    1.0f,
	200.0f
};

void LightAttrib::setAttenuationFactors(const GYVector3& vec) {
    attenuationFactors = vec;
    getRadius();
}

float LightAttrib::_calculateRadius(const GYVector3 &vec) {
    //Quadratic coefficients
    const float a = vec.z;
    const float b = vec.y;
    const float c = vec.x -(1.0f/ELYSIAN_LIGHT_SOURCE_ATTENUATION_EPSILON);

    //LOLOLOL QUADRATIC EQUATION
    const float root1 = (-b + gySqrt(b*b - 4.0f*a*c))/(2.0f*a);
    //const float root2 = (-b - sqrt(b*b - 4.0f*a*c))/(2.0f*a); //reest root

    return root1;
}

float LightAttrib::getRadius(void) {
    //check if shit has changed before performing calculation
    if(!(_prevAttenuation.x == attenuationFactors.x && _prevAttenuation.y == attenuationFactors.y && _prevAttenuation.z == attenuationFactors.z)) {
        //cache the bitch for next time
        _radius = _calculateRadius(attenuationFactors);
    }
    return _radius;
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

	gyVidLightSourceInit(static_cast<GYLightSource*>(this), _type);
}

void LightSource::update(void) {
    static const GYVector4 localVec = { 0.0f, 0.0f, 0.0f, 1.0f };
	if(type == GY_LIGHT_POINT || type == GY_LIGHT_SPOT) {

        gyMatPush();
        gyMatLoadIdentity();
        _entity->_transform.apply();
        gyMatTranslate(_relPos.x, _relPos.y, _relPos.z);
        gyMatMultVec(&localVec, &position, 1);
        gyMatPop();
	}
}

void LightSource::updateBoundingVolume(const CameraState *const cam, const GY_CUBE_MAP_FACE face) {
	CameraState* thisCam = static_cast<CameraState*>(camera);
	GYViewVolume* vol = &thisCam->viewVolume;

	if(type == GY_LIGHT_DIRECTIONAL) {
		*thisCam = *cam;

    } else if(type == GY_LIGHT_SPOT) { //CALCULATE ME LATER!!!!

		*thisCam = *cam;

	} else { // GY_LIGHT_POINT

        const float halfW = static_cast<LightAttrib*>(properties)->getRadius();

        vol->ntl.x = position.x - halfW; vol->ntl.y = position.y - halfW;
        vol->ntr.x = position.x + halfW; vol->ntr.y = position.y - halfW;
        vol->nbl.x = position.x - halfW; vol->nbl.y = position.y + halfW;
        vol->nbr.x = position.x + halfW; vol->nbr.y = position.y + halfW;

		switch(face) {
		case GY_CUBE_MAP_RIGHT:
            vol->ntl.x = vol->nbl.x = position.x;
			break;
		case GY_CUBE_MAP_LEFT:
            vol->ntr.x = vol->nbr.x = position.x;
			break;

		case GY_CUBE_MAP_BACK:
            vol->nbl.y = vol->nbr.y = position.y;
			break;

		case GY_CUBE_MAP_FRONT:
            vol->ntl.y = vol->ntr.y = position.y;
			break;

		case GY_CUBE_MAP_BOTTOM:
		case GY_CUBE_MAP_TOP:
		default:
			break;
        }
	}



    if(type != GY_LIGHT_DIRECTIONAL) {
        vol->ftl = vol->ntl;
        vol->ftr = vol->ntr;
        vol->fbl = vol->nbl;
        vol->fbr = vol->nbr;

        thisCam->clipBoundingRegion(*cam);
        thisCam->updateViewVolumeBoundingRegion();
    }


}

}
