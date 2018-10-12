#ifndef ELYSIAN_THREAD_SYSTEM_PROXY_HPP
#define ELYSIAN_THREAD_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <ESGamma/system/elysian_thread_system.hpp>

namespace OOLUA {
    typedef elysian::ThreadSystem    ThreadSystem;

}

OOLUA_PROXY(ThreadSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_THREAD_SYSTEM_PROXY_HPP
