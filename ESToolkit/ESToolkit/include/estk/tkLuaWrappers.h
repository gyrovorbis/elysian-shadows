#ifndef TK_LUAWRAPPERS_H
#define TK_LUAWRAPPERS_H


#include <oolua.h>
#include "estoolkit.h"


namespace OOLUA
{




}

class estk {};
OOLUA_PROXY(estk)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END



static inline int estk_inspectTable(lua_State* const vm) {
    OOLUA_C_FUNCTION(void, ESToolkit::inspectTable, OOLUA::Lua_table_ref)
}


static inline void luaRegisterTkEnumsConstants(OOLUA::Script *s) {

}





#endif // TK_LUAWRAPPERS_H
