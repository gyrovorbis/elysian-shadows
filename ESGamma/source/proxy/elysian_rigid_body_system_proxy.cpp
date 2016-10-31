#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_rigid_body_system_proxy.hpp>



OOLUA_EXPORT_FUNCTIONS(RigidBodySystem,
                       applyForce,
                       addForce,
                       removeForce,
                       clearForces)
OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodySystem)


OOLUA_EXPORT_FUNCTIONS_CONST(ForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ForceGenerator,
                       setForceMask)

OOLUA_EXPORT_FUNCTIONS_CONST(GravityForceGenerator)
OOLUA_EXPORT_FUNCTIONS(GravityForceGenerator,
                       setAccel)

OOLUA_EXPORT_FUNCTIONS_CONST(ImplosionForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ImplosionForceGenerator,
                       setFocalEntity,
                       setForceMagnitude,
                       setRadius)

OOLUA_EXPORT_FUNCTIONS_CONST(ExplosionForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ExplosionForceGenerator,
                       setFocalEntity,
                       setForceMagnitude,
                       setRadius)
