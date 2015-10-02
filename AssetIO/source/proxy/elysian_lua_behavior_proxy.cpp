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

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
                                   addTrigger,
                                   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
                                getBehavName)

OOLUA_EXPORT_FUNCTIONS(LuaBehavior,
                        getSelfTable,
                        setTriggers,
                        setBehavName,
                        setParamTable,
                        getParamTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehavior,
                             getTriggers,
                             getScript)
