#include <proxy/elysian_primitive_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(GYColor4, setA, setR, setG, setB)
OOLUA_EXPORT_FUNCTIONS_CONST(GYColor4, getA, getR, getG, getB)

OOLUA_EXPORT_FUNCTIONS(Color4)
OOLUA_EXPORT_FUNCTIONS_CONST(Color4, getA, getR, getG, getB, setA, setR, setG, setB)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(PrimAttrib)
EXPORT_OOLUA_FUNCTIONS_0_CONST(PrimAttrib)

OOLUA_EXPORT_FUNCTIONS(SpriteAttrib,
                                   setFrame,
                                   setColor,
                                   setAlpha,
                                   loadTexture,
                                   loadSheet,
                                   setTexCoord,
                                   setColorR,
                                   setColorG,
                                   setColorB,
                                   setColorA,
                                    getColor4,
                                    setColor4,
                                    setTexPath,
                                   setSpecularMapPath,
                                   setNormalMapPath,
                                   setMaterial)
OOLUA_EXPORT_FUNCTIONS_CONST(SpriteAttrib,
                               getColor,
                               getAlpha,
                                getTexPath,
                             getColorR,
                             getColorG,
                             getColorB,
                             getColorA,
                             getFrame,
                             getNormalMapPath,
                             getSpecularMapPath,
                             getMaterial)

EXPORT_OOLUA_FUNCTIONS_1_CONST(QuadSpriteAttrib,
                               getVertexColor)
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(QuadSpriteAttrib,
                                   setVertexColor,
                                   setVerticesAlpha)

OOLUA_EXPORT_FUNCTIONS(Material,
                       setShininess,
                       setAmbientColor,
                       setDiffuseColor,
                       setSpecularColor,
                       setEmissiveColor,
                       getAmbient,
                       getDiffuse,
                       getEmissive,
                       getSpecular)
OOLUA_EXPORT_FUNCTIONS_CONST(Material,
                             getAmbientColor,
                             getDiffuseColor,
                             getSpecularColor,
                             getEmissiveColor,
                             getShininess)
