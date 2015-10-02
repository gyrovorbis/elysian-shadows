#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
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

