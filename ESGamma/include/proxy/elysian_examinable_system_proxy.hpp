#ifndef ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP
#define ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP

#include <system/elysian_examinable_system.hpp>
#include <oolua.h>

namespace OOLUA {
    typedef elysian::ExaminableSystem     ExaminableSystem;
}

OOLUA_PROXY(ExaminableSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

#endif // ELYSIAN_EXAMINABLE_SYSTEM_PROXY_HPP
