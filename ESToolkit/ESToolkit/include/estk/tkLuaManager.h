#ifndef TK_LUAMANAGER_H
#define TK_LUAMANAGER_H

#include "elysian_lua_manager_base.hpp"

#define TK_LUA_MANAGER_METATABLE_SCRIPT_PATH    "/assets/scripts/utils/tkMetatables.lua"


class TkLuaTableUserdata;


class TkLuaManager: public elysian::LuaManagerBase {
friend class elysian::LuaManagerBase;
friend class TkLuaTableUserdata;
private:
    static void _registerWrappers(void);

public:

    static void initialize(void);
};

#endif // TKLUAMANAGER_H
