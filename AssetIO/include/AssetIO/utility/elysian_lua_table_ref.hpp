#ifndef ELYSIAN_LUA_TABLE_REF_HPP
#define ELYSIAN_LUA_TABLE_REF_HPP

#if 0

#include <utility/elysian_lua_variant.hpp>
#include <crtp/elysian_lua_manager.hpp>


namespace elysian {

    //constructor to copy all values from stack and into lua array (variable argument types)
    class LuaTableRef: public LuaManager {
    public:



    protected:
        int             _luaRef = LUA_NOREF;


    public:


                        LuaTableRef(void) = default;                        
                        LuaTableRef(const int ref);
                        LuaTableRef(const OOLUA::Lua_table_ref& ref);
                        template<typename T>
                        LuaTableRef(T*const ref);

                        ~LuaTableRef(void);





    //C++11 iterators

    public:
       // LuaTableRef
        //to OOLUA type
        //conversions
        //load/save from file
        //invoke generic methods
        //C++/Lua virtuals

        //UNION REPRESENTING ALL LUA "VALUE" TYPES

        //REPRESENT C++-side of VIRTUALS VIA LAMBDA-VTABLE

    };

    inline bool LuaTableRef::isNilRef(void) const { return _luaRef == LUA_NOREF; }

    inline LuaTableRef::LuaTableRef(const int ref): _luaRef(ref) {}





    template<typename T>
    inline LuaTableRef::LuaTableRef(T*const ref) {
        OOLUA::push(*_mainState, ref);
        _luaRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    }

    inline LuaTableRef::~LuaTableRef(void) { luaL_unref(*_mainState, LUA_REGISTRYINDEX, _luaRef); }


}


#endif

#endif // ELYSIAN_LUA_TABLE_REF_HPP

