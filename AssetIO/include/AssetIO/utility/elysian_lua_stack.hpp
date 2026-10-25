/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LUA_STACK_HPP
#define ELYSIAN_LUA_STACK_HPP

#include <crtp/elysian_lua_variant.hpp>



namespace elysian {

    class LuaTempVariant {


    };



//should be used by LuaManager for pushing/popping arguments to/from function calls and shit.
    class LuaStack {
    private:
        lua_State*const _state = nullptr;

    public:
        LuaStack(lua_State* vm);


    };



    //====== INLINEZ =======

    LuaStack::LuaStack(lua_State *const vm): _state(vm) {}

    int LuaStack::getTop(void) const { return lua_gettop(_state); }
    void LuaStack::clear(void) const { lua_pop(_state, lua_gettop(_state)); }

#if 0

    LuaStack stack(vm);

    luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name); //userdata metatable

    auto metaTable = stack[-1];
    auto metaIndex = stack.pushField(metaTable, "__index");

    metaIndex.setField("fromTable", &luaFromTableCFunction, _propertyProxyIndexTable, _propertyProxyNewIndexTable, nullptr, -1);
    metaIndex.setField(ELYSIAN_LUA_PROXY_TYPE_METAFIELD, OOLUA::Proxy_class<T>::class_name);
    metaIndex.setField(ELYSIAN_LUA_PROXY_DOWN_CAST_METAFIELD, &T::lua_downCast);

    metaIndex.pushField("new");
    if(!stack[-1].isNil()) {
        metaIndex.setFieldRaw("oolua_new", stack[-1]);
        metaIndex.setFieldRaw("new", &_luaCreate);
    }
    stack.pop();

    _ooluaIndexTable = metaIndex;
    _ooluaNewIndexTable = metaTable;

    stack.clear();



#endif


}

#endif // ELYSIAN_LUA_STACK_HPP
