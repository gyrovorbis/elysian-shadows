#include <AssetIO/crtp/elysian_renderable.hpp>

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(RenderableBase)

shared_ptr<QuadSpriteAttrib> RenderableBase::getQuadSpriteAttrib(void) const {
    if(_spriteAttrib->getType() == PrimAttrib::QUAD_SPRITE) {
        return std::static_pointer_cast<QuadSpriteAttrib>(_spriteAttrib);
    } else {
        return nullptr;
    }
}



}
