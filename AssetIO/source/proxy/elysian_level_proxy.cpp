#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_level_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(Tile,
                        setColliderType,
                        setDamping,
                        setWalkSfx,
                        setHitSfx)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Tile,
                               getColliderType)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Level,
                                    getTile)
EXPORT_OOLUA_FUNCTIONS_2_CONST(Level,
                                //getTileAmount,
                                getDirName,
                                getPath)

