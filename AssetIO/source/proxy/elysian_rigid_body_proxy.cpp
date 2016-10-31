#include <proxy/elysian_entity_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodyAttrib,
                             getCanHold,
                             isBreakable,
                             getBaseDurability)
OOLUA_EXPORT_FUNCTIONS(RigidBodyAttrib,
                       setForceMask,
                       setBreakable,
                       setBaseDurability,
                       setCanHold,
                       getForceMask)

OOLUA_EXPORT_FUNCTIONS(ForceTarget,
                       applyForce,
                       applyForceToPoint,
                       clearKinetic,
                       getVel,
                       getAccel,
                       getForceAccum,
                       setRotation,
                       setAngAccel,
                       setTorqueAccum)
OOLUA_EXPORT_FUNCTIONS_CONST(ForceTarget,
                             getRotation,
                             getAngAccel,
                             getTorqueAccum)

OOLUA_EXPORT_FUNCTIONS_CONST(RigidBody,
                             getDurability,
                             getAttrib,
                             getVelocity,
                             getTileDamping)
OOLUA_EXPORT_FUNCTIONS(RigidBody,
                       setAttrib,
                       setDurability,
                       setVelocity)

