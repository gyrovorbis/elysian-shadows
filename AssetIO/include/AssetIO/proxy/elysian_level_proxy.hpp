#ifndef ELYSIAN_LEVEL_PROXY_HPP
#define ELYSIAN_LEVEL_PROXY_HPP

#include <oolua.h>
#include <AssetIO/crtp/elysian_level.hpp>

namespace OOLUA {
    typedef elysian::Tile       Tile;
    typedef elysian::Level      Level;

}


OOLUA_PROXY(Tile)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, setColliderType, unsigned)
    OOLUA_MEM_FUNC_CONST(unsigned, getColliderType)
    OOLUA_MEM_FUNC(void, setDamping, float)
    OOLUA_MEM_FUNC(void, setWalkSfx, GYWav*)
    OOLUA_MEM_FUNC(void, setHitSfx, GYWav*)
OOLUA_PROXY_END

OOLUA_PROXY(Level)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(Tile*, getTile, uint16_t)
    //OOLUA_MEM_FUNC_CONST(unsigned short, getTileAmount)
    OOLUA_MEM_FUNC_CONST(const char*, getDirName)
    OOLUA_MEM_FUNC_CONST(const char*, getPath)
OOLUA_PROXY_END

#endif // ELYSIAN_LEVEL_PROXY_HPP

