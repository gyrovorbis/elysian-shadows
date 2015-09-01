#include <cfloat>
#include <crtp/elysian_light_source.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_area.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LightAttrib)
ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LightSourceBase)

LightSourceBase::~LightSourceBase(void) {
    if(_entity) gyVidLightSourceUninit(static_cast<GYLightSource*>(this));
}

GYLightProperties LightAttrib::defaultProperties = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 10.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.00005f, 0.00005f, 0.00005f },
    1.0f,
	200.0f
};

LightAttrib LightAttrib::defaultAttrib;

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
    const float root2 = (-b - sqrt(b*b - 4.0f*a*c))/(2.0f*a); //reest root

    return root1 >= 0.0f? root1 : root2;
}

float LightAttrib::getRadius(void) {
    //check if shit has changed before performing calculation
    if(!(_prevAttenuation.x == attenuationFactors.x && _prevAttenuation.y == attenuationFactors.y && _prevAttenuation.z == attenuationFactors.z)) {
        //cache the bitch for next time
        _radius = _calculateRadius(attenuationFactors);
    }
    return _radius;
}

void LightSourceBase::setLightType(const GY_LIGHT_TYPE _type) {
  /*  if(_data) {
        gyVidLightSourceUninit(static_cast<GYLightSource*>(this));
        _data = nullptr;
    }*/
	gyVidLightSourceInit(static_cast<GYLightSource*>(this), _type);
}

void LightSourceBase::update(void) {
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

void LightSourceBase::updateBoundingVolume(const Camera *const cam, const GY_CUBE_MAP_FACE face) {
    Camera* thisCam = static_cast<Camera*>(camera);
    GYViewVolume* vol = &thisCam->viewVolume;

	if(type == GY_LIGHT_DIRECTIONAL) {
		*thisCam = *cam;

    } else if(type == GY_LIGHT_SPOT) { //CALCULATE ME LATER!!!!

		*thisCam = *cam;

    } else { // GY_LIGHT_POINT
           //     *thisCam = *cam;

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


        vol->ftl = vol->ntl;
        vol->ftr = vol->ntr;
        vol->fbl = vol->nbl;
        vol->fbr = vol->nbr;

        thisCam->updateViewVolumeBoundingRegion();
                thisCam->clipBoundingRegion(*cam);
	}


}

}
