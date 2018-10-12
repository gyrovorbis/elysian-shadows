#ifndef ELYSIAN_COMPUTE_SYSTEM_PROXY_HPP
#define ELYSIAN_COMPUTE_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/system/elysian_thread_system.hpp>

namespace OOLUA {
    typedef elysian::ComputeSystem    ComputeSystem;

}

OOLUA_PROXY(ComputeSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_COMPUTE_SYSTEM_PROXY_HPP
