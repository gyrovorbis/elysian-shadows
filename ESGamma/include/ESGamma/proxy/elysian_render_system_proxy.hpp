#ifndef ELYSIAN_RENDER_SYSTEM_PROXY_HPP
#define ELYSIAN_RENDER_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/crtp/elysian_render_system.hpp>

namespace OOLUA {
    typedef elysian::RenderSystem    RenderSystem;

}

OOLUA_PROXY(RenderSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_RENDER_SYSTEM_PROXY_HPP
