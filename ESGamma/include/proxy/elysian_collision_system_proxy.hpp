#ifndef ELYSIAN_COLLISION_SYSTEM_PROXY_HPP
#define ELYSIAN_COLLISION_SYSTEM_PROXY_HPP

#include <system/elysian_collision_system.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::CollisionSystem     CollisionSystem;
}

OOLUA_PROXY(CollisionSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_COLLISION_SYSTEM_PROXY_HPP
