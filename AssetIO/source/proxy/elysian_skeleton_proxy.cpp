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

OOLUA_EXPORT_FUNCTIONS(SkeletonAttrib,
                        loadAssets,
                        setJsonPath,
                        setAtlasPath)
OOLUA_EXPORT_FUNCTIONS_CONST(SkeletonAttrib,
                             getJsonPath,
                             getAtlasPath)

OOLUA_EXPORT_FUNCTIONS_CONST(Skeleton,
                             getAttrib,
                             getAnimTimeScale,
                             setAnimByName,
                             getEntityAttachedToSlot)
OOLUA_EXPORT_FUNCTIONS(Skeleton,
                       setAttrib,
                       setAnimTimeScale,
                       attachEntityToSlot,
                       removeEntityFromSlot)
