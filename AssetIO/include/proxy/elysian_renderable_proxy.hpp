#ifndef ELYSIAN_RENDERABLE_PROXY_HPP
#define ELYSIAN_RENDERABLE_PROXY_HPP

#include <crtp/elysian_renderable.hpp>
#include <proxy/elysian_component_proxy.hpp>

namespace OOLUA {
    typedef elysian::Vector2            Vector2;
    typedef elysian::SpriteAttrib       SpriteAttrib;
    typedef elysian::Material           Material;
    typedef elysian::QuadSpriteAttrib   QuadSpriteAttrib;
}

OOLUA_PROXY(Renderable, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Vector2, getOffsetVec)
    OOLUA_MEM_FUNC(void, setOffsetVec, const Vector2&)
    OOLUA_MEM_FUNC(void, setOffset, const GYVector2&)
    OOLUA_MEM_FUNC_CONST(unsigned, getFrame)
    OOLUA_MEM_FUNC(void, setFrame, unsigned)
    OOLUA_MEM_FUNC_CONST(int, getTexOrient)
    OOLUA_MEM_FUNC(void, setTexOrient, GY_RECT_TEX_ORIENT)
    OOLUA_MEM_FUNC_CONST(maybe_null<SpriteAttrib*>, getSpriteAttrib)
    OOLUA_MEM_FUNC_CONST(QuadSpriteAttrib*, getQuadSpriteAttrib)
    OOLUA_MEM_FUNC(void, setSpriteAttrib, cpp_in_p<SpriteAttrib*>)
    OOLUA_MEM_FUNC(void, setMaterial, cpp_in_p<Material*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<Material*>, getMaterial)
    OOLUA_MFUNC(loadNormalMap)
    OOLUA_MEM_FUNC(void, loadSpecularMap, const char*, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, setTexCoord, const unsigned, const float, const float)
    OOLUA_MEM_FUNC_CONST(const char*, getNormalMapPath)
    OOLUA_MEM_FUNC_CONST(const char*, getSpecularMapPath)
    OOLUA_MEM_FUNC(void, setNormalMapPath, const char*)
    OOLUA_MEM_FUNC(void, setSpecularMapPath, const char*)
    OOLUA_MEM_FUNC_CONST(unsigned, getUVCoordsCount)
    OOLUA_MEM_FUNC(Vector2, getUVCoords, const unsigned)
    OOLUA_MEM_FUNC(void, setUVCoords, const unsigned, const Vector2&)
OOLUA_PROXY_END

#endif // ELYSIAN_RENDERABLE_PROXY_HPP

