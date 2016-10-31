#include <proxy/elysian_entity_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS(CollisionFlags,
                                    setType,
                                    setTypeAgainst,
                                    getType,
                                    getTypeAgainst)
OOLUA_EXPORT_FUNCTIONS_CONST(CollisionFlags)

OOLUA_EXPORT_FUNCTIONS_CONST(CollidableStaticAttrib,
                             getInverseMass,
                             getCoeffOfRest,
                             getInvMomOfInertia,
                             getAngDamping,
                             getZCoeffOfRest)
OOLUA_EXPORT_FUNCTIONS(CollidableStaticAttrib,
                       setDampingVec,
                       setZDamping,
                       setDamping3f,
                       getDampingVec,
                       setDamping,
                       setInverseMass,
                       setCoeffOfRest,
                       setInvMomOfInertia,
                       setAngDamping,
                       setZCoeffOfRest)

OOLUA_EXPORT_FUNCTIONS(CollidableAttrib,
                                   setColliderType,
                                   setRelPos,
                                   setRelSize,
                                   getFlags,
                                   setFlags,
                                    getStaticAttrib,
                                    setStaticAttrib,
                                    getRelPosVec,
                                    getRelSizeVec,
                                    setRelPosVec,
                                    setRelSizeVec)
OOLUA_EXPORT_FUNCTIONS_CONST(CollidableAttrib,
                               getColliderType)

OOLUA_EXPORT_FUNCTIONS_CONST(CollisionContact,
                             getCollidable,
                             getWorldPos,
                             getNormal,
                             getObjPos,
                             getSeparatingVel,
                             getClosingVel,
                             getDeltaVel,
                             getDepth)
OOLUA_EXPORT_FUNCTIONS(CollisionContact)

OOLUA_EXPORT_FUNCTIONS(Collidable,
                                   setAttrib,
                                   setResolvePenetration,
                                    setWorldSpace,
                                    setActive)
OOLUA_EXPORT_FUNCTIONS_CONST(Collidable,
                               getAttrib,
                               getContactCount,
                             getResolvePenetration,
                             getWorldSpace,
                             getStackedOnGround,
                             getRestingOnBody,
                             isActive)
