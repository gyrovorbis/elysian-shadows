#ifndef ELYSIAN_LUA_TABLE_MODEL_H
#define ELYSIAN_LUA_TABLE_MODEL_H

#include "luatables.h"
#include "elysian_lua_manager.hpp"

namespace elysian {

#if 1

template<typename P, class... _Types> class LuaTableProxy;


/*
 * Base class for accessing, manipulating, serializing, and representing Lua tables
 * within ESTk
 */
/*    class LuaTable: public OOLUA::Lua_table_ref,
                    public LuaTables
    {


    };
*/
//T (C::*const _getAccessor)(void) const;
//void (C::*const _setAccessor)(const T);
    struct ConstExprStringLiteral {
        const char* const literal;
        constexpr inline ConstExprStringLiteral(const char* str): literal(str) {}
    };




    template <typename P, typename T, const char*const N, const char*const G, const char*const S>
    struct LuaTableFieldProxy {
        typedef LuaTableFieldProxy<P, T, N, G, S> FieldType;
        static constexpr const char *const key = N;
                static constexpr const char *const Get = G;
                        static constexpr const char *const Set = S;
        //static T (*P::*const getAccessor)(void) const = Getter;
        //static void (*const setAccessor)(const T) = Setter;

     //   static constexpr GetType getAccessor = Getter;
     //   static constexpr SetType setAccessor = Setter;

       // static T    getValue(const P*const obj) { return (obj->*getAccessor)(); }
      //  static void setValue(P*const obj, const T val) { (obj->*setAccessor)(val); }
#if 0
        static void pushVal(const P*const obj, lua_State* const state) {
            T val = getValue(obj);
            Debug::printf(Debug::VERBOSE, "VALUE = %f", val);
            OOLUA::push(state, val);
        }
    #endif
    };

    template <typename P, typename T, const char*const N, const char* G, const char* S>
    const char*const LuaTableFieldProxy<P, T, N, G, S>::key;

    template <typename P, typename T, const char*const N, const char* G, const char* S>
    const char*const LuaTableFieldProxy<P, T, N, G, S>::Get;

    template <typename P, typename T, const char*const N, const char* G, const char* S>
    const char*const LuaTableFieldProxy<P, T, N, G, S>::Set;

    template<typename P>
    class LuaTableProxy<P>: public LuaTable {
    protected:

        inline bool _pushFieldProxyValue(const char *key, lua_State *state) { return false; }
        inline void _registerFieldProxyIndex(lua_State*, const int) {

        }

        inline void _registerFieldProxyNewIndex(lua_State*, const int) {

        }

        static inline void _registerPropertyProxyField(lua_State* l, const bool get=true) {
            return;
        }
    };
    /*

    // recursive tuple definition
    template<typename P, class _This,
             class... _Rest>
      class SubProcessManager<P, _This, _Rest...>
      : public SubProcessManager<P, _Rest...>
    {
          friend _This;
          */


    template <typename P, class _This, class... _Rest>
    class LuaTableProxy<P, _This, _Rest...>:
            public LuaTableProxy<P, _Rest...> {

        typedef LuaTableProxy<P, _Rest...>          ParentType;
        typedef LuaTableProxy<P, _This, _Rest...>    ThisType;
        //friend _This;
    protected:
        static _This _MyFirst;

        static LuaTable _propertyProxyIndexTable;
        static LuaTable _propertyProxyNewIndexTable;
        static LuaTable _ooluaIndexTable;               //OOLUA's default index metamethod table
        static LuaTable _ooluaNewIndexTable;            //OOLUA's default new index metamethod table
        static LuaTable _staticMetaTable;
        LuaTable _protoTable;

        static inline void _registerPropertyProxyField(lua_State* l, const bool get=true) {
            lua_getfield(l, -2, get? _MyFirst.Get : _MyFirst.Set);
            lua_setfield(l, -2, _MyFirst.key);

            ParentType::_registerPropertyProxyField(l, get);
        }


        static inline void _registerPropertyMetaMethodTables(lua_State* l) {

            lua_getglobal(l, "Vex2");

            lua_newtable(l);
            _registerPropertyProxyField(l, true);
            _propertyProxyIndexTable.pull(l);


            lua_newtable(l);
            _registerPropertyProxyField(l, false);
            _propertyProxyNewIndexTable.pull(l);

            lua_pop(l, 1);

            assert(LuaManager::_stackDepth() == 0);

        }

        inline void _registerFieldProxyIndex(lua_State* l, const int tableIndex) {
        //    lua_pushstring(l, _MyFirst.key);
        //    lua_pushlightuserdata(l, upvalue);
        //    lua_pushcclosure(l, func, 1);
        //    lua_rawset(l, tableIndex);

          //  _registerFieldProxyIndex(tableIndex);
        }

        inline void _registerFieldProxyNewIndex(lua_State* l, const int tableIndex) {

           // _registerFieldProxyNewIndex(tableIndex);
        }

        static int _indexMetaMethodCFunction(lua_State *l) {
            return _metaMethodCFunction(l, _ooluaIndexTable, _propertyProxyIndexTable, false);
        }

        static int _newIndexMetaMethodCFunction(lua_State *l) {
            return _metaMethodCFunction(l, _ooluaNewIndexTable, _propertyProxyNewIndexTable, true);
        }


        //CRTP Overridable?
        static int _metaMethodCFunction(lua_State *l, LuaTable& ooluaIndexTable, LuaTable& propertyIndexTable, const bool newIndex) {
            char key[200];

            //stack state
            //1 - table ref
            //2 - key
            //(3) - arg [__newIndex only]

            //1) try protoTable if exists
            //2) try staticMetaTable
            //3) try propertyMetaTable if exists

            strcpy(key, lua_tostring(l, 2));
            assert(key);

          //  Vex2*obj = OOLUA::INTERNAL::check_index_no_const<Vex2>(l, 1);

            lua_remove(l, 2);

            //check the oolua methods table first!
            ooluaIndexTable.push(l);
            lua_getfield(l, -1, key);
            lua_remove(l, -2);
            if(!lua_isnil(l, -1)) {
                //++argCount;
                lua_remove(l, 1);
                Debug::printf(Debug::VERBOSE, "regular lookup!");
                LuaManager::_cStackDump();

                return 1;

            } else {

                lua_remove(l, -1);
                propertyIndexTable.push(l);
                lua_getfield(l, -1, key);
                lua_remove(l, -2);
                if(lua_isnil(l, -1)) {
                    lua_pop(l, lua_gettop(l));
                    Debug::printf(Debug::VERBOSE, "SHIT HAS TOTALLY REESTED");
                    //return 0;
                    //LOOKUP INTO OOLUA'S base checker metamethod



                } else {
                    lua_insert(l, 1);


                //lua_remove(l, 2);
                //OOLUA::push(l, obj);
               // lua_insert(l, 2);
              //  if(newIndex) lua_pushvalue(l, 3);
                LuaManager::_protectedCall(newIndex? 2 : 1, newIndex? 0 : 1, nullptr);

               Debug::printf(Debug::VERBOSE, "PROTECTED[%s] - %s", newIndex? "SET" : "GET", key);
                LuaManager::_cStackDump();


                return newIndex? 0 : 1;
                }

            }

         //   OOLUA::INTERNAL::Lua_ud* ud;
          //  OOLUA::INTERNAL::index_is_userdata(l, -2, ud);


#if 1
      //      if(reinterpret_cast<ThisType*>(obj)->_pushFieldProxyValue(key, l)) {
        //        return 1;
 //           }

     #else
            //lookup into property fields by name
            if(_pushFieldProxyValue(key, l)) {

                return 1;
                //We're done, shit should be on the stack!
            }
#endif

         //   return 0;

        }

        //static void _writeMetaMethodCFunction(blah blah);

        inline bool _pushFieldProxyValue(const char* key, lua_State* state) {
#if 0
            Debug::printf(Debug::VERBOSE, "strcmp(%s, %s)", key, _MyFirst.key);
            if(strcmp(key, _MyFirst.key) == 0) {
                Debug::printf(Debug::VERBOSE, "FOUND IT!");
                LuaManager::_cStackDump();
                _This::pushVal(static_cast<P*>(this), state);
                return true;
            }

            return ParentType::_pushFieldProxyValue(key, state);
            //return static_cast<ParentType*>(this)->_pushFieldProxyValue(key, state);
#endif
        }

    public:

        //set metatable
        //LuaTableProxy(void);
        //LuaTableProxy(const LuaTable& ref);
        //assignment operator
        //copy constructo

        void		setMetaTable(const LuaTable& ref) const;
        void		setProtoTable(const LuaTable& ref) const;
        LuaTable	getProtoTable(void) const;

        //override OOLUA's Proxy Class metatable
        //do any kind of static registration bullshit for fields
        template<typename Proxy=P>
        static void registerProxyMetaTable(lua_State *l) {
            _registerPropertyMetaMethodTables(l);

            _propertyProxyIndexTable.push(l);
            LuaManager::rPrintByIndex(-2, 999, "IndexTable");
            lua_pop(l, 1);

            _propertyProxyNewIndexTable.push(l);
            LuaManager::rPrintByIndex(-2, 999, "NewIndexTable");
            lua_pop(l, 1);

            assert(LuaManager::_stackDepth() == 0);

           // _propertyProxyTable.push(*LuaManager::_mainState);
           // LuaManager::_rPrintByIndex(-2, 999, "ProxyFieldTable");

            Debug::printf(Debug::VERBOSE, "Registering shit!");
/*
            lua_newtable(*LuaManager::_mainState);
            lua_pushcfunction(l, &_indexMetaMethodCFunction);
            lua_setfield(l, -2, "__index");

            LuaManager::rPrintByIndex(-2, 999, "instance meta");
            _staticMetaTable.pull(*LuaManager::_mainState);

            assert(LuaManager::_stackDepth() == 0);

            Debug::printf(Debug::VERBOSE, "Created metatable!"); */


            assert(OOLUA::INTERNAL::class_name_is_already_registered(*LuaManager::_mainState, OOLUA::Proxy_class<Proxy>::class_name));

//          lua_getglobal(*LuaManager::_mainState, OOLUA::Proxy_class<Proxy>::class_name);
            luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<Proxy>::class_name); //userdata metatable

            lua_getfield(l, -1, "__index");
            _ooluaIndexTable.pull(l);

            lua_getfield(l, -1, "__newindex");
            _ooluaNewIndexTable.pull(l);

            lua_pushcfunction(l, &_indexMetaMethodCFunction);
            lua_setfield(l, -2, "__index");
           // LuaManager::rPrintByIndex(-2, 999, "orig");

            lua_pushcfunction(l, &_newIndexMetaMethodCFunction);
            lua_setfield(l, -2, "__newindex");

            lua_pop(*LuaManager::_mainState, 1);

          //  _staticMetaTable.push(*LuaManager::_mainState);
           // lua_setmetatable(*LuaManager::_mainState, -2);
           // LuaManager::_rPrintByIndex(-2, 999, "new");

/*
            _staticMetaTable.push(*LuaManager::_mainState);
            luaL_setmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<Proxy>::class_name);
            lua_pop(*LuaManager::_mainState, 1);

            luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<Proxy>::class_name);
            LuaManager::_rPrintByIndex(-2, 999, "NEW");
            lua_pop(*LuaManager::_mainState, 1);
*/


            //TEST TABLEZ
#if 0
            lua_newtable(*LuaManager::_mainState);
            lua_pushvalue(*LuaManager::_mainState, -1);

            OOLUA::Lua_table_ref table;
            OOLUA::pull(*LuaManager::_mainState, table);

            Debug::printf(Debug::VERBOSE, "Created new table!");


            double width;

            _staticMetaTable.push(*LuaManager::_mainState);
            lua_setmetatable(*LuaManager::_mainState, -2);
            lua_getfield(l, -1, "width");
            OOLUA::pull(l, width);

            Debug::printf(Debug::VERBOSE, "width = %d", width);

#endif


            assert(LuaManager::_stackDepth() == 0);

        }

    };


    template <typename P, class _This, class... _Rest>
    LuaTable LuaTableProxy<P, _This, _Rest...>::_staticMetaTable;

    template <typename P, class _This, class... _Rest>
    LuaTable LuaTableProxy<P, _This, _Rest...>::_ooluaIndexTable;

    template <typename P, class _This, class... _Rest>
    LuaTable LuaTableProxy<P, _This, _Rest...>::_ooluaNewIndexTable;

    template <typename P, class _This, class... _Rest>
    LuaTable LuaTableProxy<P, _This, _Rest...>::_propertyProxyIndexTable;

    template <typename P, class _This, class... _Rest>
    LuaTable LuaTableProxy<P, _This, _Rest...>::_propertyProxyNewIndexTable;





}



/*

// tuple_element
template<size_t _Index, class _ProcManager> struct SubProcessManagerSubProcessElement;

// select first element
template<typename P, class _This, class... _Rest>
struct SubProcessManagerSubProcessElement<0, SubProcessManager<P, _This, _Rest...>>
{
  typedef _This& type;
  typedef SubProcessManager<P, _This, _Rest...> _Ttype;
};

// recursive tuple_element definition
template <size_t _Index, class P, class _This, class... _Rest>
struct SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _This, _Rest...>>
  : public SubProcessManagerSubProcessElement<_Index - 1, SubProcessManager<P, _Rest...> >
{
};


// get reference to _Index element of tuple
template<size_t _Index, typename P, class... _Types> inline
  typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _Types...>>::type
  get(SubProcessManager<P, _Types...>& _SubProcessManager)
{
  typedef typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _Types...>>::_Ttype _Ttype;
  return (((_Ttype&) _SubProcessManager)._Myfirst);
}



*/





















#else








    #define ELYSIAN_MAP_ENTRY_SET_ATTRIB(p, g, s) AbstractProperty<##p##, decltype(p), decl \
                                                    value &= ~(ELYSIAN_##t##_ENTRY_##a##_BIT_MASK << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
                                                    (value) |= ((unsigned)(v) << ELYSIAN_##t##_ENTRY_##a##_BIT_POS)

    template <const char* TypeName, /*int TypeIndex,*/ typename Type, typename GetType, GetType Getter, typename SetType, SetType Setter>
    struct AbstractProperty {
        typedef                 Type        PropertyType;
        static const char       name[]      = TypeName;
        static const int        index       = TypeIndex;
        static const GetType*   getter      = Getter;
        static const SetType*   setter      = Setter;
        T*                      obj         = nullptr;

        T       getValue(void) const;
        void    setValue(const T);
    };

    template<typename P>
    class AbstractModel<P> {

    };
    
    class AbstractProperty<"", 
    

    template<typename P, class _This, class... _Rest>
    class AbstractModel: public AbstractModel<P, _Rest...>,
                        public AbstractProperty<""> {
        friend _This;
    public:
        _This _MyFirst;

    protected:
        typedef AbstractModel<P, _This, _Rest> AbstractModelParent;

        bool saveModel(FILE*const file);
        bool loadModel(FILE*const file);

        template<typename T>
        T getProperty(const char*const name) const;

        template<typename T>
        void setProperty(const char*const name, const T val);
        
        Lua_table_t 
        
        

    };

    class LuaTableModel: public AbstractModel, public LuaTable {
    private:

    public:

        //LuaTableModel()

        LuaTableModel getParentModel(void) const;
        LuaTableModel getChildModel(const char* key) const;

        LuaTableModel pushModel(const char* key) const;
        LuaTableModel popModel(void) const;

        virtual bool saveModel(FILE*const file)  override;
        virtual bool loadModel(FILE*const file) override;

        virtual void updateModel(void) const {}
        virtual void doneLoadingModel(void) const {}

        const char* getKey(void) const;

    };

}

#endif

#endif // ELYSIAN_LUA_TABLE_MODEL_H
