#ifndef ELYSIAN_LIGHT_SOURCE_PROXY_HPP
#define ELYSIAN_LIGHT_SOURCE_PROXY_HPP

#include <crtp/elysian_light_source.hpp>

namespace OOLUA {
    typedef elysian::LightAttrib        LightAttrib;
    typedef elysian::LightSource        LightSource;
    typedef elysian::Vector4            Vector4;
    typedef elysian::Vector3            Vector3;

}

OOLUA_PROXY(LightAttrib)
    OOLUA_TAGS(Shared)
    OOLUA_MEM_FUNC(void, setAmbientColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setDiffuseColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setSpecularColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setAttenuationFactors, const GYVector3&)
    OOLUA_MEM_FUNC(void, setSpotExponent, const float)
    OOLUA_MEM_FUNC_CONST(void, getAmbientColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getDiffuseColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getSpecularColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getAttenuationFactors, GYVector3&)
    OOLUA_MEM_FUNC(void, setAmbient, const Vector4&)
    OOLUA_MEM_FUNC(void, setDiffuse, const Vector4&)
    OOLUA_MEM_FUNC(void, setSpecular, const Vector4&)
    OOLUA_MEM_FUNC(void, setAttenuation, const Vector3&)
    OOLUA_MEM_FUNC_CONST(float, getSpotExponent)
    OOLUA_MEM_FUNC(Vector4, getAmbient)
    OOLUA_MEM_FUNC(Vector4, getDiffuse)
    OOLUA_MEM_FUNC(Vector4, getSpecular)
    OOLUA_MEM_FUNC(Vector3, getAttenuation)
    OOLUA_MEM_FUNC(float, getRadius)
    //OOLUA_MEM_FUNC(void, setComputeDistanceAttenuation, const bool)
OOLUA_PROXY_END

OOLUA_PROXY(LightSource, Component)
     OOLUA_TAGS (
         No_public_constructors,
         No_public_destructor
     )
    OOLUA_MEM_FUNC(void, setLightType, const GY_LIGHT_TYPE)
    OOLUA_MEM_FUNC(void, setDirection, const GYVector3&)
   // OOLUA_MEM_FUNC(void, setRelativePos, float, float, float)
    OOLUA_MFUNC(setRelPos)
    OOLUA_MEM_FUNC(void, setSpotCutoffAngle, const float)
    OOLUA_MEM_FUNC(void, setAttrib, std::shared_ptr<LightAttrib>)
    OOLUA_MEM_FUNC_CONST(maybe_null<std::shared_ptr<LightAttrib>>, getAttrib)
    OOLUA_MFUNC(setShadowCaster)
    OOLUA_MEM_FUNC_CONST(void, getDirection, GYVector3&)
    OOLUA_MEM_FUNC_CONST(float, getSpotCutoffAngle)
    OOLUA_MEM_FUNC_CONST(GY_LIGHT_TYPE, getLightType)
    OOLUA_MEM_FUNC_CONST(bool, isShadowCaster)
    OOLUA_MEM_FUNC_CONST(void, getRelativePos, GYVector3&)
    OOLUA_MEM_FUNC(Vector3, getRelPos)
    OOLUA_MEM_FUNC(Vector3, getDir)
OOLUA_PROXY_END


#endif // ELYSIAN_LIGHT_SOURCE_PROXY_HPP

