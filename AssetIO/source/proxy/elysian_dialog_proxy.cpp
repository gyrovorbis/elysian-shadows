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
#include <proxy/elysian_dialog_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(DialogScreen,
                       addSourceEntity,
                       setTrigger,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogScreen,
                             getText,
                             getIndex,
                             isTrigger,
                             getEntity)

OOLUA_EXPORT_FUNCTIONS(DialogText,
                       setNextIndex)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogText,
                             getNextIndex)

OOLUA_EXPORT_FUNCTIONS(DialogOption,
                       setNextIndex,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogOption,
                             getNextIndex,
                             getText)

OOLUA_EXPORT_FUNCTIONS(DialogPrompt,
                       addOption,
                       setOption)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogPrompt,
                             getOption,
                             getOptionCount)


OOLUA_EXPORT_FUNCTIONS(Dialog,
                       addTextScreen,
                       addPromptScreen,
                       setTriggerEntity,
                       setStartScreen,
                       setScreen)
OOLUA_EXPORT_FUNCTIONS_CONST(Dialog,
                             getScreen,
                             getScreenCount,
                             getTriggerEntity,
                             getStartScreen)

