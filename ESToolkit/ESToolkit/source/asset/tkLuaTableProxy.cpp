#include "tkLuaTableProxy.h"

#define PRINT(a) do { \
    } while(0)

TkLuaTableUserdata*  TkLuaTableUserdata::inject(lua_State*const l, const int luaRef) {
    TkLuaTableUserdata* ud;


    PRINT("1");
    unsigned oldTop = lua_gettop(l);

    // TERMINOLOGY:
    // 1) original table (table we're injecting into)
    // 2) proxy table (new table with Tk callbacks as original's metatable)
    // 3) parent table (origina's metatable, becomes proxy's metatable)

    //Retrieve field directly from metatable, since we can't access it through a metamethod
    lua_rawgeti(l, LUA_REGISTRYINDEX, luaRef);

    PRINT("2");

    //LuaManager::rPrintByIndex(-1, 99, "LOLOL");

    lua_getmetatable(l, -1);
    if(!lua_isnil(l, -1)) {
        lua_getfield(l, -1, TK_LUA_TABLE_USERDATA_FIELD_KEY);
            PRINT("3");
        //lua_remove(l, -2);
    }

    if(!lua_isnil(l, -1)) {     //table already owned by ESTk, return userdata pointer
        ud = static_cast<TkLuaTableUserdata*>(lua_touserdata(l, -1));
        assert(ud);

    } else {    //create new proxy table
        //LuaManager::_cStackDump();

        lua_pop(l, 1); //only original on stack

        ud = new TkLuaTableUserdata(l);
        PRINT("4");

        //Store reference to original table
        lua_pushvalue(l, -1);
        ud->setOrigRef(luaL_ref(l, LUA_REGISTRYINDEX));                     //1) orig table

        PRINT("5");

        //Create proxy table, store ref
        lua_newtable(l);
        lua_pushvalue(l, -1);
        ud->setProxyRef(luaL_ref(l, LUA_REGISTRYINDEX));                    //1) orig table 2) proxy table
        PRINT("6");

        //add ud ptr lightuserdata field to proxy table
        lua_pushlightuserdata(l, ud);
        lua_setfield(l, -2, TK_LUA_TABLE_USERDATA_FIELD_KEY);                                      //1) orig table 2) proxy table
        PRINT("7");



        //iterate over original table, moving entries to proxy table
        lua_rawgeti(l, LUA_REGISTRYINDEX, ud->getOrigRef());
        lua_pushnil(l);  /* first key */

        PRINT("8");

        while (lua_next(l, -2) != 0) {                                       //1) orig table 2) proxy table 3) orig table 4) key 5) value

            lua_pushvalue(l, -2);       //duplicate key
            lua_insert(l, -2);          //move value to top
            lua_rawset(l, -5);           //copy <key,value> to proxy table

            lua_pushvalue(l, -1);       //duplicate key
            lua_pushnil(l);             //push nil for field value (deletes field)
            lua_rawset(l, -4);           //remove <key, value> from orig table
            //lua_pop(l, 3);
            PRINT("!!!!");
        }                                                                   //1) orig table 2) proxy table 3) orig table
    }
    lua_pop(l, 1);

      PRINT("9");
        //set proxy table's metamethods
        lua_pushcfunction(l, &TkLuaTableUserdata::_indexMetamethod);
        lua_setfield(l, -2, "__index");
        lua_pushcfunction(l, &TkLuaTableUserdata::_newIndexMetamethod);
        lua_setfield(l, -2, "__newindex");
        lua_pushcfunction(l, &TkLuaTableUserdata::_gcMetamethod);
        lua_setfield(l, -2, "__gc");                                        //1) orig table 2) proxy table
        lua_pushcfunction(l, &TkLuaTableUserdata::_pairsMetamethod);
        lua_setfield(l, -2, "__pairs");
        lua_pushcfunction(l, &TkLuaTableUserdata::_ipairsMetamethod);
        lua_setfield(l, -2, "__ipairs");
        //lua_pushcfunction(l, &TkLuaTableUserdata::_nextMetamethod);
        //lua_setfield(l, -2, "__next");
        lua_pushcfunction(l, &TkLuaTableUserdata::_lengthMetamethod);
        lua_setfield(l, -2, "__len");

        //set original's metatable to proxy table
        lua_pushvalue(l, -1);
        lua_setmetatable(l, -3);                                            //1) orig table 2) proxy table

        //set proxy's metatable to parent table
        lua_getmetatable(l, -2);
        lua_setmetatable(l, -2);                                            //1) orig table 2) proxy table

    lua_pop(l, 2);

    LuaManager::rPrintByRef(ud->getOrigRef(), 99, "ORIG");
     LuaManager::rPrintByRef(ud->getOrigRef(), 99, "PROXY");
    return ud;
}

TkLuaTableUserdata::~TkLuaTableUserdata(void) {
    luaL_unref(_lvm, LUA_REGISTRYINDEX, _proxyRef);
    luaL_unref(_lvm, LUA_REGISTRYINDEX, _origRef);
    Debug::printf(Debug::VERBOSE, "__TKUD GARBAGE COLLECTED!!!!");
}

int TkLuaTableUserdata::_indexMetamethod(lua_State* l) {
    const char*         keyName;

    //make sure we aren't trying to access our hidden pointer
    lua_pushvalue(l, -1);
    keyName = lua_tostring(l, -1);
    if(keyName && (strcmp(keyName, TK_LUA_TABLE_USERDATA_FIELD_KEY) == 0)) {

        return 0;
    }
    lua_pop(l, 1);

    //retrieve value from proxy table and parents
    lua_getmetatable(l, -2);
    lua_insert(l, -2);
    lua_gettable(l, -2);
    return 1;
}

int TkLuaTableUserdata::_newIndexMetamethod(lua_State* l) {
    bool                newKey = false;
    const char*         keyName;
    TkLuaTableUserdata* ud;

    //make sure we aren't trying to access our hidden pointer
    lua_pushvalue(l, -2);
    keyName = lua_tostring(l, -1);
    if(keyName && (strcmp(keyName, TK_LUA_TABLE_USERDATA_FIELD_KEY)==0)) {
        return 0;
    }
    lua_pop(l, 1);

    //check if the value exists in proxy table or parents
    lua_getmetatable(l, 1);
    lua_pushvalue(l, 2);
    lua_gettable(l, -2);
    newKey = lua_isnil(l, -1);
    lua_pop(l, 1);

    lua_pushvalue(l, 2);    //key
    lua_pushvalue(l, 3);    //new value
    lua_settable(l, -3);    //set or insert value

    //notify Qt that shit has gone down
    lua_getfield(l, -1, TK_LUA_TABLE_USERDATA_FIELD_KEY);
    ud = static_cast<TkLuaTableUserdata*>(lua_touserdata(l, -1));

    if(newKey)  ud->emitFieldAdded(lua_tostring(l, 2));
    else        ud->emitFieldChanged(lua_tostring(l, 2));

    return 0;
}


int TkLuaTableUserdata::_gcMetamethod(lua_State* l) {
    lua_getfield(l, -1, TK_LUA_TABLE_USERDATA_FIELD_KEY);
    auto* ud = static_cast<TkLuaTableUserdata*>(lua_touserdata(l, -1));
    assert(ud);
    delete ud;
    lua_pop(l, lua_gettop(l));
    return 0;
}

#if 0
static int _ipairsMetaMethodCFunction(lua_State* l) {
    //Pushes onto Lua stack:
    //1 - Userdata iterator function
    //2 - Userdata
    //3 - nil (first key)
    lua_pushcfunction(l, &_iNextMetaMethodCFunction);
    lua_insert(l, 1);
    lua_pushnumber(l, 0);

   // qDebug() << "IPAIRS";

    return 3;
}

static int _pairsMetaMethodCFunction(lua_State* l) {
    //Pushes onto Lua stack:
    //1 - Userdata iterator function
    //2 - Userdata
    //3 - nil (first key)
    lua_pushcfunction(l, &_nextMetaMethodCFunction);
    lua_insert(l, 1);
    lua_pushnil(l);

    return 3;
}

static int _lengthMetaMethodCFunction(lua_State* l) {
    lua_pop(l, lua_gettop(l));
    lua_pushinteger(l, P::_proxyFieldCount);
    return 1;
}

#endif

int TkLuaTableUserdata::_pairsMetamethod(lua_State*const l) {
    //Pushes onto Lua stack:
    //1 - next metamethod
    //2 - proxy table
    //3 - nil (first key)
    lua_pushcfunction(l, &_nextMetamethod);
    lua_getmetatable(l, -2);
    lua_pushnil(l);
    return 3;
}

int TkLuaTableUserdata::_ipairsMetamethod(lua_State*const l) {
    //Pushes onto Lua stack:
    //1 - default ipairs function
    //2 - proxy table
    //3 - 0 (first key)
    lua_getglobal(l, "ipairs");
    lua_getmetatable(l, -2);
    lua_pushnumber(l, 0);
    return 3;
}

int TkLuaTableUserdata::_nextMetamethod(lua_State*const l) {
    const char* keyName;

    //LuaManager::rPrintByIndex(-1, 3, "KEY");

  //  auto luaNext = [](lua_State*const l) {
        lua_getglobal(l, "next");
        lua_pushvalue(l, -3);
        lua_pushvalue(l, -3);
        lua_call(l, 2, 2);
    //};

    //luaNext(l);

    lua_pushvalue(l, -2);
    keyName = lua_tostring(l, -1);
    if(keyName && (strcmp(keyName, TK_LUA_TABLE_USERDATA_FIELD_KEY) == 0)) {
        lua_getglobal(l, "next");
        lua_pushvalue(l, -6); //metatable
        lua_pushvalue(l, -3); //key
        lua_call(l, 2, 2);
    } else {
        lua_pop(l, 1);
    }
    return 2;
}

int TkLuaTableUserdata::_lengthMetamethod(lua_State*const l) {
    lua_getmetatable(l, -1);
    lua_len(l, -1);
    return 1;
}

#if 0
bool TkLuaTableUserdata::hasTkUserdata(OOLUA::Lua_table_ref &ref) {
    lua_State* l = ref.state();
    assert(l);

    ref.push();
    lua_getmetatable(l, -1);
    if(!lua_isnil(l, -1)) {
        lua_getfield(l, -1, TK_LUA_TABLE_USERDATA_FIELD_KEY);
        lua_remove(l, -2);
    }


}

#endif
