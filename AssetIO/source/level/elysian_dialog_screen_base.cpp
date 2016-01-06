#include <crtp/elysian_dialog_screen.hpp>
#include <crtp/elysian_dialog.hpp>

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
#include <proxy/elysian_dialog_proxy.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogScreenBase)

int DialogScreenBase::lua_downCast(lua_State*const vm) {
    DialogScreen* screen;
    OOLUA::pull(vm, screen);

    if(screen->getScreenType() == SCREEN_TEXT) {
        OOLUA::push(vm, static_cast<DialogText*>(screen));
        return 1;
    } else if(screen->getScreenType() == SCREEN_PROMPT) {
        OOLUA::push(vm, static_cast<DialogPrompt*>(screen));
        return 1;
    } else {
        GY_ASSERT(false); //undefined type, shit or return nil
        return 0;
    }
}

}
