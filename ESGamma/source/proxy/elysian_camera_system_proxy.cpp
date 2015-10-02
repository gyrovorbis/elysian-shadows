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

#include <proxy/elysian_camera_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS_CONST(CameraKeyFrame)
OOLUA_EXPORT_FUNCTIONS(CameraKeyFrame)

OOLUA_EXPORT_FUNCTIONS(CameraSystem,
                       scale,
                       moveToKeyFrame,
                       setBoundaries,
                       setDynamic,
                       pushPerspectiveState,
                       popState,
                       shake,
                       setFollowEntity)
OOLUA_EXPORT_FUNCTIONS_CONST(CameraSystem,
                       addEntity,
                       removeEntity,
                       getKeyFrame)
