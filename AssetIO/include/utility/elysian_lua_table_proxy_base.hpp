#ifndef ELYSIAN_LUA_TABLE_PROXY_BASE_HPP
#define ELYSIAN_LUA_TABLE_PROXY_BASE_HPP

#include <utility/luatables.h>
#include <elysian_lua_manager.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_lua_variant.hpp>

//DSL for autogenerating proxy fields
#define ELYSIAN_LUA_PROXY_FIELDS_BEGIN() \
    public:                                         \
    static bool                                     _proxyRegistered; \
    static constexpr elysian::LuaTableFieldProxy    _proxyFields[] = {

#define ELYSIAN_LUA_PROXY_FIELDS_END() }; \
    static constexpr unsigned _proxyFieldCount = sizeof(_proxyFields) / sizeof(elysian::LuaTableFieldProxy);
#if 0
#define ELYSIAN_LUA_PROXY_PARENTS(...) \
    static constexpr const char*const   _proxyParents[] = { ELYSIAN_STRINGIFY_MACRO(__VA_ARGS__) }; \
    static constexpr const unsigned     _proxyParentCount = sizeof(_proxyParents) / sizeof(char*);
#endif

#define ELYSIAN_LUA_PROXY_FIELD_READ_WRITE(f, g, s) { #f, #g, #s, nullptr, elysian::LuaTableFieldProxy::FLAG_NONE },
#define ELYSIAN_LUA_PROXY_FIELD_READ_ONLY(f, g) { #f, #g, nullptr, nullptr, elysian::LuaTableFieldProxy::FLAG_NONE },

#define ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_FIELD(...) ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(f) { #f, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_BEGIN },
#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END() { nullptr, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_END },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE(f, g, s, c) { #f, #g, #s, #c, LuaTableFieldProxy::FLAG_ARRAY },
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY(f, g, c) { #f, #g, nullptr, #c, LuaTableFieldProxy::FLAG_ARRAY },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD(...) ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_FIELDS_EXPORT(c) \
    bool                                    c::_proxyRegistered = false; \
    constexpr unsigned                      c::_proxyFieldCount; \
    constexpr elysian::LuaTableFieldProxy	c::_proxyFields[];


#define ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE    100
#define ELYSIAN_LUA_PROXY_FIELDS_MAX            100
#define ELYSIAN_LUA_PROXY_TO_STRING_BUFF_SIZE   100

#ifndef ELYSIAN_ENGINE
template<typename P, typename T, class... Bases>
class TkLuaTableProxy;

namespace elysian {
    template <typename P, typename T=P, class... Bases>
    using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;
}

template <typename P, typename T=P, class... Bases>
using LuaTableProxy = TkLuaTableProxy<P, T, Bases...>;
#else
namespace elysian {
    template<typename P, typename T, class... Bases>
    class LuaTableProxyBase;

    template <typename P, typename T=P, class... Bases>
    using LuaTableProxy = LuaTableProxyBase<T, P, Bases...>;
}
#endif

namespace elysian {

    enum LUA_TABLE_PROXY_TYPE {
        LUA_TABLE_PROXY_NORMAL,
        LUA_TABLE_PROXY_VIRTUAL,
        LUA_TABLE_PROXY_ARRAY,
        LUA_TABLE_PROXY_TYPE_COUNT
    };

    enum LUA_TABLE_PROXY_UPVALUE {
        LUA_TABLE_PROXY_UPVALUE_GET_METHOD = 1,
        LUA_TABLE_PROXY_UPVALUE_SET_METHOD,
        LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD,
        LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX,
        LUA_TABLE_PROXY_UPVALUE_COUNT = LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX
    };

    struct LuaTableFieldProxy {
        enum FLAGS { FLAG_NONE, FLAG_VTABLE_BEGIN, FLAG_VTABLE_END, FLAG_ARRAY };
        const char*   key;
        const char*   get;
        const char*   set;
        const char*   size;
        int           flags;
	};

    /* Private class template for handling property registration, since C++ doesn't allow template member function
     * partial specialization. */

    //Class template for allowing CRTP-d properties to be overridden by their child properties at compile-time
    template<typename P, typename T, class... Bases>
    struct _LuaTableProxyPropertyRegistrar {
        static inline void registerProperties(lua_State*const l) {
            LuaTableProxy<P, T, Bases...>::template _registerProperties<P>(l); //register base class
            LuaTableProxy<P, T, Bases...>::template _registerProperties<T>(l); //register base's CRTP child
        }
    };

    //Partial specialization for non-CRTP classes
    template<typename P, class... Bases>
    struct _LuaTableProxyPropertyRegistrar<P, P, Bases...> {
        static inline void registerProperties(lua_State*const l) {
            LuaTableProxy<P, P, Bases...>::template _registerProperties<P>(l); //register base class
        }
    };

    template<typename P, typename T, class... Args> struct _LuaTableProxyParents;

    template<typename P, typename T>
    struct _LuaTableProxyParents<P, T>: public LuaManager {
        static int iteratorDepth;

        static bool iterateParentsIndexMetaMethods(lua_State*const /*l*/) { return false; }

        static bool iterateParentsNewIndexMetaMethods(lua_State*const /*l*/) { return false; }

        static unsigned pushParentsIndexTable(lua_State*const /*l*/) { return 0; }

        static int adjustParentIndex(const int /*startIndex*/) { return -1; }

        static void appendParentIndexTable(lua_State*const /*l*/, LuaTableFieldProxy*const /*proxy*/, int& /*totalProxies*/) {}
        static void appendParentNewIndexTable(lua_State*const /*l*/) {}

        static LUA_TABLE_PROXY_TYPE getLuaTableProxyType(void);
        static void pushUpvalues(void);
    };

    template<typename P, typename T, typename This, class... Rest>
    struct _LuaTableProxyParents<P, T, This, Rest...>:
        public _LuaTableProxyParents<P, T, Rest...>
    {

        static bool iterateParentsIndexMetaMethods(lua_State*const l) {
            return (This::_metaMethodCFunction(l, This::_ooluaIndexTable, This::_propertyProxyIndexTable, false, true))?
                true : _LuaTableProxyParents<T, P, Rest...>::iterateParentsIndexMetaMethods(l);
        }

        static bool iterateParentsNewIndexMetaMethods(lua_State*const l) {
            return (This::_metaMethodCFunction(l, This::_ooluaNewIndexTable, This::_propertyProxyNewIndexTable, true, true))?
                true : _LuaTableProxyParents<T, P, Rest...>::iterateParentsNewIndexMetaMethods(l);
        }

#if 0
        static int adjustParentIndexTable(const int startIndex) {
            if()
        }
#endif
        static void appendParentIndexTable(lua_State*const l, LuaTableFieldProxy*const proxy, int& totalProxies) {

           // assert(_LuaTableProxyParents<P, T>::template _proxyTotalCount + This::_proxyFieldCount <= ELYSIAN_LUA_PROXY_FIELDS_MAX);
            memcpy(&proxy[totalProxies], This::_proxyFields, sizeof(LuaTableFieldProxy)*This::_proxyFieldCount);
            totalProxies += This::_proxyFieldCount;

            lua_getglobal(l, "appendTable");
            lua_pushvalue(l, -2); //duplicate dest table
            lua_rawgeti(l, LUA_REGISTRYINDEX, This::_propertyProxyIndexTable);
            LuaManager::_protectedCall(2, 0, nullptr, "Lua Table Proxy - Appending parent index tables.");
            _LuaTableProxyParents<T, P, Rest...>::appendParentIndexTable(l, proxy, totalProxies);
        }

        static void appendParentNewIndexTable(lua_State*const l) {
            lua_getglobal(l, "appendTable");
            lua_pushvalue(l, -2); //duplicate dest table
            lua_rawgeti(l, LUA_REGISTRYINDEX, This::_propertyProxyNewIndexTable);
            LuaManager::_protectedCall(2, 0, nullptr, "Lua Table Proxy - Appending parent index tables.");
            _LuaTableProxyParents<T, P, Rest...>::appendParentNewIndexTable(l);
        }

        static bool iterateParentsNextMetaMethods(lua_State*const l, const int depth) {
            if(_LuaTableProxyParents<P, T>::iteratorDepth > depth) {
                return _LuaTableProxyParents<P, T, Rest...>::iterateParentsNextMetaMethods(l, depth+1);
            } else if(_LuaTableProxyParents<P, T>::iteratorDepth == depth) {
                if(This::_nextMetaMethodCFunction(l) == 2) {
                    return true;
                } else  {
                    ++_LuaTableProxyParents<P, T>::iteratorDepth;
                    return _LuaTableProxyParents<P, T, Rest...>::iterateParentsNextMetaMethods(l, depth+1);
                }
                return false; //WTF?
            } else return false;\


        }
    };



    /* P    - Proxied Class (or CRTP base)
     * T    - (optional CRTP child class)
     * Args - (optional parent proxy classes for property inheritance)
	 */
    template <typename P, typename T=P, class... Bases>
    class LuaTableProxyBase:
            public _LuaTableProxyParents<P, T, Bases...>
    {
        friend _LuaTableProxyPropertyRegistrar<P, T, Bases...>;

    protected:
        mutable int     _luaRef = LUA_NOREF;
    public:
        typedef P ParentType;
        typedef LuaTableProxy<P, T, Bases...> ProxyType;

        static bool     _initializing;
        static int  _propertyProxyIndexTable;
        static int _propertyProxyNewIndexTable;
        static int _ooluaIndexTable;
        static int _ooluaNewIndexTable;
#if 0
        static LuaTable _propertyProxyIndexTable;
		static LuaTable _propertyProxyNewIndexTable;
		static LuaTable _ooluaIndexTable;               //OOLUA's default index metamethod table
		static LuaTable _ooluaNewIndexTable;            //OOLUA's default new index metamethod table
#endif
        static LuaTableFieldProxy _combinedFields[ELYSIAN_LUA_PROXY_FIELDS_MAX];
        static int      _proxyTotalCount;
        //static LuaTable _staticMetaTable;
        //LuaTable        _protoTable;


        template <typename C>
        static inline void _registerProperties(lua_State* l);


        static inline int _indexVirtualTableProxyMetamethod(lua_State* l) {

            //we must be accessing an array proxy
            if(!lua_isnil(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD))) {
                if(lua_isnumber(l, -1)) {
                    lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_GET_METHOD));
                    lua_getfield(l, -3, "__ud");
                    GY_ASSERT(!lua_isnil(l, -1));
                    int index = lua_tointeger(l, -3);
                    lua_pushinteger(l, --index);                    //Lua property is 1-based, C accessor is 0-based.
                    _callPropertyProxyMethod(false, 2, 1, -4);
                    return 1;
                } else {
                    lua_rawget(l, -2);
                    return 1;
                }

            } else { //default access logic
                lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_GET_METHOD));
                lua_pushvalue(l, -2);
                lua_gettable(l, -2);
                //assert(!lua_isnil(l, -1));
                if(!lua_isnil(l, -1)) {
                    lua_getfield(l, -4, "__ud");
                    GY_ASSERT(!lua_isnil(l, -1));
                    _callPropertyProxyMethod(false, 1, 1, -4);
                }
                return 1;

            }
        }

        //used for virtual tables AND array tables
        static inline int _newIndexVirtualTableProxyMetamethod(lua_State* l) {

            //We must be accessing an array proxy
            if(!lua_isnil(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD))) {
                if(lua_isnumber(l, -2)) {
                    //_cStackDump();
                    lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SET_METHOD));
                    lua_getfield(l, -4, "__ud");
                    GY_ASSERT(!lua_isnil(l, -1));
                    lua_pushvalue(l, -4);
                    int index = lua_tointeger(l, -1);
                    lua_pop(l, 1);
                    lua_pushinteger(l, --index);   //Lua property is 1-based, C accessor is 0-based.
                    lua_pushvalue(l, -4);
                    //_cStackDump();
                    _callPropertyProxyMethod(true, 3, 0, -6);
                } else {    //just insert the bastard otherwise
                    lua_rawset(l, -3);
                }
                return 0;
            } else { //default accessor logic
                lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SET_METHOD));
                lua_pushvalue(l, -3);
                lua_gettable(l, -2);
                if(!lua_isnil(l, -1)) {
                    lua_getfield(l, -5, "__ud");
                    lua_pushvalue(l, -4);
                    _callPropertyProxyMethod(true, 2, 0, -6);
                } else {    //insert into table!
                    lua_pop(l, 1);
                    lua_pushvalue(l, -3);
                    lua_pushvalue(l, -3);
                    lua_rawset(l, -6);
                }
            }
            return 0;
        }


        inline static int _getNextKey(lua_State* l) {
            if(_proxyTotalCount <= 0) return 0;

            char    key[ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE];
            int     parentIndex     = lua_tointeger(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX)); //-1 means parent is top-level
            //int     currentIndex    = -1;
            int     currentIndex    = parentIndex;
            int     nextIndex       = -1;
            int     depth           = 0;
            int     currentDepth    = 0;

            auto findNextIndexAtDepth = [&](const int startIndex, const int startDepth, int &currentDepth) {

                for(int i = startIndex+1; i < _proxyTotalCount; ++i) {
                    //if(!LuaManager::isSerializing() || _combinedFields[i].set) {
                        switch(_combinedFields[i].flags) {
                        case LuaTableFieldProxy::FLAG_VTABLE_BEGIN:
                            if(currentDepth == (int)startDepth) {
                                ++currentDepth;
                                return i;
                            }
                            break;
                        case LuaTableFieldProxy::FLAG_VTABLE_END:
                            if(--currentDepth < (int)startDepth) return -1;
                            break;
                        case LuaTableFieldProxy::FLAG_ARRAY:
                        case LuaTableFieldProxy::FLAG_NONE:
                            if(currentDepth == (int)startDepth)
                                if(!LuaManager::isSerializing() || _combinedFields[i].set) return i;
                        default: break;
                        }
                    //}
                }
                return -1;
            };

            //figure out table depth of parent table
            for(int i = 0; i <= parentIndex; ++i) {
                switch(_combinedFields[i].flags) {
                case LuaTableFieldProxy::FLAG_VTABLE_BEGIN:
                    ++depth;
                    break;
                case LuaTableFieldProxy::FLAG_VTABLE_END:
                    --depth;
                case LuaTableFieldProxy::FLAG_ARRAY:
                case LuaTableFieldProxy::FLAG_NONE:
                default: break;
                }
            }

            currentDepth = depth;

            if(lua_isstring(l, -1)) {
                strcpy(key, lua_tostring(l, -1));
                lua_pop(l, 1);

                bool found          = false;
                while((currentIndex = findNextIndexAtDepth(currentIndex, depth, currentDepth)) != -1) {
                    if(currentIndex >= _proxyTotalCount) break;
                    if(strcmp(key, _combinedFields[currentIndex].key) == 0) {
#if 1
                        nextIndex = findNextIndexAtDepth(currentIndex, depth, currentDepth);
                        found = true;
                        break;
#else
                        found = true;
                        nextIndex = currentIndex;
                        for(;;) {
                            nextIndex = findNextIndexAtDepth(nextIndex, depth, currentDepth);
                            //Skip read-only fields if we're serializing!
                            if(!LuaManager::isSerializing() || nextIndex == -1 || nextIndex >= _proxyTotalCount || _combinedFields[nextIndex].set) {
                                break;
                            }
                        }
#endif

                    }
                    if(found) break;
                }

                //Never even found the starting key! Must be invalid, and it ain't NIL!
                if(!found) {
                    GY_ASSERT(0);
                    return 0;
                //We're at the end!
                } else if(nextIndex == -1 || nextIndex >= _proxyTotalCount) {
                    lua_pushnil(l);
                    return 1;
                //Return next key
                } else {
                    //qDebug() << "NEXT KEY " << P::_proxyFields[index+1].key;
                    lua_pushstring(l, _combinedFields[nextIndex].key);
                    return 1;
                }
            //assume this is a nil index, and we want the first key.
            } else {
                //qDebug() << "FIRST KEY " << P::_proxyFields[0].key;
                lua_pop(l, 1);
                //depth 0's parent is implicitly -1
                nextIndex = findNextIndexAtDepth(currentIndex, depth, currentDepth);
               // lua_pushstring(l, _combinedFields[parentIndex+1].key); //Unless you did something fucktarded (empty table), this should be a valid index
                lua_pushstring(l, _combinedFields[nextIndex].key);
                return 1;
            }
        }

        static inline int _nextMetaMethodCFunction(lua_State*const l) {
            const bool isVirtual = (lua_tointeger(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX)) != -1);   //check if entry has a real parent
            const bool isArray   = (!lua_isnil(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD)));
            const bool firstPass = lua_isnil(l, -1);

            /*
            if(firstPass) {
                _LuaTableProxyParents<P, T>::iteratorDepth = 0;
            } else {
                if(_LuaTableProxyParents<Bases...>::iterateParentsNextMetaMethods(l, 0)) return 2;
            }*/


            if(isArray) {
                int index       = firstPass? 0 : lua_tonumber(l, -1);

                lua_getfield(l, -2, "__ud");                //this ptr

                lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD));      //size function
                lua_pushvalue(l, -2);                       //this ptr

                GY_ASSERT(!lua_isnil(l, -1));      //ARRAY WITH NO SIZE FUNCTION, FOOL!
                LuaManager::_protectedCall(1, 1, nullptr);              //get array size

                int size = lua_tointeger(l, -1);

                if(index >= size) {
                    lua_pop(l, lua_gettop(l));
                    return 0;
                } else {
                    lua_pop(l, 1);
                }

                lua_pushinteger(l, ++index);                //new index key
                lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_GET_METHOD));      //get function
                lua_pushvalue(l, -3);                       //this ptr arg1
                lua_pushinteger(l, index-1);                //index arg2 (C arrays are zero based)
                LuaManager::_protectedCall(2, 1, nullptr);              //this->getByIndex(index);

                return 2; // return <key, value> pair

            } else {

                if(lua_isnil(l, -2)) {
                   // qDebug() << "__NEXT called with NIL table";
                    lua_pop(l, 2);
                    return 0;
                } else {
                   // qDebug() << "_nextMetaMethodCFunction()";
                }

#if 0
                lua_getglobal(*_mainState, "proxyNext");
                lua_pushvalue(*_mainState, -3);     //table
                lua_pushvalue(*_mainState, -3);     //key
                const unsigned varArgs = _LuaTableProxyParents<Bases...>::pushParentsIndexTable(*_mainState);
                _ooluaIndexTable.push(*_mainState);
                LuaManager::_protectedCall(3+varArgs, 2, nullptr, "LuaTableProxy::next()");
                if(!lua_isnil(*_mainState, -1)) {
#else

                if(_getNextKey(l)) {
#endif
                   // qDebug() << "getNextKey() - true";
                    lua_pushvalue(l, -1);

                    lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_GET_METHOD));
                    //_propertyProxyIndexTable.push(l);
                    lua_insert(l, -2);
                    lua_gettable(l, -2);
                    //lua_rawget(l, -2);

                    if(!lua_isnil(l, -1)) {
                      //  qDebug() << "found field!";
                        lua_pushvalue(l, -4);
                        if(isVirtual) {
                            lua_getfield(l, -1, "__ud");
                            lua_remove(l, -2);
                        }

                        if(!LuaManager::_protectedCall(1, 1, nullptr)) {
                            return 0;
                        }

                        lua_remove(l, -4);
                        lua_remove(l, -2);

                        return 2;
                    } else;// qDebug() << "couldn't find field!";
                } else {
                 //   qDebug() << "getNextKey() - FALSE";
                }

                lua_pop(l, lua_gettop(l));
                return 0;

            }
        }

        //used as the set/create function for virtual tables AND array tables
        static inline int _setVirtualTableProxy(lua_State* l) {
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_luaFromTableCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_createVirtualTableProxy, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_pushvalue(l, -4);
            LuaManager::_protectedCall(1, 1, nullptr);      //create proxy table
            lua_pushvalue(l, -3);                           //push valueTable
            LuaManager::_protectedCall(2, 0, nullptr);      //iterate over valueTable, copying values
            return 0;
        }

        //metatable is upvalue 1
        //userdata ptr is arg 1
        static inline int _createVirtualTableProxy(lua_State* l) {
            lua_newtable(l);
            lua_pushvalue(l, -2);
            lua_setfield(l, -2, "__ud");
            lua_newtable(l);
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_indexVirtualTableProxyMetamethod, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__index");
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_newIndexVirtualTableProxyMetamethod, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__newindex");
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_nextMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__next");
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_pairsMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__pairs");
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_lengthMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__len");
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_luaToStringMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__tostring");
            //SET ANY OTHER METAMETHODS
            lua_setmetatable(l, -2);

            return 1;
        }

        static inline  void _callPropertyProxyMethod(const bool /*write*/, const unsigned args, const unsigned retVals, const int keyIndex) {
            lua_remove(*LuaManager::_mainState, keyIndex);
            LuaManager::_protectedCall(args, retVals, nullptr);
        }

        static inline void  _registerPropertyMetaMethodTables(lua_State* l);

        inline static int _iNextMetaMethodCFunction(lua_State* l) {
            if(lua_isnil(l, -2)) {
				//qDebug() << "__NEXT called with NIL";
                lua_pop(l, 2);
                return 0;
            } else {
				//qDebug() << "_iNextMetaMethodCFunction()";
            }

            unsigned index = lua_tonumber(l, -1);
			//qDebug() << "IPAIRS INDEX " << index;
            lua_pop(l, 1);
            if(++index > (unsigned)_proxyTotalCount) {
                lua_pop(l, 1);
                lua_pushnil(l);
                return 1;
            }

            lua_pushnumber(l, index);

            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_pushstring(l, _combinedFields[index-1].key);

            lua_rawget(l, -2);

            if(!lua_isnil(l, -1)) {
				//qDebug() << "found field!";
                lua_pushvalue(l, -4);

                LuaManager::_protectedCall(1, 1, nullptr);

                lua_remove(l, -4);
                lua_remove(l, -2);

                return 2;
            } else {
			   // qDebug() << "COULDNT FIND FIELD";
            }

            lua_pop(l, lua_gettop(l));
            return 0;

        }

        static int _lengthMetaMethodCFunction(lua_State* l) {
            const auto type = _LuaTableProxyParents<P,T>::getLuaTableProxyType();

            switch(type) {
            default:
                lua_pushinteger(l, 0);
                break;
            case LUA_TABLE_PROXY_ARRAY:

                lua_getfield(l, -1, "__ud");                //this ptr
                GY_ASSERT(!lua_isnil(l, -1)); //no userdata ptr for array proxy

                lua_pushvalue(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD));      //size function
                GY_ASSERT(!lua_isnil(l, -1)); //no sizef unction for array proxy
                lua_pushvalue(l, -2);                       //this ptr

                LuaManager::_protectedCall(1, 1, nullptr); //put array size on stack
                break;

            }
            return 1;
        }

		static int _indexMetaMethodCFunction(lua_State* l) {
            return  _metaMethodCFunction(l, _ooluaIndexTable, _propertyProxyIndexTable, false, false)? 1 : 0;
		}

		static int _newIndexMetaMethodCFunction(lua_State* l) {
            _metaMethodCFunction(l, _ooluaNewIndexTable, _propertyProxyNewIndexTable, true, false);
            return 0;
		}

		static int _pairsMetaMethodCFunction(lua_State* l) {
			//Pushes onto Lua stack:
			//1 - Userdata iterator function
			//2 - Userdata
			//3 - nil (first key)
            //lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _propertyProxyIndexTable.ref());
#if 1
            _LuaTableProxyParents<P, T>::pushUpvalues();
            lua_pushcclosure(l, &_nextMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
			lua_insert(l, 1);
			lua_pushnil(l);
#else
            lua_getglobal(l, "next");
            _propertyProxyIndexTable.push(l);
            lua_pushnil(l);
#endif
            return 3;
		}

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

    public:

		//CRTP Overridable?
        static bool _metaMethodCFunction(lua_State* l, const int ooluaIndexTable, const int propertyIndexTable, const bool newIndex, const bool recursing=false) {
            //char key[ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE];

			//stack state
			//1 - table ref
			//2 - key
			//(3) - arg [__newIndex only]

			//1) try protoTable if exists
			//2) try staticMetaTable
			//3) try propertyMetaTable if exists

            //strcpy(key, lua_tostring(l, 2));
            //assert(key);

            //lua_remove(l, 2);

			//check the oolua methods table first!
            if(!recursing) {
                lua_rawgeti(l, LUA_REGISTRYINDEX, ooluaIndexTable);
                lua_pushvalue(l, newIndex? -3 : -2);
                lua_gettable(l, -2);
                lua_remove(l, -2);


                //key found in OOLUA's methods table
                if(!lua_isnil(l, -1)) {
                    lua_remove(l, 1);
                    return true;           //Must have been an OOLUA proxy method lookup.
                }

                lua_pop(l, 1);

            }

            //key not found, try property proxy tables
            lua_rawgeti(l, LUA_REGISTRYINDEX, propertyIndexTable);
            lua_pushvalue(l, newIndex? -3 : -2);
            lua_gettable(l, -2);
            lua_remove(l, -2);

            //key not found here either, we ain't got it at this level -- LOOK IN PARENTS' SHIT
            if(lua_isnil(l, -1)) {
                lua_pop(l, 1);
                //return false;
                return newIndex? _LuaTableProxyParents<P, T, Bases...>::iterateParentsNewIndexMetaMethods(l) : _LuaTableProxyParents<P, T, Bases...>::iterateParentsIndexMetaMethods(l);

            } else {
                //lua_remove(l, 1);
                lua_insert(l, 1);   //push get/set function to bottom of stack

                //lua_remove(l, 3);   //remove key from stack ESTK'S METHOD EXPECTS THIS TO BE HERE

                //userdata is index 2
               //  OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));

                //Invoke proxy accessor method
                LuaTableProxy<P, T, Bases...>::_callPropertyProxyMethod(newIndex, newIndex? 2 : 1, newIndex? 0 : 1, 3);

                return true;

            }

		}


	public:
        int         getLuaTableRef(void) const;
		void		setMetaTable(const LuaTable& ref) const;
		void		setProtoTable(const LuaTable& ref) const;
		LuaTable	getProtoTable(void) const;

		//override OOLUA's Proxy Class metatable
		//do any kind of static registration bullshit for fields
		static void registerProxyMetaTable(lua_State *l) {
            P::_proxyRegistered             = false;
            T::_proxyRegistered             = false;
            _propertyProxyIndexTable        = LUA_REFNIL;
            _propertyProxyNewIndexTable     = LUA_REFNIL;

            //Register properties
			_registerPropertyMetaMethodTables(l);

            //Inherit from parent tables
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            _proxyTotalCount = P::_proxyFieldCount;
            memcpy(_combinedFields, P::_proxyFields, sizeof(LuaTableFieldProxy)*P::_proxyFieldCount);
            _LuaTableProxyParents<P, T, Bases...>::appendParentIndexTable(l, _combinedFields, _proxyTotalCount);
            lua_pop(l, 1);

            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            _LuaTableProxyParents<P, T, Bases...>::appendParentNewIndexTable(l);
            lua_pop(l, 1);

            GY_ASSERT(OOLUA::INTERNAL::class_name_is_already_registered(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name));

			luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name); //userdata metatable

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__index");

            //metatable["fromTable"] = _luaFromTableCFunction
            lua_pushstring(l, "fromTable");
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            lua_pushnil(l);
            lua_pushinteger(l, -1);
            lua_pushcclosure(l, &_luaFromTableCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_rawset(l, -3);

            //metatable["__type"] = class name
            lua_pushstring(l, ELYSIAN_LUA_PROXY_TYPE_METAFIELD);
            lua_pushstring(l, OOLUA::Proxy_class<T>::class_name);
            lua_rawset(l, -3);

            //metatable["__downcast"] = T::_lua_downCast()
            lua_pushstring(l, ELYSIAN_LUA_PROXY_DOWN_CAST_METAFIELD);
            lua_pushcfunction(l, &T::lua_downCast);
            lua_rawset(l, -3);

            //replace OOLUA constructor with custom one
            lua_getfield(l, -1, "new"); //only register creator function if constructor is exposed
            if(!lua_isnil(l, -1)) {
                lua_pushstring(l, "oolua_new");
                lua_insert(l, -2);
                lua_rawset(l, -3);

                lua_pushstring(l, "new");
                lua_pushcfunction(l, &_luaCreate);
                lua_rawset(l, -3);
            } else lua_pop(l, 1);

            _ooluaIndexTable = luaL_ref(l, LUA_REGISTRYINDEX);

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__newindex");
            _ooluaNewIndexTable = luaL_ref(l, LUA_REGISTRYINDEX);

            //LuaManager::_cStackDump();
            LuaTableProxy<P, T, Bases...>::_addMetamethods(l);
            //LuaManager::_cStackDump();
            //Create a new __next metamethod for internal iteration
            //_propertyProxyIndexTable.push(l);
            //lua_setfield(l, -2, "__properties");

            //lua_pushcfunction(l, &_getNextKey);
            //lua_setfield(l, -2, "__nextkey");

			lua_pop(*LuaManager::_mainState, 1);

            GY_ASSERT(LuaManager::stackDepth() == 0);

		}

        static int _luaToStringMetaMethodCFunction(lua_State*const l) {
            char        strBuff[ELYSIAN_LUA_PROXY_TO_STRING_BUFF_SIZE];
            const auto  type         = _LuaTableProxyParents<P,T>::getLuaTableProxyType();
            const int   parentIndex   = lua_tointeger(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX));

            switch(type) {
            case LUA_TABLE_PROXY_NORMAL:
                sprintf(strBuff, "userdata[%s]: proxy",  OOLUA::Proxy_class<T>::class_name);
                break;
            case LUA_TABLE_PROXY_VIRTUAL:
                sprintf(strBuff, "userdata[%s.%s]: virtual proxy",  OOLUA::Proxy_class<T>::class_name, _combinedFields[parentIndex].key);
                break;
            case LUA_TABLE_PROXY_ARRAY:
                sprintf(strBuff, "userdata[%s.%s]: array proxy",  OOLUA::Proxy_class<T>::class_name, _combinedFields[parentIndex].key);
                break;
            default:
                GY_ASSERT(0);
                break;
            }


            lua_pushstring(l, strBuff);
            return 1;
        }


        static void _addMetamethods(lua_State* l) {
            //Use our __index metamethod
            lua_pushcfunction(l, &_indexMetaMethodCFunction);
            lua_setfield(l, -2, "__index");

            //Use our __newindex metamethod
            lua_pushcfunction(l, &_newIndexMetaMethodCFunction);
            lua_setfield(l, -2, "__newindex");

            //Use our __pairs metamethod
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            lua_pushnil(l);
            lua_pushinteger(l, -1);
            lua_pushcclosure(l, &_pairsMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__pairs");

            lua_pushcfunction(l, &_ipairsMetaMethodCFunction);
            lua_setfield(l, -2, "__ipairs");

            //Create a new __next metamethod for internal iteration
            //lua_pushcfunction(l, &_nextMetaMethodCFunction);
            //lua_setfield(l, -2, "__next");

            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            lua_pushnil(l);
            lua_pushinteger(l, -1);
            lua_pushcclosure(l, &_lengthMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__len");

            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            lua_pushnil(l);
            lua_pushinteger(l, -1);
            lua_pushcclosure(l, &_luaToStringMetaMethodCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_setfield(l, -2, "__tostring");

            //lua_pushstring(l, OOLUA::Proxy_class<T>::class_name);
            //lua_setfield(l, -2, "__typename");

        }

        static int _luaFromTableCFunction(lua_State*const l) {
            //STACK[-2] = proxy table/ud
            //STACK[-1] = value table

            LuaManager::stackBalanceBegin();

            const LUA_TABLE_PROXY_TYPE tableType = _LuaTableProxyParents<P,T>::getLuaTableProxyType();
            const int startIndex = lua_tointeger(l, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX));

            bool exitLoop = false;

           // dbgprintf("<%d, %s>", i, _combinedFields[i].key);

            switch(tableType) {
            case LUA_TABLE_PROXY_NORMAL:
            case LUA_TABLE_PROXY_VIRTUAL:
                {
#if 0
                    DbgEvent event("Deserializing %s Lua Proxy table", tableType == LUA_TABLE_PROXY_NORMAL? "Normal" : "Virtual");
#endif
                    int depth = 1;
                    for(int i = startIndex+1; i < _proxyTotalCount && !exitLoop; ++i) {
                        switch(_combinedFields[i].flags) {
                        case LuaTableFieldProxy::FLAG_VTABLE_END:
                            if(--depth < 1) exitLoop = true;
                            break;
                        default:
                            if(depth == 1) {
                                lua_getfield(l, -1, _combinedFields[i].key);
                                if(!lua_isnil(l, -1)) {
#if 0
                                    lua_pushvalue(l, -1);
                                    dbgprintf("[%d] = <%s, %s>", i, _combinedFields[i].key, luaL_tolstring(l, -1, nullptr));
                                    lua_pop(l, 2);
#endif
                                    lua_setfield(l, -3, _combinedFields[i].key);
                                } else {
                                    lua_pop(l, 1);
                                }
                            }
                            if(_combinedFields[i].flags == LuaTableFieldProxy::FLAG_VTABLE_BEGIN) ++depth;
                        }
                    }
                }
                break;
            case LUA_TABLE_PROXY_ARRAY:
            default:
                {
#if 0
                    DbgEvent event("Deserializing Array Lua Proxy table");
#endif
                    lua_len(l, -1);
                    const int length = lua_tointeger(l, -1);
                    lua_pop(l, 1);

                    for(int i = 1; i <= length; ++i) {
                        lua_pushnumber(l, i);
                        lua_gettable(l, -2);

#if 0
                        lua_pushvalue(l, -1);
                        dbgprintf("<%d, %s>", i, luaL_tolstring(l, -1, nullptr));
                        lua_pop(l, 2);
#endif

                        lua_pushnumber(l, i);
                        lua_insert(l, -2);
                        lua_settable(l, -4);
                    }

                }
                break;
            }

            LuaManager::stackBalanceEnd();
            lua_pop(l, 2);
            return 0;
        }

        void fromTable(OOLUA::Lua_table_ref ref) {
            _initializing = true;
            lua_State*const state = *LuaManager::_mainState;

            OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));
            //OOLUA::push(state, static_cast<T*>(this));
            ref.push(state);

            lua_rawgeti(state, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
            lua_rawgeti(state, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
            lua_pushnil(*LuaManager::_mainState);
            lua_pushinteger(state, -1);
            lua_pushcclosure(*LuaManager::_mainState, &_luaFromTableCFunction, LUA_TABLE_PROXY_UPVALUE_COUNT);
            lua_insert(state, -3);
            LuaManager::_protectedCall(2, 0, nullptr);
            _initializing = false;
        }

        /* Specialized custom Ud constructor overriding OOLUA's Ud["new"] method.
         * Automatically supports table-style constructors for property initialization
         * Calls OOLUA's original new method, also forwarding Ud arguments for copy construction
         * Supports construction in the following forms ("Entity" ud as an example):
         * 1) Entity.new()
         * 2) Entity:new()
         * 3) Entity.new({})        (initialize properties from table)
         * 4) entityObj:new()       (copy construct new entity from entityObj)
         * 5) entityObj:new({})     (copy construct new entity from entityObj then initialize properties from table)
         */
        static int _luaCreate(lua_State*const l) {
            LuaVariant          valueTable;
            LuaVariant          rvalue;
            int args     =      lua_gettop(l);
            int retCount =      1;

            for(int i = 0; i < args; ++i) {
                const int luaType = lua_type(l, -1);
                if(luaType == LUA_TTABLE) {
                    //Check whether table we were passed is a new value table or just the oolua method table (ie: Entity:new())
                    lua_rawgeti(l, LUA_REGISTRYINDEX, _ooluaIndexTable);
                    const bool isMetaTable = lua_compare(l, -1, -2, LUA_OPEQ);
                    lua_pop(l, 1);
                    if(!isMetaTable) valueTable.pull(); //Only pull table if it's a value initialization table
                    else lua_pop(l, 1);
                }
                else if(luaType == LUA_TUSERDATA) rvalue.pull();
                else lua_pop(l, 1); //ignore unknown argument type, whatever.
            }

            lua_rawgeti(l, LUA_REGISTRYINDEX, _ooluaIndexTable);
            lua_getfield(l, -1, "oolua_new");
            GY_ASSERT(!lua_isnil(l, -1));       //Could not find constructor!
            if(!rvalue.isNil()) rvalue.push();  //Push rvalue object as argument to copy constructor!

            if(!LuaManager::_protectedCall(!rvalue.isNil()? 1 : 0, 1, nullptr)) {
                errprintf("LuaTableProxy[%s]::_luaCreate(%s, %s) failed!", OOLUA::Proxy_class<T>::class_name, rvalue.getTypeString(), valueTable.getTypeString());
                retCount = 0;
            }

            //Whatever constructor that was invoked did not return a new object!
            GY_ASSERT(!lua_isnil(l, -1));

            //Initialize new object with valueTable, if supplied.
            if(!valueTable.isNil()) {
                T* obj;
                if(OOLUA::pull(l, obj)) {
                    static_cast<LuaTableProxy<P, T, Bases...>*>(obj)->fromTable(valueTable.getValue<OOLUA::Lua_table_ref>());
                    retCount = OOLUA::push(l, obj);
                } else {
                    GY_ASSERT(obj); //Could not pull correct object type from stack!
                    retCount = 0;
                }
            }

            GY_ASSERT(retCount);
            return retCount;
        }

        static int lua_downCast(lua_State*const /*vm*/) {
            return 1; //hopefully just leave the argument on the stack as a return value?
        }

	};

    template<typename P, typename T>
    void _LuaTableProxyParents<P, T>::pushUpvalues(void) {
        for(unsigned i = 1; i <= LUA_TABLE_PROXY_UPVALUE_COUNT; ++i) lua_pushvalue(*_mainState, lua_upvalueindex(i));
    }


    template<typename P, typename T>
    LUA_TABLE_PROXY_TYPE _LuaTableProxyParents<P, T>::getLuaTableProxyType(void) {
        if(!lua_isnil(*_mainState, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_SIZE_METHOD))) return LUA_TABLE_PROXY_ARRAY;
        else if(lua_tointeger(*_mainState, lua_upvalueindex(LUA_TABLE_PROXY_UPVALUE_PARENT_INDEX)) != -1) return LUA_TABLE_PROXY_VIRTUAL;
        else return LUA_TABLE_PROXY_NORMAL;

    }

/*
	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_staticMetaTable;
*/

    template<typename P, typename T, class... Args>
    int LuaTableProxyBase<P, T, Args...>::_proxyTotalCount = 0;

    template<typename P, typename T, class... Args>
    LuaTableFieldProxy LuaTableProxyBase<P, T, Args...>::_combinedFields[ELYSIAN_LUA_PROXY_FIELDS_MAX] = {{
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0
    }};
#if 0
    template <typename P, typename T, class... Args>
    LuaTable LuaTableProxyBase<P, T, Args...>::_ooluaIndexTable;

    template <typename P, typename T, class... Args>
    LuaTable LuaTableProxyBase<P, T, Args...>::_ooluaNewIndexTable;

    template <typename P, typename T, class... Args>
    LuaTable LuaTableProxyBase<P, T, Args...>::_propertyProxyIndexTable;

    template <typename P, typename T, class... Args>
    LuaTable LuaTableProxyBase<P, T, Args...>::_propertyProxyNewIndexTable;

#else
    template <typename P, typename T, class... Args>
    int LuaTableProxyBase<P, T, Args...>::_ooluaIndexTable = LUA_REFNIL;

    template <typename P, typename T, class... Args>
    int LuaTableProxyBase<P, T, Args...>::_ooluaNewIndexTable = LUA_REFNIL;

    template <typename P, typename T, class... Args>
    int LuaTableProxyBase<P, T, Args...>::_propertyProxyIndexTable = LUA_REFNIL;

    template <typename P, typename T, class... Args>
    int LuaTableProxyBase<P, T, Args...>::_propertyProxyNewIndexTable = LUA_REFNIL;

#endif

    template <typename P, typename T, class... Args>
    bool LuaTableProxyBase<P, T, Args...>::_initializing = false;

    template <typename P, typename T, class... Args> template <typename C>
    inline void LuaTableProxyBase<P, T, Args...>::_registerProperties(lua_State* l) {
        unsigned tableDepth = 0;
        if(C::_proxyRegistered) return; //proxy properties have already been registered!

        LuaManager::pushGlobalField(OOLUA::Proxy_class<T>::class_name); //push OOLUA's proxy class methods table

        if(_propertyProxyIndexTable != LUA_REFNIL) lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable);
        else
            lua_newtable(l); //get table (__index metamethod for properties)

        if(_propertyProxyNewIndexTable != LUA_REFNIL) lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyNewIndexTable);
        else
            lua_newtable(l); //set table (__newindex metamethod for properties)

        //Register property pairs for index table <K=field, V=getFunc>
        for(unsigned i = 0; i < C::_proxyFieldCount; ++i) {

            switch(C::_proxyFields[i].flags) {
            case LuaTableFieldProxy::FLAG_ARRAY:
                lua_getfield(l, -3-(tableDepth*2), C::_proxyFields[i].get);         //upvalue #1, getByIndex method
                GY_ASSERT(!lua_isnil(l, -1)); //LUA GET ACCESSOR NOT FOUND!!!!
                if(C::_proxyFields[i].set) {
                    lua_getfield(l, -3-(tableDepth*2)-1, C::_proxyFields[i].set);   //upvalue #2, setByIndex method
                    GY_ASSERT(!lua_isnil(l, -1)); //LUA SET ACCESSOR NOT FOUND!!!!
                } else lua_pushnil(l);
                lua_getfield(l, -3-(tableDepth*2)-2, C::_proxyFields[i].size);    //upvalue #3 size field for arrays
                GY_ASSERT(!lua_isnil(l, -1)); //SIZE ACCESSOR NOT FOUND!!!!
                lua_pushinteger(l, i);                                              //upvalue #4, starting index of virtual table properties
                lua_pushcclosure(l, &_createVirtualTableProxy, LUA_TABLE_PROXY_UPVALUE_COUNT);
#if 0
                lua_setfield(l, -3-(tableDepth*2), C::_proxyFields[i].key);
#else
                lua_setfield(l, -3, C::_proxyFields[i].key);
#endif
                lua_getfield(l, -3-(tableDepth*2), C::_proxyFields[i].get);     //upvalue #1, getByIndex method
                GY_ASSERT(!lua_isnil(l, -1)); //LUA GET ACCESSOR NOT FOUND!!!!
                if(P::_proxyFields[i].set) {
                    lua_getfield(l, -3-(tableDepth*2)-1, C::_proxyFields[i].set); //upvalue #2, setByIndex method
                    GY_ASSERT(!lua_isnil(l, -1)); //LUA SET ACCESSOR NOT FOUND!!!!
                } else lua_pushnil(l);
                lua_getfield(l, -3-(tableDepth*2)-2, P::_proxyFields[i].size);    //upvalue #3 size field for arrays
                GY_ASSERT(!lua_isnil(l, -1)); //SIZE ACCESSOR NOT FOUND!!!!
                lua_pushinteger(l, i);                                          //upvalue #4, starting index of virtual table properties
                lua_pushcclosure(l, &_setVirtualTableProxy, LUA_TABLE_PROXY_UPVALUE_COUNT);
#if 0
                lua_setfield(l, -3-(tableDepth*2)+1, C::_proxyFields[i].key);
#else
                lua_setfield(l, -2, C::_proxyFields[i].key);
#endif
                break;
            case LuaTableFieldProxy::FLAG_VTABLE_BEGIN:
                lua_newtable(l); //__index metatable for virtual table
                lua_newtable(l); //__newindex metatable for virtual table
                lua_pushvalue(l, -2);                       //upvalue #1, __index for virtual table
                lua_pushvalue(l, -2);                       //upvalue #2 __newindex for virtual table
                lua_pushnil(l);                             //upvalue #3 size for arrays (ignored)
                lua_pushinteger(l, i);                      //upvalue #4, starting index of virtual table properties
                //upvalue #4 size field for arrays
                lua_pushcclosure(l, &_createVirtualTableProxy, LUA_TABLE_PROXY_UPVALUE_COUNT);

                ++tableDepth;
#if 0
                lua_setfield(l, -3-(tableDepth*2), C::_proxyFields[i].key);
#else
                lua_setfield(l, -5, C::_proxyFields[i].key);
#endif
                lua_pushvalue(l, -2);                       //upvalue #1, __index for virtual table
                lua_pushvalue(l, -2);                       //upvalue #2 __newindex for virtual table
                lua_pushnil(l);                             //upvalue #3 size for arrays (ignored)
                lua_pushinteger(l, i);                      //upvalue #4, starting index of virtual table properties
                lua_pushcclosure(l, &_setVirtualTableProxy, LUA_TABLE_PROXY_UPVALUE_COUNT);
#if 0
                lua_setfield(l, -3-(tableDepth*2)+1, C::_proxyFields[i].key);
#else
                lua_setfield(l, -4, C::_proxyFields[i].key);
#endif
                break;

            case LuaTableFieldProxy::FLAG_VTABLE_END:
                --tableDepth;
                lua_pop(l, 2);
                break;
            default:
                lua_getfield(l, -3-(tableDepth*2), C::_proxyFields[i].get); //get field from OOLUA methods table
                if(lua_isnil(l, -1)) {
                    errprintf("Proxy GET accessor [%s] not found!", C::_proxyFields[i].get);
                    GY_ASSERT(false);
                }

                lua_setfield(l, -3, C::_proxyFields[i].key);
                if(C::_proxyFields[i].set) {
                    lua_getfield(l, -3-(tableDepth*2), C::_proxyFields[i].set); //get field from OOLUA methods table
                    if(lua_isnil(l, -1)) {
                        errprintf("Proxy SET accessor [%s] not found!", C::_proxyFields[i].set);
                        GY_ASSERT(false);
                    }

                    lua_setfield(l, -2, C::_proxyFields[i].key);
                }
            }
        }
        GY_ASSERT(tableDepth == 0); // YOUR SHIT IS MALFORMED, NIGGER. USE MY DSL CORRECTLY FOR FUCK'S SAKE.

    //                LuaManager::rPrintByIndex(-1, 100, "__INDEX");
        _propertyProxyNewIndexTable = luaL_ref(l, LUA_REGISTRYINDEX);

      //              LuaManager::rPrintByIndex(-1, 100, "__NEWINDEX");
        _propertyProxyIndexTable = luaL_ref(l, LUA_REGISTRYINDEX);
        lua_pop(l, 1);

        C::_proxyRegistered = true;
    }

    template<typename P, typename T, class... Bases>
    inline void LuaTableProxyBase<P, T, Bases...>::_registerPropertyMetaMethodTables(lua_State* l) {
        //invoke partially specialized class template
        _LuaTableProxyPropertyRegistrar<P, T, Bases...>::registerProperties(l);
    }

    template <typename P, typename T, class... Bases>
    inline int LuaTableProxyBase<P, T, Bases...>::getLuaTableRef(void) const {
        if(_luaRef == LUA_NOREF) {
            //Cannot push a const pointer to Lua, or it'll shit every time we attempt to modify proxy from Lua runtime
            OOLUA::push(*LuaManager::_mainState, const_cast<T*>(static_cast<const T*>(this)));
            _luaRef = luaL_ref(*LuaManager::_mainState, LUA_REGISTRYINDEX);
        }
        return _luaRef;
    }



}

#endif // ELYSIAN_LUA_TABLE_PROXY_H
