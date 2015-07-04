#ifndef ELYSIAN_LEVEL_SYSTEM_PROXY_HPP
#define ELYSIAN_LEVEL_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <system/elysian_level_system.hpp>

namespace OOLUA {
    typedef elysian::LevelSystem    LevelSystem;

}


OOLUA_PROXY(LevelSystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getPrevAreaPath)
    OOLUA_MFUNC_CONST(getPrevLevelPath)
OOLUA_PROXY_END

#endif // ELYSIAN_LEVEL_SYSTEM_PROXY_HPP

