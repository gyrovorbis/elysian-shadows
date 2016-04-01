#include <proxy/elysian_entity_proxy.hpp>



EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Direction)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Direction)

OOLUA_EXPORT_FUNCTIONS(Character,
                                   faceEntity,
                                   faceDir,
                                   moveByForce,
                                   setAnimate,
                                   setWalkForce,
                                   setDashMultiplier,
                                    setDashing,
                                   getEntityForceDampFactor,
                                    addState,
                                    removeState,
                                    jump,
                                    applyKnockbackForce,
                                    addTempState)
OOLUA_EXPORT_FUNCTIONS_CONST(Character,
                               getFaceDir,
                               isFacing,
                               getHeldEntity,
                                getLastMoveDir,
                                getWalkForce,
                             getDashMultiplier,
                             hasState,
                             getAnimNameFromStates)
