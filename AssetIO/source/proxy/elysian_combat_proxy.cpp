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



OOLUA_EXPORT_FUNCTIONS_CONST(CombatAttrib)
OOLUA_EXPORT_FUNCTIONS(CombatAttrib,
                       setTypeFlags,
                       setTypeAgainstFlags,
                       setParticleAttrib,
                       getTypeFlags,
                       getTypeAgainstFlags)

OOLUA_EXPORT_FUNCTIONS_CONST(CombatStatus,
                             getMp,
                             getHp,
                             getMaxHp,
                             getMaxMp,
                             hasStatusEffect)
OOLUA_EXPORT_FUNCTIONS(CombatStatus,
                       setHp,
                       setMaxHp,
                       setMaxMp,
                       setMp)

OOLUA_EXPORT_FUNCTIONS_CONST(CombatStatusModifier)
OOLUA_EXPORT_FUNCTIONS(CombatStatusModifier,
                       addStatusEffect,
                       removeStatusEffect)

OOLUA_EXPORT_FUNCTIONS_CONST(Combat,
                             getAttrib,
                             getTargetType,
                             getDmgActive,
                             getDmgType,
                             getAggroDist,
                             getDeaggroDist,
                             isInCombat)
OOLUA_EXPORT_FUNCTIONS(Combat,
                       setSelfTable,
                       setActionTable,
                       getSelfTable,
                       getActionTable,
                       setAttrib,
                       setDmgType,
                       setDmgActive,
                       setCollisionFlags,
                       setStatus,
                       setTargetType,
                       setAggroDist,
                       setDeaggroDist,
                       getCollisionFlags,
                       getStatus)
