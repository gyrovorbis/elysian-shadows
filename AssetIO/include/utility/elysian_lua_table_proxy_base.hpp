#ifndef ELYSIAN_LUA_TABLE_PROXY_BASE_HPP
#define ELYSIAN_LUA_TABLE_PROXY_BASE_HPP

#include <luatables.h>
#include <elysian_lua_manager.hpp>

//DSL for autogenerating proxy fields
#define ELYSIAN_LUA_PROXY_FIELDS_BEGIN() \
    static constexpr LuaTableFieldProxy _proxyFields[] = {

#define ELYSIAN_LUA_PROXY_FIELDS_END() }; \
    static constexpr unsigned _proxyFieldCount = sizeof(_proxyFields) / sizeof(LuaTableFieldProxy); \

#define ELYSIAN_LUA_PROXY_FIELD_READ_WRITE(f, g, s) { #f, #g, #s, nullptr, LuaTableFieldProxy::FLAG_NONE },
#define ELYSIAN_LUA_PROXY_FIELD_READ_ONLY(f, g) { #f, #g, nullptr, nullptr, LuaTableFieldProxy::FLAG_NONE },

#define ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_FIELD(...) ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(f) { #f, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_BEGIN },
#define ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END() { nullptr, nullptr, nullptr, nullptr, LuaTableFieldProxy::FLAG_VTABLE_END },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE(f, g, s, c) { #f, #g, #s, #c, LuaTableFieldProxy::FLAG_ARRAY },
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY(f, g, c) { #f, #g, nullptr, #c, LuaTableFieldProxy::FLAG_ARRAY },

#define ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_ARRAY_FIELD(...) ELYSIAN_LUA_PROXY_ARRAY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_ARRAY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_FIELDS_EXPORT(c) \
    constexpr unsigned				c::_proxyFieldCount; \
    constexpr LuaTableFieldProxy	c::_proxyFields[];

#define ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE    100

template<typename P, typename T>
class TkLuaTableProxy;

namespace elysian {
    template <typename P, typename T=P>
    using LuaTableProxy = TkLuaTableProxy<P, T>;
}

template <typename P, typename T=P>
using LuaTableProxy = TkLuaTableProxy<P, T>;

namespace elysian {

    struct LuaTableFieldProxy {
        enum FLAGS { FLAG_NONE, FLAG_VTABLE_BEGIN, FLAG_VTABLE_END, FLAG_ARRAY };
        const char *const   key;
        const char *const   get;
        const char *const   set;
        const char*         size;
        const int           flags;
	};

	/* P - Proxy class
	 * T - Class being proxied
	 */
	template <typename P, typename T=P>
    class LuaTableProxyBase: public LuaManager {
	protected:
        typedef P ParentType;

        static bool     _initializing;
		static LuaTable _propertyProxyIndexTable;
		static LuaTable _propertyProxyNewIndexTable;
		static LuaTable _ooluaIndexTable;               //OOLUA's default index metamethod table
		static LuaTable _ooluaNewIndexTable;            //OOLUA's default new index metamethod table
        //static LuaTable _staticMetaTable;
        //LuaTable        _protoTable;


        static inline int _indexVirtualTableProxyMetamethod(lua_State* l) {

            //we must be accessing an array proxy
            if(!lua_isnil(l, lua_upvalueindex(2))) {
                if(lua_isnumber(l, -1)) {
                    lua_pushvalue(l, lua_upvalueindex(1));
                    lua_getfield(l, -3, "__ud");
                    assert(!lua_isnil(l, -1));
                    int index = lua_tointeger(l, -3);
                    lua_pushinteger(l, --index);                    //Lua property is 1-based, C accessor is 0-based.
                    _callPropertyProxyMethod(false, 2, 1, -4);
                    return 1;
                } else {
                    lua_rawget(l, -2);
                    return 1;
                }

            } else { //default access logic
                lua_pushvalue(l, lua_upvalueindex(1));
                lua_pushvalue(l, -2);
                lua_gettable(l, -2);
                //assert(!lua_isnil(l, -1));
                if(!lua_isnil(l, -1)) {
                    lua_getfield(l, -4, "__ud");
                    assert(!lua_isnil(l, -1));
                    _callPropertyProxyMethod(false, 1, 1, -4);
                }
                return 1;

            }
        }

        static inline int _newIndexVirtualTableProxyMetamethod(lua_State* l) {

            //We must be accessing an array proxy
            if(!lua_isnil(l, lua_upvalueindex(2))) {
                if(lua_isnumber(l, -2)) {
                    //_cStackDump();
                    lua_pushvalue(l, lua_upvalueindex(1));
                    lua_getfield(l, -4, "__ud");
                    assert(!lua_isnil(l, -1));
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
                lua_pushvalue(l, lua_upvalueindex(1));
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
            if(P::_proxyFieldCount <= 0) return 0;

            char    key[ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE];
            int     parentIndex     = lua_tointeger(l, lua_upvalueindex(2)); //-1 means parent is top-level
            int     currentIndex    = -1;
            int     nextIndex       = -1;
            int     depth           = 0;

            auto findNextIndexAtDepth = [&](const int startIndex, const int startDepth, int &currentDepth) {

                for(int i = startIndex+1; i < (int)P::_proxyFieldCount; ++i) {
                    switch(P::_proxyFields[i].flags) {
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
                        if(currentDepth == (int)startDepth) return i;
                    default: break;
                    }
                }
                return -1;
            };

            for(int i = 0; i <= parentIndex; ++i) { //figure out table depth of parent table
                switch(P::_proxyFields[i].flags) {
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

            if(lua_isstring(l, -1)) {
                strcpy(key, lua_tostring(l, -1));
                lua_pop(l, 1);

                bool found = false;
                currentIndex = parentIndex;
                int currentDepth = depth;
                while((currentIndex = findNextIndexAtDepth(currentIndex, depth, currentDepth)) != -1) {
                    if(strcmp(key, P::_proxyFields[currentIndex].key) == 0) {
                        nextIndex = findNextIndexAtDepth(currentIndex, depth, currentDepth);
                        found = true;
                        break;
                    }
                }

                //Never even found the starting key! Must be invalid, and it ain't NIL!
                if(!found) {
                    assert(0);
                    return 0;
                //We're at the end!
                } else if(nextIndex == -1) {
                    lua_pushnil(l);
                    return 1;
                //Return next key
                } else {
                    //qDebug() << "NEXT KEY " << P::_proxyFields[index+1].key;
                    lua_pushstring(l, P::_proxyFields[nextIndex].key);
                    return 1;
                }
            //assume this is a nil index, and we want the first key.
            } else {
                //qDebug() << "FIRST KEY " << P::_proxyFields[0].key;
                lua_pop(l, 1);
                //depth 0's parent is implicitly -1
                lua_pushstring(l, P::_proxyFields[parentIndex+1].key); //Unless you did something fucktarded (empty table), this should be a valid index
                return 1;
            }
        }

        static inline int _nextMetaMethodCFunction(lua_State*const l) {
            const bool isVirtual = (lua_tointeger(l, lua_upvalueindex(2)) != -1);   //check if entry has a real parent
            const bool isArray   = (!lua_isnil(l, lua_upvalueindex(3)));

            if(isArray) {
                bool firstPass  = lua_isnil(l, -1);
                int index       = firstPass? 0 : lua_tonumber(l, -1);

                lua_getfield(l, -2, "__ud");                //this ptr

                lua_pushvalue(l, lua_upvalueindex(3));      //size function
                lua_pushvalue(l, -2);                       //this ptr

                assert(!lua_isnil(l, -1));
                _protectedCall(1, 1, nullptr);              //get array size

                int size = lua_tointeger(l, -1);

                if(index >= size) {
                    lua_pop(l, lua_gettop(l));
                    return 0;
                } else {
                    lua_pop(l, 1);
                }

                lua_pushinteger(l, ++index);                //new index key
                lua_pushvalue(l, lua_upvalueindex(1));      //get function
                lua_pushvalue(l, -3);                       //this ptr arg1
                lua_pushinteger(l, index-1);                //index arg2 (C arrays are zero based)
                _protectedCall(2, 1, nullptr);              //this->getByIndex(index);

                return 2; // return <key, value> pair

            } else {

                if(lua_isnil(l, -2)) {
                   // qDebug() << "__NEXT called with NIL";
                    lua_pop(l, 2);
                    return 0;
                } else {
                   // qDebug() << "_nextMetaMethodCFunction()";
                }

                if(_getNextKey(l)) {
                   // qDebug() << "getNextKey() - true";
                    lua_pushvalue(l, -1);

                    lua_pushvalue(l, lua_upvalueindex(1));
                    //_propertyProxyIndexTable.push(l);
                    lua_insert(l, -2);
                    lua_rawget(l, -2);

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

        static inline int _setVirtualTableProxy(lua_State* l) {
            lua_pushcfunction(l, &_luaFromTableCFunction);
            lua_pushvalue(l, lua_upvalueindex(1));
            lua_pushvalue(l, lua_upvalueindex(2));
            lua_pushvalue(l, lua_upvalueindex(3));
            lua_pushvalue(l, lua_upvalueindex(4));
            lua_pushcclosure(l, &_createVirtualTableProxy, 4);
            lua_pushvalue(l, -4);
            _protectedCall(1, 1, nullptr);      //create proxy table
            lua_pushvalue(l, -3);               //push valueTable
            _protectedCall(2, 0, nullptr);
            return 0;
        }

        //metatable is upvalue 1
        //userdata ptr is arg 1
        static inline int _createVirtualTableProxy(lua_State* l) {
            lua_newtable(l);
            lua_pushvalue(l, -2);
            lua_setfield(l, -2, "__ud");
            lua_newtable(l);
            lua_pushvalue(l, lua_upvalueindex(1));
            lua_pushvalue(l, lua_upvalueindex(4));
            lua_pushcclosure(l, &_indexVirtualTableProxyMetamethod, 2);
            lua_setfield(l, -2, "__index");
            lua_pushvalue(l, lua_upvalueindex(3));
            lua_pushvalue(l, lua_upvalueindex(4));
            lua_pushcclosure(l, &_newIndexVirtualTableProxyMetamethod, 2);
            lua_setfield(l, -2, "__newindex");
            lua_pushvalue(l, lua_upvalueindex(1));
            lua_pushvalue(l, lua_upvalueindex(2));
            lua_pushvalue(l, lua_upvalueindex(4));
            lua_pushcclosure(l, &_nextMetaMethodCFunction, 3);
            lua_setfield(l, -2, "__next");
            lua_pushvalue(l, lua_upvalueindex(1));
            lua_pushvalue(l, lua_upvalueindex(2));
            lua_pushvalue(l, lua_upvalueindex(4));
            lua_pushcclosure(l, &_pairsMetaMethodCFunction, 3);
            lua_setfield(l, -2, "__pairs");
            //SET ANY OTHER METAMETHODS
            lua_setmetatable(l, -2);

            return 1;
        }

        static inline  void _callPropertyProxyMethod(const bool /*write*/, const unsigned args, const unsigned retVals, const unsigned keyIndex) {
            lua_remove(*_mainState, keyIndex);
            LuaManager::_protectedCall(args, retVals, nullptr);
        }

        static inline void  _registerPropertyMetaMethodTables(lua_State* l) {
            unsigned tableDepth = 0;
            LuaManager::getGlobal(OOLUA::Proxy_class<T>::class_name); //push OOLUA methods table

            //Register property pairs for index table <K=field, V=getFunc>
            lua_newtable(l); //get table
            lua_newtable(l); //set table

            for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {

                switch(P::_proxyFields[i].flags) {
                case LuaTableFieldProxy::FLAG_ARRAY:
                    lua_getfield(l, -3-(tableDepth*2), P::_proxyFields[i].get);     //upvalue #1, getByIndex method
                    assert(!lua_isnil(l, -1)); //LUA GET ACCESSOR NOT FOUND!!!!
                    lua_pushinteger(l, i);                                          //upvalue #2, starting index of virtual table properties
                    if(P::_proxyFields[i].set) {
                        lua_getfield(l, -3-(tableDepth*2)-2, P::_proxyFields[i].set); //upvalue #3, setByIndex method
                        assert(!lua_isnil(l, -1)); //LUA SET ACCESSOR NOT FOUND!!!!
                    } else lua_pushnil(l);
                    lua_getfield(l, -3-(tableDepth*2)-3, P::_proxyFields[i].size);    //upvalue #4 size field for arrays
                    assert(!lua_isnil(l, -1)); //SIZE ACCESSOR NOT FOUND!!!!
                    lua_pushcclosure(l, &_createVirtualTableProxy, 4);
                    lua_setfield(l, -3-(tableDepth*2), P::_proxyFields[i].key);

                    lua_getfield(l, -3-(tableDepth*2), P::_proxyFields[i].get);     //upvalue #1, getByIndex method
                    assert(!lua_isnil(l, -1)); //LUA GET ACCESSOR NOT FOUND!!!!
                    lua_pushinteger(l, i);                                          //upvalue #2, starting index of virtual table properties
                    if(P::_proxyFields[i].set) {
                        lua_getfield(l, -3-(tableDepth*2)-2, P::_proxyFields[i].set); //upvalue #3, setByIndex method
                        assert(!lua_isnil(l, -1)); //LUA SET ACCESSOR NOT FOUND!!!!
                    } else lua_pushnil(l);
                    lua_getfield(l, -3-(tableDepth*2)-3, P::_proxyFields[i].size);    //upvalue #4 size field for arrays
                    assert(!lua_isnil(l, -1)); //SIZE ACCESSOR NOT FOUND!!!!
                    lua_pushcclosure(l, &_setVirtualTableProxy, 4);
                    lua_setfield(l, -3-(tableDepth*2)+1, P::_proxyFields[i].key);

                    break;
                case LuaTableFieldProxy::FLAG_VTABLE_BEGIN:
                    lua_newtable(l); //__index metatable for virtual table
                    lua_newtable(l); //__newindex metatable for virtual table
                    lua_pushvalue(l, -2);                       //upvalue #1, __index for virtual table
                    lua_pushinteger(l, i);                      //upvalue #2, starting index of virtual table properties
                    lua_pushvalue(l, -3);                       //upvalue #3 __newindex for virtual table
                    lua_pushnil(l);                             //upvalue #4 size for arrays (ignored)
                    //upvalue #4 size field for arrays
                    lua_pushcclosure(l, &_createVirtualTableProxy, 4);
                    ++tableDepth;
                    lua_setfield(l, -3-(tableDepth*2), P::_proxyFields[i].key);
                    lua_pushvalue(l, -2);                       //upvalue #1, __index for virtual table
                    lua_pushinteger(l, i);                      //upvalue #2, starting index of virtual table properties
                    lua_pushvalue(l, -3);                       //upvalue #3 __newindex for virtual table
                    lua_pushnil(l);                             //upvalue #4 size for arrays (ignored)
                    lua_pushcclosure(l, &_setVirtualTableProxy, 4);
                    lua_setfield(l, -3-(tableDepth*2)+1, P::_proxyFields[i].key);
                    break;

                case LuaTableFieldProxy::FLAG_VTABLE_END:
                    --tableDepth;
                    lua_pop(l, 2);
                    break;
                default:
                    lua_getfield(l, -3-(tableDepth*2), P::_proxyFields[i].get); //get field from OOLUA methods table
                    assert(!lua_isnil(l, -1)); //LUA GET ACCESSOR NOT FOUND!!!!
                    lua_setfield(l, -3, P::_proxyFields[i].key);
                    if(P::_proxyFields[i].set) {
                        lua_getfield(l, -3-(tableDepth*2), P::_proxyFields[i].set); //get field from OOLUA methods table
                        assert(!lua_isnil(l, -1)); //LUA SET ACCESSOR NOT FOUND!!!!
                        lua_setfield(l, -2, P::_proxyFields[i].key);
                    }
                }
            }
            assert(tableDepth == 0); // YOUR SHIT IS MALFORMED, NIGGER. USE MY DSL CORRECTLY FOR FUCK'S SAKE.

        //                LuaManager::rPrintByIndex(-1, 100, "__INDEX");
            _propertyProxyNewIndexTable.pull(l);
          //              LuaManager::rPrintByIndex(-1, 100, "__NEWINDEX");
            _propertyProxyIndexTable.pull(l);
			lua_pop(l, 1);



		}

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
            if(++index > P::_proxyFieldCount) {
                lua_pop(l, 1);
                lua_pushnil(l);
                return 1;
            }

            lua_pushnumber(l, index);

            _propertyProxyIndexTable.push(l);
            lua_pushstring(l, P::_proxyFields[index-1].key);

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
            lua_pop(l, lua_gettop(l));
            lua_pushinteger(l, P::_proxyFieldCount);
            return 1;
        }

		static int _indexMetaMethodCFunction(lua_State* l) {
			return _metaMethodCFunction(l, _ooluaIndexTable, _propertyProxyIndexTable, false);
		}

		static int _newIndexMetaMethodCFunction(lua_State* l) {
			return _metaMethodCFunction(l, _ooluaNewIndexTable, _propertyProxyNewIndexTable, true);
		}

		static int _pairsMetaMethodCFunction(lua_State* l) {
			//Pushes onto Lua stack:
			//1 - Userdata iterator function
			//2 - Userdata
			//3 - nil (first key)
            //lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _propertyProxyIndexTable.ref());
            lua_pushvalue(*_mainState, lua_upvalueindex(1));
            lua_pushvalue(*_mainState, lua_upvalueindex(2));
            lua_pushvalue(*_mainState, lua_upvalueindex(3));
            lua_pushcclosure(l, &_nextMetaMethodCFunction, 3);
			lua_insert(l, 1);
			lua_pushnil(l);
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

		//CRTP Overridable?
		static int _metaMethodCFunction(lua_State* l, LuaTable& ooluaIndexTable, LuaTable& propertyIndexTable, const bool newIndex) {
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
			ooluaIndexTable.push(l);
            lua_pushvalue(l, newIndex? -3 : -2);
            lua_gettable(l, -2);
			lua_remove(l, -2);

            //key found in OOLUA's methods table
			if(!lua_isnil(l, -1)) {
                lua_remove(l, 1);
                return 1;           //Must have been an OOLUA proxy method lookup? ...I think?

			//key not found, try property proxy tables
			} else {

				lua_remove(l, -1);
				propertyIndexTable.push(l);
                lua_pushvalue(l, newIndex? -3 : -2);
                lua_gettable(l, -2);
				lua_remove(l, -2);

                //key not found here either, we ain't got it -- INSERT THE BITCH IF NEWINDEX, NOTIFY IF ESTK
				if(lua_isnil(l, -1)) {
					lua_pop(l, lua_gettop(l));

                    //retrieve object, notify it of change
                    //instance._onFieldAdded("ADDED");


					return 0;

				} else {
                    //lua_remove(l, 1);
                    lua_insert(l, 1);   //push get/set function to bottom of stack

                    //lua_remove(l, 3);   //remove key from stack ESTK'S METHOD EXPECTS THIS TO BE HERE

                    //userdata is index 2
                   //  OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));

                    //Invoke proxy accessor method
                    LuaTableProxy<P, T>::_callPropertyProxyMethod(newIndex, newIndex? 2 : 1, newIndex? 0 : 1, 3);

                    if(newIndex) {
                        //instance._onFieldChanged("CHANGED");
                        return 0;
                    } else {
                        return 1;
                    }

				}

			}
		}


	public:

		void		setMetaTable(const LuaTable& ref) const;
		void		setProtoTable(const LuaTable& ref) const;
		LuaTable	getProtoTable(void) const;

		//override OOLUA's Proxy Class metatable
		//do any kind of static registration bullshit for fields
		static void registerProxyMetaTable(lua_State *l) {
			_registerPropertyMetaMethodTables(l);

			assert(OOLUA::INTERNAL::class_name_is_already_registered(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name));

			luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name); //userdata metatable


			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__index");
            lua_pushcfunction(l, &_luaFromTableCFunction);
            lua_setfield(l, -2, "fromTable");

            lua_getfield(l, -1, "new"); //only register creator function if constructor is exposed
            if(!lua_isnil(l, -1)) {
                lua_pushstring(l, "oolua_new");
                lua_insert(l, -2);
                lua_rawset(l, -3);
                lua_pushcfunction(l, &_luaCreate);
                lua_setfield(l, -2, "new");
            } else lua_pop(l, 1);

            _ooluaIndexTable.pull(l);

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__newindex");
			_ooluaNewIndexTable.pull(l);

            //LuaManager::_cStackDump();
            LuaTableProxy<P, T>::_addMetamethods(l);
            //LuaManager::_cStackDump();
            //Create a new __next metamethod for internal iteration
            //_propertyProxyIndexTable.push(l);
            //lua_setfield(l, -2, "__properties");

            //lua_pushcfunction(l, &_getNextKey);
            //lua_setfield(l, -2, "__nextkey");

			lua_pop(*LuaManager::_mainState, 1);

            assert(LuaManager::stackDepth() == 0);

		}

        static void _addMetamethods(lua_State* l) {
            //Use our __index metamethod
            lua_pushcfunction(l, &_indexMetaMethodCFunction);
            lua_setfield(l, -2, "__index");

            //Use our __newindex metamethod
            lua_pushcfunction(l, &_newIndexMetaMethodCFunction);
            lua_setfield(l, -2, "__newindex");

            //Use our __pairs metamethod
            lua_rawgeti(l, LUA_REGISTRYINDEX, _propertyProxyIndexTable.ref());
            lua_pushinteger(l, -1);
            lua_pushnil(l);
            lua_pushcclosure(l, &_pairsMetaMethodCFunction, 3);
            lua_setfield(l, -2, "__pairs");

            lua_pushcfunction(l, &_ipairsMetaMethodCFunction);
            lua_setfield(l, -2, "__ipairs");

            //Create a new __next metamethod for internal iteration
            //lua_pushcfunction(l, &_nextMetaMethodCFunction);
            //lua_setfield(l, -2, "__next");

            lua_pushcfunction(l, &_lengthMetaMethodCFunction);
            lua_setfield(l, -2, "__len");

            lua_pushstring(l, OOLUA::Proxy_class<T>::class_name);
            lua_setfield(l, -2, "__typename");

        }

        static int _luaFromTableCFunction(lua_State*const l) {
            //STACK[-2] = proxy table/ud
            //STACK[-1] = value table

            int startTop = lua_gettop(l)-1; //proxy table absolute index


            //_cStackDump();

#ifdef ELYSIAN_LUA_DEBUG
            char keyString[ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE];
            char valString[ELYSIAN_LUA_PROXY_FIELD_KEY_MAX_SIZE];

            dbgprintf("%s:fromTable({", OOLUA::Proxy_class<T>::class_name);
            dbgpush();
#endif

            genericIterate(-1, [&](void) {

               // _cStackDump();

                lua_pushvalue(l, -2);           //push key copy

                //==== DEBUG ====
#ifdef ELYSIAN_LUA_DEBUG
                lua_pushvalue(l, -1);
                strcpy(keyString, luaL_tolstring(l, -1, nullptr));
                lua_pop(l, 2);
#endif
                //==== /DEBUG ====


                //lua_insert(l, -2);            //move key behind value
                lua_pushvalue(l, -2);           //push value

                //==== DEBUG ====
#ifdef ELYSIAN_LUA_DEBUG
                lua_pushvalue(l, -1);
                strcpy(valString, luaL_tolstring(l, -1, nullptr));
                lua_pop(l, 2);
                dbgprintf("<%s, %s>", keyString, valString);
#endif

               // _cStackDump();
                lua_settable(l, startTop);      //set new value in proxy table

                //_cStackDump();

            });

#ifdef ELYSIAN_LUA_DEBUG
            dbgpop();
            dbgprintf("})");
#endif
            lua_pop(l, 2);

            return 0;
        }

        void fromTable(OOLUA::Lua_table_ref ref) {
            _initializing = true;
            lua_State*const state = *LuaManager::_mainState;

            OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));
            //OOLUA::push(state, static_cast<T*>(this));
            ref.push(state);

            lua_pushcfunction(state, &_luaFromTableCFunction);
            lua_insert(state, -3);
            LuaManager::_protectedCall(2, 0, nullptr);
            _initializing = false;
        }

        static int _luaCreate(lua_State*const l) {
            OOLUA::Lua_table_ref valueTable;
            int args = lua_gettop(l);

            if(args >= 2) {
                OOLUA::pull(l, valueTable);
            }

            _ooluaIndexTable.push(l);
            lua_getfield(l, -1, "oolua_new");
            lua_remove(l, -2);
            assert(!lua_isnil(l, -1)); //better fucking have a constructor if you invoked this method
            _protectedCall(0, 1, nullptr);

            if(args >= 2) {
                T* obj;
                OOLUA::pull(l, obj);
                obj->fromTable(valueTable);
                OOLUA::push(l, obj);
            }

            return 1;
        }

	};

/*
	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_staticMetaTable;
*/
	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_ooluaIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_ooluaNewIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_propertyProxyIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_propertyProxyNewIndexTable;

    template <typename P, typename T>
    bool LuaTableProxyBase<P, T>::_initializing = false;

}

#endif // ELYSIAN_LUA_TABLE_PROXY_H
