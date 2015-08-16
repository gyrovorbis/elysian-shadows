#ifndef ELYSIAN_PRIMITIVE_PROXY_HPP
#define ELYSIAN_PRIMITIVE_PROXY_HPP

#include <primitive/elysian_color.hpp>
#include <primitive/elysian_material.hpp>
#include <primitive/elysian_sprite_attrib.hpp>
#include <primitive/elysian_quad_sprite_attrib.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>

namespace OOLUA {
    typedef elysian::Color4             Color4;
    typedef elysian::PrimAttrib         PrimAttrib;
    typedef elysian::SpriteAttrib       SpriteAttrib;
    typedef elysian::QuadSpriteAttrib   QuadSpriteAttrib;
    typedef elysian::Material           Material;

    //template<>                          class Proxy_class<SpriteAttrib>;
    //template<>                          class Proxy_class<QuadSpriteAttrib>;
    //template<>                          class Proxy_class<Material>;
}

OOLUA_PROXY(GYColor4)
    OOLUA_MGET_MSET(a, getA, setA)
    OOLUA_MGET_MSET(r, getR, setR)
    OOLUA_MGET_MSET(g, getG, setG)
    OOLUA_MGET_MSET(b, getB, setB)
OOLUA_PROXY_END

OOLUA_PROXY(Color4)
    OOLUA_MEM_FUNC_CONST(unsigned char, getR)
    OOLUA_MEM_FUNC_CONST(unsigned char, getG)
    OOLUA_MEM_FUNC_CONST(unsigned char, getB)
    OOLUA_MEM_FUNC_CONST(unsigned char, getA)
    OOLUA_MEM_FUNC_CONST(void, setR, const unsigned char)
    OOLUA_MEM_FUNC_CONST(void, setG, const unsigned char)
    OOLUA_MEM_FUNC_CONST(void, setB, const unsigned char)
    OOLUA_MEM_FUNC_CONST(void, setA, const unsigned char)
OOLUA_PROXY_END

OOLUA_PROXY(Material)
    OOLUA_MEM_FUNC(void, setAmbientColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setDiffuseColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setSpecularColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setEmissiveColor, const GYVector4&)
    OOLUA_MEM_FUNC(void, setShininess, float)
    OOLUA_MEM_FUNC_CONST(void, getAmbientColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getDiffuseColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getSpecularColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(void, getEmissiveColor, GYVector4&)
    OOLUA_MEM_FUNC_CONST(float, getShininess)
    OOLUA_MEM_FUNC(Vector4, getEmissive)
    OOLUA_MEM_FUNC(Vector4, getDiffuse)
    OOLUA_MEM_FUNC(Vector4, getAmbient)
    OOLUA_MEM_FUNC(Vector4, getSpecular)
OOLUA_PROXY_END

OOLUA_PROXY(PrimAttrib)
    OOLUA_TAGS (Abstract)
OOLUA_PROXY_END

OOLUA_PROXY(SpriteAttrib, PrimAttrib)
    OOLUA_MEM_FUNC(void, setFrame, unsigned)
    OOLUA_MEM_FUNC_CONST(int, getFrame)
    OOLUA_MEM_FUNC(void, setColor, unsigned char, unsigned char, unsigned char, unsigned char)
    OOLUA_MEM_FUNC_CONST(void, getColor, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>)
    OOLUA_MEM_FUNC_CONST(unsigned char, getAlpha)
    OOLUA_MEM_FUNC_CONST(const char*, getTexPath)
    OOLUA_MEM_FUNC(void, setTexPath, char const*)
    OOLUA_MEM_FUNC_CONST(unsigned char, getColorR)
    OOLUA_MEM_FUNC_CONST(unsigned char, getColorG)
    OOLUA_MEM_FUNC_CONST(unsigned char, getColorB)
    OOLUA_MEM_FUNC_CONST(unsigned char, getColorA)
    OOLUA_MEM_FUNC(Color4, getColor4)
    OOLUA_MEM_FUNC(void, setColor4, const Color4&)
    OOLUA_MEM_FUNC(void, setColorR, const unsigned char)
    OOLUA_MEM_FUNC(void, setColorG, const unsigned char)
    OOLUA_MEM_FUNC(void, setColorB, const unsigned char)
    OOLUA_MEM_FUNC(void, setColorA, const unsigned char)
    OOLUA_MEM_FUNC(void, setAlpha, unsigned char)
    OOLUA_MEM_FUNC(void, loadTexture, const char *, unsigned, unsigned, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, loadSheet, const char *, unsigned, unsigned)
    OOLUA_MEM_FUNC(void, setTexCoord, unsigned, float, float)
    OOLUA_MEM_FUNC(void, setMaterial, cpp_in_p<Material*>)
    OOLUA_MEM_FUNC_CONST(maybe_null<Material*>, getMaterial)
    OOLUA_MEM_FUNC_CONST(const char*, getNormalMapPath)
    OOLUA_MEM_FUNC_CONST(const char*, getSpecularMapPath)
    OOLUA_MEM_FUNC(void, setNormalMapPath, const char*)
    OOLUA_MEM_FUNC(void, setSpecularMapPath, const char*)
OOLUA_PROXY_END

OOLUA_PROXY(QuadSpriteAttrib, SpriteAttrib)
    OOLUA_MEM_FUNC(void, setVertexColor, const unsigned short, const unsigned char, const unsigned char, const unsigned char, const unsigned char)
    OOLUA_MEM_FUNC_CONST(void, getVertexColor, unsigned short, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>, out_p<unsigned char&>)
    OOLUA_MEM_FUNC(void, setVerticesAlpha, const unsigned char, const unsigned char, const unsigned char, const unsigned char)
OOLUA_PROXY_END




#endif // ELYSIAN_PRIMITIVE_PROXY_HPP

