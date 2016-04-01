#include <proxy/elysian_entity_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(Warp,
                                   setToPos,
                                   setAreaPath,
                                   setLevelPath,
                                   setChangeArea,
                                   setChangeLevel,
                                   setObliqueTransition,
                                    getToPosVec,
                                    setToPosVec)
OOLUA_EXPORT_FUNCTIONS_CONST(Warp,
                             getAreaPath,
                             getLevelPath,
                             getChangeArea,
                             getChangeLevel,
                             getObliqueTransition)
