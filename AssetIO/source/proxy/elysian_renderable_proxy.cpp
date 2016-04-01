#include <proxy/elysian_entity_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(Renderable,
                       setFrame,
                       setOffset,
                       setOffsetVec,
                       setSpriteAttrib,
                       setTexCoord,
                       setTexOrient,
                       getOffsetVec,
                       getUVCoords,
                       setUVCoords)
OOLUA_EXPORT_FUNCTIONS_CONST(Renderable,
                           getFrame,
                           getSpriteAttrib,
                           getQuadSpriteAttrib,
                             getTexOrient,
                             getUVCoordsCount)

