#include "crtp/elysian_renderable.hpp"

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(RenderableBase)

RenderableBase::RenderableBase(const RenderableBase& other) {
    *this = other;
}


}
