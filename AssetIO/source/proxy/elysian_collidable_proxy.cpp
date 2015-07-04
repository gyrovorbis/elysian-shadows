#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_animation_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>



OOLUA_EXPORT_FUNCTIONS(CollisionFlags,
                                   resetType,
                                   resetCheckAgainst,
                                   addType,
                                   addCheckAgainst,
                                    setType,
                                    setTypeAgainst)
OOLUA_EXPORT_FUNCTIONS_CONST(CollisionFlags,
                               getType,
                               getTypeAgainst)

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
                                    setWorldSpace)
OOLUA_EXPORT_FUNCTIONS_CONST(Collidable,
                               getAttrib,
                               getContactCount,
                             getResolvePenetration,
                             getWorldSpace,
                             getStackedOnGround,
                             getRestingOnBody)
