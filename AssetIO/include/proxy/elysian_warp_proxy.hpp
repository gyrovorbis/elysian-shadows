#ifndef ELYSIAN_WARP_PROXY_HPP
#define ELYSIAN_WARP_PROXY_HPP

#include <crtp/elysian_warp.hpp>

namespace OOLUA {
    typedef elysian::Warp Warp;
}

OOLUA_PROXY(Warp, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Vector3, getToPosVec)
    OOLUA_MEM_FUNC(void, setToPosVec, const Vector3&)
    OOLUA_MEM_FUNC(void, setAreaPath, const char*)
    OOLUA_MEM_FUNC_CONST(const char*, getAreaPath)
    OOLUA_MEM_FUNC(void, setLevelPath, const char*)
    OOLUA_MEM_FUNC_CONST(const char*, getLevelPath)
    OOLUA_MEM_FUNC(void, setChangeArea, bool)
    OOLUA_MEM_FUNC_CONST(bool, getChangeArea)
    OOLUA_MEM_FUNC(void, setChangeLevel, bool)
    OOLUA_MEM_FUNC_CONST(bool, getChangeLevel)
    OOLUA_MEM_FUNC(void, setObliqueTransition, bool)
    OOLUA_MEM_FUNC_CONST(bool, getObliqueTransition)
OOLUA_PROXY_END

#endif // ELYSIAN_WARP_PROXY_HPP

