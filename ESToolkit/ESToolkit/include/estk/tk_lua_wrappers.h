#ifndef TK_LUAWRAPPERS_H
#define TK_LUAWRAPPERS_H


#include <oolua.h>
#include <estk/tk_estoolkit.h>
#include <lua/elysian_lua_singleton.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <asset/tk_area.h>


namespace OOLUA
{




}

class Toolkit {};

OOLUA_PROXY(Toolkit)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END



static inline int Toolkit_select(lua_State* const vm) {
    const int ref = luaL_ref(vm, LUA_REGISTRYINDEX);
    ESToolkit::select(ref);
    luaL_unref(vm, LUA_REGISTRYINDEX, ref);
    return 0;
}

static inline int Toolkit_findEntity(lua_State* const vm) {
    QString name;
    OOLUA::pull(vm, name);
    auto* entity = Debug::getCurrentArea()->findEntity(Q_CSTR(name));
    OOLUA::push(vm, entity);
    return entity? 1 : 0;
}


static inline void luaRegisterTkEnumsConstants(OOLUA::Script */*s*/) {

}





#endif // TK_LUAWRAPPERS_H
