#include <crtp/elysian_dialog_screen.hpp>
#include <crtp/elysian_dialog.hpp>

#include <proxy/elysian_entity_proxy.hpp>


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
