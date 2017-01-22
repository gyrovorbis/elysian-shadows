#ifndef TK_LUAMANAGER_H
#define TK_LUAMANAGER_H

#include <elysian_lua_manager_base.hpp>
#include <tk_lua_qt_utils.h>

#define TK_LUA_MANAGER_METATABLE_SCRIPT_PATH    "/assets/scripts/estk/tk_metatable.lua"

class TkLuaTableUserdata;


class TkLuaManager: public elysian::LuaManagerBase {
friend class elysian::LuaManagerBase;
friend class TkLuaTableUserdata;
private:


    static void _registerWrappers(void);

protected:
    static QStringList              _udTypeNames;
    static elysian::LuaVariant      _serializeFunc;
    static unsigned                 _serializeDepth;

public:

    static bool         initialize(void);
    static bool         uninitialize(void);
    static bool         getMetatable(const int index);
    static elysian::LuaVariant   getParent(const elysian::LuaVariant& t);

    static QStringList  getUdTypeNames(void);

    static int          _serializeCFunc(lua_State* vm);
    //static void getGlobal(const char*const key);
};






// ===== INLINEZ ========

inline QStringList TkLuaManager::getUdTypeNames(void) { return _udTypeNames; }

/*
inline void TkLuaManager::getGlobal(const char * const key) {
    lua_pushglobaltable(*_mainState);
    lua_pushstring(*_mainState, key);
    lua_gettable(*_mainState, -2);
    lua_remove(*_mainState, -2);
}*/


/* We have to call the function from Lua, because
 * it has been overridden to respect Tk proxy metatables. */
inline bool TkLuaManager::getMetatable(const int index) {
    pushGlobalField("getmetatable");
    lua_pushvalue(*_mainState, index < 0? index-1 : index);
    _protectedCall(1, 1, nullptr);

    if(lua_isnil(*_mainState, -1)) {
        lua_pop(*_mainState, 1);
        return false;
    }

    return true;
}




#endif // TKLUAMANAGER_H
